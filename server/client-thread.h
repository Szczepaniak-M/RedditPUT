#ifndef SERVER_CLIENT_THREAD_H
#define SERVER_CLIENT_THREAD_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "structures.h"
#include "database.h"

void *clientThread(void *data);

int signUp(ServerStatus *status, int descriptor, int size);

int login(ServerStatus *status, int descriptor, int size);

int addPost(ServerStatus *status, int descriptor, int size);

int addChannel(ServerStatus *status, int descriptor, int size);

int subscribeChannel(ServerStatus *status, int descriptor, int size);

int unsubscribeChannel(ServerStatus *status, int descriptor, int size);

char* readContent(int descriptor, int size, int *error);

void clear(char *requestType, int *charCounter, int *delimiterCounter, char *sizeBuffer);

int sendResponse(char type, int success, int descriptor);

#endif //SERVER_CLIENT_THREAD_H
