#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "server.h"
#include <ulfius.h>
#include "../todo.h"

int endpoint_get_todos(const struct _u_request *request, struct _u_response *response, void *user_data);
int endpoint_get_todo(const struct _u_request *request, struct _u_response *response, void *user_data);
int endpoint_post_todo(const struct _u_request *request, struct _u_response *response, void *user_data);
int endpoint_put_todo(const struct _u_request *request, struct _u_response *response, void *user_data);
int endpoint_delete_todo(const struct _u_request *request, struct _u_response *response, void *user_data);

#endif
