#include "client-thread.h"

void *clientThread(void *inputData) {
    ThreadData *data = (ThreadData *) inputData;
    int descriptor = data->descriptor;
    ServerStatus *status = data->status;
    int error;
    char textBuffer[300];
    int requestType = -1;
    int requestComplete = 0;
    int size = 0;
    while ((error = read(descriptor, textBuffer, 300)) && error > 0) {
        // todo buffor, czytanie do czasu znalezienia typu requestu

        switch (requestType) {
            case 1: // rejestracja
                error = signUp(status, size, textBuffer);
                requestType = -1;
                break;
            case 2: // logowanie
                error = login(status, size, textBuffer);
                requestType = -1;
                break;
            case 3: // dodaj postll
                error = addPost(status, size, textBuffer);
                requestType = -1;
                break;
            case 4: // dodaj kanał
                error = addChannel(status, size, textBuffer);
                requestType = -1;
                break;
            case 5: // zasubskrybuj kanał
                error = subscribeChannel(status, size, textBuffer);
                requestType = -1;
                break;
            case 6: // usuń subksrypcję
                error = unsubscribeChannel(status, size, textBuffer);
                requestType = -1;
                break;
            default:
                break;
        }
    }
    if (error == -1) {

    }
    pthread_mutex_lock(&status->mutex);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].descriptor = -1;
            if (status->isCleaning == 0) {
                status->pthreads[i].isInitialized = 0;
            }
        }
    }
    pthread_mutex_unlock(&status->mutex);
    free(inputData);
    close(descriptor);
    pthread_exit(NULL);
}

int signUp(ServerStatus *status, int size, char *buffer) {

}

int login(ServerStatus *status, int size, char *buffer) {

}

int addPost(ServerStatus *status, int size, char *buffer) {

}

int addChannel(ServerStatus *status, int size, char *buffer) {

}

int subscribeChannel(ServerStatus *status, int size, char *buffer) {

}

int unsubscribeChannel(ServerStatus *status, int size, char *buffer) {

}
