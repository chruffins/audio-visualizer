#include "database.h"

int init_database(sqlite3* db) {
    char* err_msg = NULL;
    int retcode = sqlite3_exec(db, INIT_SQL, 0, 0, &err_msg);

    if (retcode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}