#include "database.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int createDatabase(ServerStatus *status) {
    int error = sqlite3_open("server.db", &status->db);
    if (error != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(status->db));
        sqlite3_close(status->db);
    }
    return error;
}

int createTableCheck(ServerStatus *status, char *sqlStatement, char *tableName) {
    int error;
    char *errMsg = 0;

    error = sqlite3_exec(status->db, sqlStatement, NULL, 0, &errMsg);
    if( error != SQLITE_OK ){
        fprintf(stderr, "SQL error during creating table %s: %s\n", tableName, errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(status->db);
    }
    return error;
}

int createTables(ServerStatus *status) {

    char *sqlStatement;
    int error;

    // Creating table USER
    sqlStatement = "CREATE TABLE IF NOT EXISTS USER("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "PASSWORD       TEXT    NOT NULL" \
        ");";

    error = createTableCheck(status, sqlStatement, "USER");
    if (error != SQLITE_OK) {
        return error;
    }


    // Creating table CHANNEL
    sqlStatement = "CREATE TABLE IF NOT EXISTS CHANNEL("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL" \
        ");";

    error = createTableCheck(status, sqlStatement, "CHANNEL");
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating table USER_CHANNEL
    sqlStatement = "CREATE TABLE IF NOT EXISTS USER_CHANNEL("  \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "PRIMARY KEY(USER_ID, CHANNEL_ID)," \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)" \
        ");";

    error = createTableCheck(status, sqlStatement, "USER_CHANNEL");
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating table POST
    sqlStatement = "CREATE TABLE IF NOT EXISTS POST("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "CONTENT        TEXT        NOT NULL," \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)" \
        ");";

    error = createTableCheck(status, sqlStatement, "POST");
    return error;
}


