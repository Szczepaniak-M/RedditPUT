#ifndef SERVER_CLIENT_THREAD_H
#define SERVER_CLIENT_THREAD_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "structures.h"

void *clientThread(void *data);

#endif //SERVER_CLIENT_THREAD_H
