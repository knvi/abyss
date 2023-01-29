#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int database_make_todo_table(struct database *db) {
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sql = "CREATE TABLE todo("  \
          "id INTEGER PRIMARY KEY AUTOINCREMENT," \
          "title TEXT NOT NULL," \
          "description TEXT NOT NULL);";

    rc = sqlite3_exec(db->db, sql, 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 1;
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    return 0;
}

struct database *database_open(const char *path) {
    struct database *db = malloc(sizeof(struct database));

    int rc = sqlite3_open(path, &db->db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db->db));
        return NULL;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    char *zErrMsg = 0;
    char *sql;
    int table_exists = 0;

    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='todo';";
    rc = sqlite3_exec(db->db, sql, 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    if (table_exists == 0) {
        database_make_todo_table(db);
    }

    return db;
}

void database_close(struct database *db) {
    sqlite3_close(db->db);
    free(db);
}

int database_insert_todo(struct database *db, struct todo *todo) {
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sql = "INSERT INTO todo (title, description) VALUES (?, ?);";

    rc = sqlite3_prepare_v2(db->db, sql, -1, &db->stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_bind_text(db->stmt, 1, todo->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(db->stmt, 2, todo->description, -1, SQLITE_STATIC); 

    rc = sqlite3_step(db->stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_finalize(db->stmt);
    
    return 0;
}

int database_update_todo(struct database *db, int id, struct todo *todo) {
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sql = "UPDATE todo SET title = ?, description = ? WHERE id = ?;";

    rc = sqlite3_prepare_v2(db->db, sql, -1, &db->stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_bind_text(db->stmt, 1, todo->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(db->stmt, 2, todo->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(db->stmt, 3, id);

    rc = sqlite3_step(db->stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_finalize(db->stmt);

    return 0;
}

int database_delete_todo(struct database *db, int id) {
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sql = "DELETE FROM todo WHERE id = ?;";

    rc = sqlite3_prepare_v2(db->db, sql, -1, &db->stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_bind_int(db->stmt, 1, id);

    rc = sqlite3_step(db->stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_finalize(db->stmt);

    return 0;
}

int database_get_todo_count(struct database *db) {
    char *zErrMsg = 0;
    int rc;
    char *sql;
    int count = 0;

    sql = "SELECT COUNT(*) FROM todo;";

    rc = sqlite3_prepare_v2(db->db, sql, -1, &db->stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    rc = sqlite3_step(db->stmt);

    if (rc == SQLITE_ROW) {
        count = sqlite3_column_int(db->stmt, 0);
    }

    sqlite3_finalize(db->stmt);

    return count;
}

int database_get_todo(struct database *db, int id, struct todo *todo) {
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sql = "SELECT * FROM todo WHERE id = ?;";

    rc = sqlite3_prepare_v2(db->db, sql, -1, &db->stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    sqlite3_bind_int(db->stmt, 1, id);

    rc = sqlite3_step(db->stmt);

    if (rc == SQLITE_ROW) {
        todo->id = sqlite3_column_int(db->stmt, 0);
        todo->title = (char *) malloc(strlen((char *) sqlite3_column_text(db->stmt, 1)) + 1);
        strcpy(todo->title, (char *) sqlite3_column_text(db->stmt, 1));
        todo->description = (char *) malloc(strlen((char *) sqlite3_column_text(db->stmt, 2)) + 1);
        strcpy(todo->description, (char *) sqlite3_column_text(db->stmt, 2));
    }

    sqlite3_finalize(db->stmt);

    return 0;
}

int database_get_todos(struct database *db, struct todo *todos) {
    char *zErrMsg = 0;
    int rc;
    char *sql;
    int i = 0;

    sql = "SELECT * FROM todo;";

    rc = sqlite3_prepare_v2(db->db, sql, -1, &db->stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_free(zErrMsg);
        return 1;
    }

    while (sqlite3_step(db->stmt) == SQLITE_ROW) {
        todos[i].id = sqlite3_column_int(db->stmt, 0);
        todos[i].title = (char *) malloc(strlen((char *) sqlite3_column_text(db->stmt, 1)) + 1);
        strcpy(todos[i].title, (char *) sqlite3_column_text(db->stmt, 1));
        todos[i].description = (char *) malloc(strlen((char *) sqlite3_column_text(db->stmt, 2)) + 1);
        strcpy(todos[i].description, (char *) sqlite3_column_text(db->stmt, 2));
        i++;
    }

    sqlite3_finalize(db->stmt);

    return 0;
}