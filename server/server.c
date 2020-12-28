#include <sqlite3.h>
#include <stdio.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(void) {
    
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    
    rc = sqlite3_open("server.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    sql = "CREATE TABLE IF NOT EXISTS USER("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "PASSWORD       TEXT    NOT NULL" \
        ");";
    
    sql = "CREATE TABLE IF NOT EXISTS CHANNEL("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL" \
        ");";
    
    sql = "CREATE TABLE IF NOT EXISTS POST("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "CONTENT        TEXT        NOT NULL" \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)" \
        ");";
    
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }    
    
    sqlite3_close(db);
    
    return 0;
}
