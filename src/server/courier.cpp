#include <cstdio>
#include <cstdlib>

#include "server.h"
#include "debug.h"

struct EntitySnapshot {
};

struct Packet {
    uint16_t id;
};

int main () {
    if(!init_server(25565))
	return EXIT_FAILURE;

    run_server();
    
    exit_server();

    return EXIT_SUCCESS;
}
