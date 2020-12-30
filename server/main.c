#include "database.h"
#include "server.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Sposób użycia: %s port_number\n", argv[0]);
        exit(1);
    }

    ServerStatus status;
    status.programName = argv[0];
    status.serverPort = atoi(argv[1]);
    int error;

    // Creating database
    error = createDatabase(&status);
    if (error != SQLITE_OK) {
        return error;
    }

    //Creating tables
    error = createTables(&status);
    if (error != SQLITE_OK) {
        return error;
    }

    //Creating socket connection
    error = createConnection(&status);
    if (error != 0) {
        sqlite3_close(status.db);
        return error;
    }

    error = waitForExit(&status);
    if (error != 0) {
        sqlite3_close(status.db);
        return error;
    }

    sqlite3_close(status.db);
    return 0;
}

