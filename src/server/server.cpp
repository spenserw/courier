#include "server.h"

#include <cstdio>
// #include <mutex>
// #include <queue>
#include <string>
#include <cassert>

#include "debug.h"
#include "steam/steamnetworkingtypes.h"

// TODO 9-6-2019: handle other OS cases
#define LINUX
#define GNUC
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

//////////////////////////////////////////
// STATE
//////////////////////////////////////////
// Note: various vars commented out, used for multi-threading later
static uint16_t server_port;
// static std::mutex server_state_mutex;
static struct ServerState {
    bool b_running;
} server_state;
// static std::mutex message_queue_mutex;
// static std::queue<ISteamNetworkingMessage> message_queue;

static ISteamNetworkingSockets* pInterface;
static HSteamListenSocket hListenSock;
static HSteamNetPollGroup hPollGroup;

static SteamNetworkingMicroseconds g_log_time_zero;

static void debug_steam_socket_func ( ESteamNetworkingSocketsDebugOutputType error_type, const char *msg) {
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - g_log_time_zero;
    printf( "%10.6f %s\n", time*1e-6, msg);
    fflush(stdout);
    if ( error_type == k_ESteamNetworkingSocketsDebugOutputType_Bug ) {
        fflush(stdout);
        fflush(stderr);
        // kill( getpid(), SIGKILL );
    }
}

static void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo ) {
  // What's the state of the connection?
  switch (pInfo->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None:
    // NOTE: We will get callbacks here when we destroy connections.  You can
    // ignore these.
    break;

  case k_ESteamNetworkingConnectionState_ClosedByPeer:
  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
      LOG_ERROR("Connection problem!\n");
    // Ignore if they were not previously connected.  (If they disconnected
    // before we accepted the connection.)
    if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
      // Clean up the connection.  This is important!
      // The connection is "closed" in the network sense, but
      // it has not been destroyed.  We must close it on our end, too
      // to finish up.  The reason information do not matter in this case,
      // and we cannot linger because it's already closed on the other end,
      // so we just pass 0's.
      pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
      break;
    }

  case k_ESteamNetworkingConnectionState_Connecting: {
    // // This must be a new connection
    // assert(m_mapClients.find(pInfo->m_hConn) == m_mapClients.end());

    LOG_INFO("Connection request from %s\n",
             pInfo->m_info.m_szConnectionDescription);

    // A client is attempting to connect
    // Try to accept the connection.
    if (pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
      // This could fail.  If the remote host tried to connect, but then
      // disconnected, the connection may already be half closed.  Just
      // destroy whatever we have on our side.
      pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
      LOG_ERROR("Can't accept connection.  (It was already closed?)\n");
      break;
    }

    // Assign the poll group
    if (!pInterface->SetConnectionPollGroup(pInfo->m_hConn, hPollGroup)) {
      pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
      LOG_ERROR("Failed to set poll group?\n");
      break;
    }

    break;
  }

  case k_ESteamNetworkingConnectionState_Connected:
    // We will get a callback immediately after accepting the connection.
    // Since we are the server, we can ignore this, it's not news to us.
    break;

  default:
    // Silences -Wswitch
    break;
  }
  }
}

bool init_server (uint16_t port) {
    server_port = port;

    SteamDatagramErrMsg errMsg;

    if( !GameNetworkingSockets_Init(nullptr, errMsg) ) {
        LOG_ERROR("GameNetworkingSockets failed to initialize: %s\n", errMsg);
        return false;
    }

    g_log_time_zero = SteamNetworkingUtils()->GetLocalTimestamp();
    SteamNetworkingUtils()->SetDebugOutputFunction( k_ESteamNetworkingSocketsDebugOutputType_Msg, debug_steam_socket_func);

    pInterface = SteamNetworkingSockets();

    return true;
}

void process_message (ISteamNetworkingMessage* pMsg) {
    std::string msg;
    msg.assign((const char *)pMsg->m_pData, pMsg->m_cbSize);
    LOG_INFO("Message received: %s\n", msg.c_str());
    pMsg->Release();
}

void poll_socket_messages () {
    bool b_should_listen = server_state.b_running;
    while(b_should_listen) {
        ISteamNetworkingMessage *pIncomingMessage = nullptr;
        const int msg_count = pInterface->ReceiveMessagesOnPollGroup(hPollGroup, &pIncomingMessage, 1);
        if (msg_count == 0)
            break;
        if (msg_count < 0)
            LOG_ERROR("Failed to receive message from listen socket!\n");
        assert(msg_count == 1 && pIncomingMessage);

        process_message(pIncomingMessage);

        // Has another thread asked us to quit?
        // first check we haven't already decided to stop
        if(b_should_listen) {
            b_should_listen = server_state.b_running;
        }
    }
}

void pollState() {
    pInterface->RunCallbacks();
}

void run_server () {
    server_state.b_running = true;

    SteamNetworkingIPAddr server_local_addr;
    server_local_addr.Clear();
    server_local_addr.m_port = server_port;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)OnSteamNetConnectionStatusChanged);
    hListenSock = pInterface->CreateListenSocketIP(server_local_addr, 1, &opt);
    if (hListenSock == k_HSteamListenSocket_Invalid) {
        LOG_ERROR("Failed to create listen socket on port [%i]\n", server_port);
        return;
    }
    hPollGroup = pInterface->CreatePollGroup();
    if(hPollGroup == k_HSteamNetPollGroup_Invalid) {
        LOG_ERROR("Failed to initialize poll group!");
        return;
    }
    LOG_INFO("Server listening on port [%i]\n", server_port);

    while(server_state.b_running) {
        pollState();
        poll_socket_messages();
    }
}

void exit_server () {
    LOG_INFO("Exiting...\n");

    pInterface->CloseListenSocket(hListenSock);
    hListenSock = k_HSteamListenSocket_Invalid;

    pInterface->DestroyPollGroup(hPollGroup);
    hPollGroup = k_HSteamNetPollGroup_Invalid;

    GameNetworkingSockets_Kill();
}
