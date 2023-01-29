#include "db/database.h"
#include "server/server.h"
#include <stdio.h>

int main(int argc, char **argv) {
    struct server server;

    if(server_init(&server, 8080) != 0) {
        fprintf(stderr, "Error initializing server\n");
        return 1;
    }

    server_start(&server);

    server_stop(&server);

    return 0;
}