#include "database.h"
#include "server.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Sposób użycia: %s port_number\n", argv[0]);
        return 1;
    }

    int error;

    // Initializing ServerStatus
    ServerStatus status;
    status.programName = argv[0];
    status.serverPort = atoi(argv[1]);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        status.activeUsers[i].descriptor = -1;
    }
    error = pthread_mutex_init(&status.mutex, NULL);
    if ( error != 0) {
        fprintf(stderr, "%s: Błąd podczas tworzenia mutex'u\n", argv[0]);
        return 1;
    }

    // Creating database
    error = createDatabase(&status);
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating tables
    error = createTables(&status);
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating socket connection
    error = createSocket(&status);
    if (error != 0) {
        sqlite3_close(status.db);
        return error;
    }

    // Creating thread which handle new connections
    error = createConnectionHandlerThread(&status);
    if (error != 0) {
        sqlite3_close(status.db);
        return error;
    }

    // Waiting for close
    error = waitForExit(&status);
    if (error != 0) {
        sqlite3_close(status.db);
        return error;
    }

    sqlite3_close(status.db);
    return 0;
}

