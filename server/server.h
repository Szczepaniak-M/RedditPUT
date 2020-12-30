#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "structures.h"
#include "client-thread.h"


int createSocket(ServerStatus *status);

int createConnectionHandlerThread(ServerStatus *status);

void *connectionHandler(void *data);

void handleConnection(ServerStatus *status, int descriptor, int index);

int waitForExit(ServerStatus *status);

void clean(ServerStatus *status);


#endif
