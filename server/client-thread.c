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
                error = signUp(status, descriptor, size, textBuffer);
                requestType = -1;
                break;
            case 2: // logowanie
                error = login(status, descriptor, size, textBuffer);
                requestType = -1;
                break;
            case 3: // dodaj postll
                error = addPost(status, descriptor, size, textBuffer);
                requestType = -1;
                break;
            case 4: // dodaj kanał
                error = addChannel(status, descriptor, size, textBuffer);
                requestType = -1;
                break;
            case 5: // zasubskrybuj kanał
                error = subscribeChannel(status, descriptor, size, textBuffer);
                requestType = -1;
                break;
            case 6: // usuń subksrypcję
                error = unsubscribeChannel(status, descriptor, size, textBuffer);
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

int signUp(ServerStatus *status, int descriptor, int size, char *buffer) {
    int error;
    User user;
    error = insertUser(status, &user);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].id = user.id;

        }
    }
    return error;
}

int login(ServerStatus *status, int descriptor, int size, char *buffer) {
    User user;
    int error;
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].id = user.id;

        }
    }
    return error;
}

int addPost(ServerStatus *status, int descriptor, int size, char *buffer) {
    Post post;
    int error;
    error = insertPost(status, &post);
    return error;
}

int addChannel(ServerStatus *status, int descriptor, int size, char *buffer) {
    Channel channel;
    int error;
    error = insertChannel(status, &channel);
    return error;
}

int subscribeChannel(ServerStatus *status, int descriptor, int size, char *buffer) {
    int userId;
    int channelId;
    int error;
    error = insertSubscription(status, userId, channelId);
    return error;

}

int unsubscribeChannel(ServerStatus *status, int descriptor, int size, char *buffer) {
    int userId;
    int channelId;
    int error;
    error = deleteSubscription(status, userId, channelId);
    return error;
}
