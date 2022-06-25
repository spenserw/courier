#include "server.h"

#include <cstdio>
// #include <mutex>
// #include <queue>
#include <string>
#include <cassert>

#include "debug.h"

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

static ISteamNetworkingSockets* p_interface;
static HSteamListenSocket listen_sock;
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

bool init_server (uint16_t port) {
	server_port = port;

	SteamDatagramErrMsg errMsg;

	if( !GameNetworkingSockets_Init(nullptr, errMsg) ) {
		LOG_ERROR("GameNetworkingSockets failed to initialize: %s\n", errMsg);
		return false;
	}

	g_log_time_zero = SteamNetworkingUtils()->GetLocalTimestamp();
	SteamNetworkingUtils()->SetDebugOutputFunction( k_ESteamNetworkingSocketsDebugOutputType_Msg, debug_steam_socket_func);

	p_interface = SteamNetworkingSockets();

	return true;
}

void process_message (ISteamNetworkingMessage* p_msg) {
	LOG_INFO("Message received: %s\n", static_cast<char*>(p_msg->m_pData));
}

void poll_socket_messages () {
	bool b_should_listen = server_state.b_running;
	while(b_should_listen) {

		ISteamNetworkingMessage *p_incoming_msg = nullptr;
		const int msg_count = p_interface->ReceiveMessagesOnListenSocket(listen_sock, &p_incoming_msg, 1);
		if (msg_count == 0)
			break;
		if ( msg_count < 0 )
			LOG_ERROR("Failed to receive message from listen socket!\n");
		assert( msg_count == 1 && p_incoming_msg);

		process_message(p_incoming_msg);

		p_incoming_msg->Release();

		// Has another thread asked us to quit?
		// first check we haven't already decided to stop
		if(b_should_listen) {
			b_should_listen = server_state.b_running;
		}
	}
}

void run_server () {
	server_state.b_running = true;

	SteamNetworkingIPAddr server_local_addr;
	server_local_addr.Clear();
	server_local_addr.m_port = server_port;
	listen_sock = p_interface->CreateListenSocketIP(server_local_addr);
	if (listen_sock == k_HSteamListenSocket_Invalid) {
		LOG_ERROR("Failed to create listen socket on port [%i]\n", server_port);
		return;
	}
	LOG_INFO("Server listening on port [%i]\n", server_port);

	while(server_state.b_running) {
		poll_socket_messages();
	}
}

void exit_server () {
	LOG_INFO("Exiting...\n");
	GameNetworkingSockets_Kill();
}
