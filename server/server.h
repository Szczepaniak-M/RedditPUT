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


int createSocket(ServerStatus *status);

int createAcceptingConnectionThread(ServerStatus *status);

void *createConnections(void *data);

int waitForExit(ServerStatus *status);

void clean(ServerStatus *status);


#endif
