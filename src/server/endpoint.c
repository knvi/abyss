#include "endpoint.h"
#include <stdio.h>
#include <string.h>
#include <ulfius.h>
#include <jansson.h>

// GET /todos
int endpoint_get_todos(const struct _u_request *request, struct _u_response *response, void *user_data) {
    struct database *db = (struct database *)user_data;
    int num_todos = database_get_todo_count(db);
    struct todo *todos = malloc(sizeof(struct todo) * num_todos);

    if(database_get_todos(db, todos) < 0) {
        ulfius_set_string_body_response(response, 500, "Error getting todos");
        return U_CALLBACK_ERROR;
    }
    
    json_t *json_todos = json_array();
    for(int i = 0; i < num_todos; i++) {
        json_t *json_todo = json_object();
        json_object_set_new(json_todo, "id", json_integer(todos[i].id));
        json_object_set_new(json_todo, "title", json_string(todos[i].title));
        json_object_set_new(json_todo, "description", json_string(todos[i].description));
        json_array_append_new(json_todos, json_todo);
    }

    char *json_todos_str = json_dumps(json_todos, JSON_INDENT(2));
    ulfius_set_string_body_response(response, 200, json_todos_str);

    free(json_todos_str);
    json_decref(json_todos);
    free(todos);

    return U_CALLBACK_CONTINUE;
}

// GET /todos/:id
int endpoint_get_todo(const struct _u_request *request, struct _u_response *response, void *user_data) {
    struct database *db = (struct database *)user_data;
    int id = atoi(u_map_get(request->map_url, "id"));
    printf("ok\n");
    struct todo todo;
    if(database_get_todo(db, id, &todo) < 0) {
        ulfius_set_string_body_response(response, 500, "Error getting todo");
        return U_CALLBACK_ERROR;
    }

    json_t *json_todo = json_object();
    json_object_set_new(json_todo, "id", json_integer(todo.id));
    json_object_set_new(json_todo, "title", json_string(todo.title));
    json_object_set_new(json_todo, "description", json_string(todo.description));

    char *json_todo_str = json_dumps(json_todo, JSON_INDENT(2));
    ulfius_set_string_body_response(response, 200, json_todo_str);

    free(json_todo_str);
    json_decref(json_todo);

    return U_CALLBACK_CONTINUE;
}

// POST /todos
int endpoint_post_todo(const struct _u_request *request, struct _u_response *response, void *user_data) {
    struct database *db = (struct database *)user_data;
    struct todo todo;

    json_error_t error;
    json_t *json_todo = json_loads(request->binary_body, 0, &error);
    if(json_todo == NULL) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return U_CALLBACK_ERROR;
    }

    const json_t *json_title = json_object_get(json_todo, "title");
    const json_t *json_description = json_object_get(json_todo, "description");
    if(json_title == NULL || json_description == NULL) {
        fprintf(stderr, "Error parsing JSON: missing title or description\n");
        return U_CALLBACK_ERROR;
    }

    todo.title = strdup(json_string_value(json_title));
    todo.description = strdup(json_string_value(json_description));


    if(database_insert_todo(db, &todo) != 0) {
        fprintf(stderr, "Error inserting todo into database\n");
        return U_CALLBACK_ERROR;
    }

    json_t *json_response = json_object();
    json_object_set_new(json_response, "id", json_integer(todo.id));

    char *json_response_str = json_dumps(json_response, JSON_INDENT(2));
    ulfius_set_string_body_response(response, 200, json_response_str);

    free(json_response_str);
    json_decref(json_response);

    return U_CALLBACK_CONTINUE;
}

// PUT /todos/:id
int endpoint_put_todo(const struct _u_request *request, struct _u_response *response, void *user_data) {
    struct database *db = (struct database *)user_data;
    struct todo todo;

    int id = atoi(u_map_get(request->map_url, "id"));
    if(database_get_todo(db, id, &todo) != 0) {
        fprintf(stderr, "Error getting todo from database\n");
        return U_CALLBACK_ERROR;
    }

    json_error_t error;
    json_t *json_todo = json_loads(request->binary_body, 0, &error);
    if(json_todo == NULL) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return U_CALLBACK_ERROR;
    }

    json_t *json_title = json_object_get(json_todo, "title");
    json_t *json_description = json_object_get(json_todo, "description");
    if(json_title == NULL || json_description == NULL) {
        fprintf(stderr, "Error parsing JSON: missing title or description\n");
        return U_CALLBACK_ERROR;
    }

    todo.title = strdup(json_string_value(json_title));
    todo.description = strdup(json_string_value(json_description));

    if(database_update_todo(db, id, &todo) != 0) {
        fprintf(stderr, "Error updating todo in database\n");
        return U_CALLBACK_ERROR;
    }

    json_t *json_response = json_object();
    json_object_set_new(json_response, "id", json_integer(todo.id));

    char *json_response_str = json_dumps(json_response, JSON_INDENT(2));
    ulfius_set_string_body_response(response, 200, json_response_str);

    free(json_response_str);
    json_decref(json_response);

    return U_CALLBACK_CONTINUE;
}

// DELETE /todos/:id
int endpoint_delete_todo(const struct _u_request *request, struct _u_response *response, void *user_data) {
    struct database *db = (struct database *)user_data;

    int id = atoi(u_map_get(request->map_url, "id"));
    if(database_delete_todo(db, id) != 0) {
        fprintf(stderr, "Error deleting todo from database\n");
        return U_CALLBACK_ERROR;
    }

    ulfius_set_empty_body_response(response, 200);

    return U_CALLBACK_CONTINUE;
}