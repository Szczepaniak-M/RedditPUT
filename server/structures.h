#ifndef SERVER_STRUCTURES_H
#define SERVER_STRUCTURES_H

#include <sqlite3.h>
#include <pthread.h>

#define QUEUE_SIZE 100
#define ACTIVE_USER_LIMIT 100

typedef struct User {
    int id;
    int descriptor;
} User;

typedef struct Thread {
    pthread_t id;
    int isInitialized;
} Thread;


typedef struct ServerStatus {
    sqlite3 *db;
    char *programName;
    int serverPort;
    int serverSocketDescriptor;
    pthread_mutex_t mutex;
    User activeUsers[ACTIVE_USER_LIMIT];
    Thread pthreads[ACTIVE_USER_LIMIT];
    int isCleaning;
} ServerStatus;

typedef struct ThreadData {
    ServerStatus *status;
    int descriptor;
} ThreadData;

#endif
