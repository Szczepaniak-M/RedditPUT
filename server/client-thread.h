#ifndef SERVER_CLIENT_THREAD_H
#define SERVER_CLIENT_THREAD_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <crypt.h>

#include "structures.h"
#include "database.h"

void *clientThread(void *data);

int signUp(ServerStatus *status, int descriptor, int size);

int login(ServerStatus *status, int descriptor, int size);

int addPost(ServerStatus *status, int descriptor, int size, int index);

int addChannel(ServerStatus *status, int descriptor, int size);

int subscribeChannel(ServerStatus *status, int descriptor, int size);

int unsubscribeChannel(ServerStatus *status, int descriptor, int size);

int getPostByChannelId(ServerStatus *status, int descriptor, int size);

int sendNotice(int channelId, int descriptor);

int sendChannel(Channel *channel, int descriptor);

int sendPost(Post *post, int descriptor);

char* readContent(int descriptor, int size, int *error);

void clear(char *requestType, int *charCounter, int *delimiterCounter, char *sizeBuffer);

int sendResponse(char type, int fail, int descriptor);

int intLength(int n);

#endif //SERVER_CLIENT_THREAD_H
