#ifndef SERVER_STRUCTURES_H
#define SERVER_STRUCTURES_H

#include <sqlite3.h>
#include <pthread.h>

#define QUEUE_SIZE 100
#define ACTIVE_USER_LIMIT 100

typedef struct ActiveUser {
    int id;
    int descriptor;
} ActiveUser;

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
    ActiveUser activeUsers[ACTIVE_USER_LIMIT];
    Thread pthreads[ACTIVE_USER_LIMIT];
    int isCleaning;
} ServerStatus;

typedef struct ThreadData {
    ServerStatus *status;
    int descriptor;
} ThreadData;

typedef struct User {
    int id;
    char* name;
    char* password;
} User;

typedef struct Channel {
    int id;
    char* name;
} Channel;

typedef struct Post {
    int id;
    int userId;
    int channelId;
    char* content;
} Post;



#endif
