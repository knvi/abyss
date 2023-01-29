#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "../todo.h"

struct database {
    sqlite3 *db;
    sqlite3_stmt *stmt;
};

struct database *database_open(const char *path);
void database_close(struct database *db);

int database_insert_todo(struct database *db, struct todo *todo);
int database_update_todo(struct database *db, int id, struct todo *todo);
int database_delete_todo(struct database *db, int id);

int database_get_todo_count(struct database *db);
int database_get_todo(struct database *db, int id, struct todo *todo);
int database_get_todos(struct database *db, struct todo *todos);

#endif