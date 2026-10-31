#include "app.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#include "raylib.h" // must be included befor nob_addons.h bcause it loads logging initialization

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

#include <cstdlib>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void usage(FILE *stream) {
	fprintf(stream, "Usage: ./chess-engine-battel [OPTIONS]\n");
	fprintf(stream, "OPTIONS:\n");
	flag_print_options(stream);
}

int main(int argc, char **argv) {
	addon_init_logging();

	bool help = false;
	char *server_address = "0.0.0.0";
	uint64_t server_port = 14318;

	flag_bool_var(&help, "-h", false, "Show this help message");
	flag_bool_var(&help, "--help", false, "Show this help message");

	flag_str_var(&server_address, "-server-address", server_address, "The address of the websocket server to connect to");
	flag_str_var(&server_address, "a", server_address, "The address of the websocket server to connect to");
	flag_uint64_var(&server_port, "-server-port", server_port, "The port of the websocket server to connect to");
	flag_uint64_var(&server_port, "p", server_port, "The port of the websocket server to connect to");

	if (!flag_parse(argc, argv)) {
		usage(stderr);
		flag_print_error(stderr);
		exit(1);
	}

	argc = flag_rest_argc();
	argv = flag_rest_argv();

	if (help) {
		usage(stdout);
		exit(0);
	}

	App app = App();
	app.setWebsocketServerAddress(server_address);
	app.setWebsocketServerPort(server_port);
	app.startGame();
	app.main_loop();

	return 0;
}
