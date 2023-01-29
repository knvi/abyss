#ifndef SERVER_H
#define SERVER_H

#include <ulfius.h>
#include "../db/database.h"
#include "endpoint.h"

struct server {
    struct _u_instance instance;
    struct database *db;
};

int server_init(struct server *server, unsigned int port);
void server_start(struct server *server);
void server_stop(struct server *server);

#endif