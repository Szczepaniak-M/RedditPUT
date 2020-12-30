#include "client-thread.h"

void *clientThread(void *inputData) {
    ThreadData *data = (ThreadData *) inputData;
    int descriptor = data->descriptor;
    ServerStatus *status = data->status;
    int error;
    char textBuffer[300];
    while ((error = read(descriptor, textBuffer, 300)) && error > 0) {
        pthread_mutex_lock(&status->mutex);

        pthread_mutex_unlock(&status->mutex);
    }

    if (error == -1) {

    }
    pthread_mutex_lock(&status->mutex);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].descriptor = -1;
            if (status->isCleaning == 0){
                status->pthreads[i].isInitialized = 0;
            }
        }
    }
    pthread_mutex_unlock(&status->mutex);
    free(inputData);
    close(descriptor);
    pthread_exit(NULL);
}
