#ifndef SERVER_STRUCTURES_H
#define SERVER_STRUCTURES_H

#include <sqlite3.h>
#include <pthread.h>

#define QUEUE_SIZE 100


typedef struct ServerStatus {
    sqlite3 *db;
    char *programName;
    int serverPort;
    int serverSocketDescriptor;
    pthread_mutex_t mutex;
} ServerStatus;

typedef struct ThreadData {
    int connectionSocketDescriptor;
} ThreadData;

#endif
