#include <cstdlib>
#include <cassert>

#include "debug.h"

#define LINUX
#define GNUC
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingtypes.h>

#define DEFAULT_SERVER_PORT 25575

static ISteamNetworkingSockets* pInterface;
static HSteamNetConnection hConn;
static SteamNetworkingMicroseconds g_log_time_zero;

static bool bQuit = false;

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

static void OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t *pInfo) {
  LOG_INFO("Status changed!\n");
  assert(pInfo->m_hConn == hConn || hConn == k_HSteamNetConnection_Invalid);

  // What's the state of the connection?
  switch (pInfo->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None:
    // NOTE: We will get callbacks here when we destroy connections.  You can
    // ignore these.
    break;

  case k_ESteamNetworkingConnectionState_ClosedByPeer:
  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
    bQuit = true;

    // Print an appropriate message
    if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
      // Note: we could distinguish between a timeout, a rejected connection,
      // or some other transport problem.
      LOG_ERROR(
          "We sought the remote host, yet our efforts were met with defeat. "
          " (%s)",
          pInfo->m_info.m_szEndDebug);
    } else if (pInfo->m_info.m_eState ==
               k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
      LOG_ERROR(
          "Alas, troubles beset us; we have lost contact with the host.  (%s)",
          pInfo->m_info.m_szEndDebug);
    } else {
      // NOTE: We could check the reason code for a normal disconnection
      LOG_INFO("The host hath bidden us farewell.  (%s)",
               pInfo->m_info.m_szEndDebug);
    }

    // Clean up the connection.  This is important!
    // The connection is "closed" in the network sense, but
    // it has not been destroyed.  We must close it on our end, too
    // to finish up.  The reason information do not matter in this case,
    // and we cannot linger because it's already closed on the other end,
    // so we just pass 0's.
    pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
    hConn = k_HSteamNetConnection_Invalid;
    break;
  }

  case k_ESteamNetworkingConnectionState_Connecting:
    // We will get this callback when we start connecting.
    // We can ignore this.
    LOG_INFO("Connecting....\n");
    break;

  case k_ESteamNetworkingConnectionState_Connected:
    LOG_INFO("Connected to server OK");
    break;

  default:
    // Silences -Wswitch
    break;
  }
}

bool init_client () {
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

void pollState() {
    pInterface->RunCallbacks();
}

int main (int argc, char** argv) {

    init_client();

    char server_address_cstr[ SteamNetworkingIPAddr::k_cchMaxString ];
    SteamNetworkingIPAddr serverAddress;
    serverAddress.Clear();
    int32_t port;

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "--port")) {
            ++i;
            if(i >= argc) {
                LOG_ERROR("No port number probided for flag '--port'\n");
                return EXIT_FAILURE;
            }

            port = atoi(argv[i]);
            if ( port <= 0 || port > 65535 ) {
                LOG_ERROR("Invalid port [%d]", port);
                return EXIT_FAILURE;
            }
            continue;
        }
        // Anything else, must be server address to connect to
        else {
            if (!serverAddress.ParseString(argv[i])) {
                LOG_ERROR( "Invalid server address '%s'\n", argv[i]);
                return EXIT_FAILURE;
            }
            if (serverAddress.m_port == 0)
                serverAddress.m_port = DEFAULT_SERVER_PORT;
            continue;
        }
    }

    if(serverAddress.IsIPv6AllZeros()) {
        LOG_ERROR("Invalid server address!\n");
        return EXIT_FAILURE;
    }

    serverAddress.ToString( server_address_cstr, sizeof(server_address_cstr), true );
    LOG_INFO("Connecting to server @ [%s]...\n", server_address_cstr);
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)OnSteamNetConnectionStatusChanged);
    hConn = pInterface->ConnectByIPAddress( serverAddress, 1, &opt );
    if (hConn == k_HSteamNetConnection_Invalid) {
        LOG_ERROR("Failed to connect to server!\n");
        return EXIT_FAILURE;
    }
    LOG_INFO("Successfully connected!\n");

    bool messageSent = false;
    LOG_INFO("Sending message...\n");
    std::string msg = "Hello, Courier!";

    while(!bQuit) {
        pollState();

        if(!messageSent) {
            pInterface->SendMessageToConnection(hConn, msg.c_str(), (uint32)msg.size(), k_nSteamNetworkingSend_Reliable, nullptr);
            messageSent = true;
        }
    }

    return EXIT_SUCCESS;
}
