#ifndef SERVER_STRUCTURES_H
#define SERVER_STRUCTURES_H

#include <sqlite3.h>

#define QUEUE_SIZE 100

typedef struct ServerStatus {
    sqlite3 *db;
    char* programName;
    int serverPort;
    int serverSocketDescriptor;
} ServerStatus;

typedef struct ThreadData {
    int connectionSocketDescriptor;
} ThreadData;

#endif
