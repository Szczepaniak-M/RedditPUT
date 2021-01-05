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

int signUp(ServerStatus *status, int descriptor, int size, int index);

int login(ServerStatus *status, int descriptor, int size, int index);

int addPost(ServerStatus *status, int descriptor, int size, int index);

int addChannel(ServerStatus *status, int descriptor, int size, int index);

int subscribeChannel(ServerStatus *status, int descriptor, int size, int index);

int unsubscribeChannel(ServerStatus *status, int descriptor, int size, int index);

int getPostByChannelId(ServerStatus *status, int descriptor, int size, int index);

int getAllChannels(ServerStatus *status, int descriptor, int index);

int sendNotice(ServerStatus *status, int channelId, int descriptor, int index);

int sendChannel(ServerStatus *status, Channel *channel, char requestType, int descriptor, int index);

int sendPost(ServerStatus *status, Post *post, int descriptor, int index);

int sendResponse(ServerStatus *status, char type, int fail, int descriptor, int index);

char* readContent(int descriptor, int size, int *error);

void clear(char *requestType, int *charCounter, int *delimiterCounter, char *sizeBuffer);

int intLength(int n);

#endif //SERVER_CLIENT_THREAD_H
