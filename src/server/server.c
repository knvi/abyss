#include "server.h"
#include "endpoint.h"
#include "../db/database.h"
#include <stdio.h>
#include <stdlib.h>

int server_init(struct server *server, unsigned int port) {
    server->db = database_open("todos.db");

    if(ulfius_init_instance(&server->instance, port, NULL, NULL) != U_OK) {
        fprintf(stderr, "Error initializing ulfius instance\n");
        return 1;
    }

    ulfius_add_endpoint_by_val(&server->instance, "GET", "/todos", NULL, 0, &endpoint_get_todos, server->db);
    ulfius_add_endpoint_by_val(&server->instance, "GET", "/todos", "/:id", 0, &endpoint_get_todo, server->db);
    ulfius_add_endpoint_by_val(&server->instance, "POST", "/todos", NULL, 0, &endpoint_post_todo, server->db);
    ulfius_add_endpoint_by_val(&server->instance, "PUT", "/todos", "/:id", 0, &endpoint_put_todo, server->db);
    ulfius_add_endpoint_by_val(&server->instance, "DELETE", "/todos", "/:id", 0, &endpoint_delete_todo, server->db);

    return 0;
}

void server_start(struct server *server) {
    if(ulfius_start_framework(&server->instance) == U_OK) {
        printf("🚀 Server initialized successfully on port %d\n", server->instance.port);

        printf("Press enter to stop the server.\n");
        getchar();
    } else {
        fprintf(stderr, "Error starting server\n");
    }
}

void server_stop(struct server *server) {
    printf("🛑 Stopping server\n");

    ulfius_stop_framework(&server->instance);
    ulfius_clean_instance(&server->instance);
    database_close(server->db);

    printf("🛑 Server stopped\n");
}

