#include <cstdio>
#include <cstdlib>

// For client & server shared code, indicate we are the compiler
#define COURIER_SERVER

#include "server.h"
#define SW_IMPLEMENTATION
#include "util.h"
#include "debug.h"
// TODO 9-8-2019: remove netvar
#include "netvar.h"

#define DEFAULT_PORT 37774
struct ProgArgs {
    size_t port = DEFAULT_PORT;
};

ProgArgs process_prog_args (int argc, char** argv) {
    ProgArgs args;
    for(int i = 1; i < argc; i++) {
	if(argv[i][0] == '-') {
	    switch(argv[i][1]) {
	    case 'p': i++; args.port = atoi(argv[i]); break;
	    default: LOG_ERROR("Unknown argument [%s]\n", argv[i]); break; // TODO 9-6-2019: this can lead to buffer overflow... just saying.
	    }
	} else {
	    LOG_ERROR("Unknown argument [%s]\n", argv[i]);
	}
    }

    return args;
}

int main (int argc, char** argv) {
    ProgArgs args = process_prog_args(argc, argv);

    NetworkEntity e;
    e.test2 = 42;
    e.test = 1234.0f;
    printf("%s\n", e.serialize_changed_state().c_str());
    std::string data = "1_4444";
    e.deserialize_state(data);
    printf("test2: [%i]\n", (int)e.test2);

    if(!init_server(args.port))
	return EXIT_FAILURE;

    run_server();
    
    exit_server();

    return EXIT_SUCCESS;
}
