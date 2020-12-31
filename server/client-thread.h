#ifndef SERVER_CLIENT_THREAD_H
#define SERVER_CLIENT_THREAD_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "structures.h"

void *clientThread(void *data);

int signUp(ServerStatus *status, int size, char *buffer);

int login(ServerStatus *status, int size, char *buffer);

int addPost(ServerStatus *status, int size, char *buffer);

int addChannel(ServerStatus *status, int size, char *buffer);

int subscribeChannel(ServerStatus *status, int size, char *buffer);

int unsubscribeChannel(ServerStatus *status, int size, char *buffer);

#endif //SERVER_CLIENT_THREAD_H
