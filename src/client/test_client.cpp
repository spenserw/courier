#include <cstdlib>

#include "debug.h"

#define LINUX
#define GNUC
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#define DEFAULT_SERVER_PORT 25575

static ISteamNetworkingSockets* p_interface;
static HSteamNetConnection server_conn;
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

bool init_client () {
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

int main (int argc, char** argv) {

    init_client();

    char server_address_cstr[ SteamNetworkingIPAddr::k_cchMaxString ];
    SteamNetworkingIPAddr server_address;
    server_address.Clear();
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
	    if (!server_address.ParseString(argv[i])) {
		LOG_ERROR( "Invalid server address '%s'\n", argv[i]);
		return EXIT_FAILURE;
	    }
	    if (server_address.m_port == 0)
		server_address.m_port = DEFAULT_SERVER_PORT;
	    continue;
	}
    }

    if(server_address.IsIPv6AllZeros()) {
	LOG_ERROR("Invalid server address!\n");
	return EXIT_FAILURE;
    }

    server_address.ToString( server_address_cstr, sizeof(server_address_cstr), true );
    LOG_INFO("Connecting to server @ [%s]...\n", server_address_cstr);
    server_conn = p_interface->ConnectByIPAddress( server_address);
    if (server_conn == k_HSteamNetConnection_Invalid) {
	LOG_ERROR("Failed to connect to server!\n");
	return EXIT_FAILURE;
    }
    LOG_INFO("Successfully connected!\n");

    return EXIT_SUCCESS;
}
