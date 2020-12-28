#include <sqlite3.h>
#include <stdio.h>
#include "structures.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int createTables();

int main(void) {
    
    ServerStatus status;
    int error;
    
    // Creating database
    error = sqlite3_open("server.db", &status.db);
    if (error != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(status.db));
        sqlite3_close(status.db);
        return 1;
    }

    //Creating tables
    error = createTables(&status);
    if (error != SQLITE_OK) {
        sqlite3_close(status.db);
        return 1;
    }
  
    
    sqlite3_close(status.db);
    return 0;
}

int createTables(ServerStatus *status) {
    int error;
    char *sql_statement;
    char *errMsg = 0;
    
    // Creating table USER
    sql_statement = "CREATE TABLE IF NOT EXISTS USER("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "PASSWORD       TEXT    NOT NULL" \
        ");";
   
    error = sqlite3_exec(status->db, sql_statement, callback, 0, &errMsg);
    if( error != SQLITE_OK ){
        fprintf(stderr, "SQL error during creating table USER: %s\n", errMsg);
        sqlite3_free(errMsg);
        return error;
    } else {
        fprintf(stdout, "Table USER created successfully\n");
    }
    
    // Creating table CHANNEL
    sql_statement = "CREATE TABLE IF NOT EXISTS CHANNEL("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL" \
        ");";
   
    error = sqlite3_exec(status->db, sql_statement, callback, 0, &errMsg);
    if (error != SQLITE_OK ){
        fprintf(stderr, "SQL error during creating table CHANNEL: %s\n", errMsg);
        sqlite3_free(errMsg);
        return error;
    } else {
        fprintf(stdout, "Table CHANNEL created successfully\n");
    }
    
    // Creating table POST
    sql_statement = "CREATE TABLE IF NOT EXISTS POST("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "CONTENT        TEXT        NOT NULL," \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)" \
        ");";
   
    error = sqlite3_exec(status->db, sql_statement, callback, 0, &errMsg);
    if (error != SQLITE_OK){
        fprintf(stderr, "SQL error during creating table POST: %s\n", errMsg);
        sqlite3_free(errMsg);
        return error;
    } else {
        fprintf(stdout, "Table POST created successfully\n");
    }
    
    return 0;
}

