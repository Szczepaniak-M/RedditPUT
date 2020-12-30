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

#include "structures.h"


int createConnection(ServerStatus *status);

int waitForExit(ServerStatus *status);


#endif
