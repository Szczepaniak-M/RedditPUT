#include "client-thread.h"

void *clientThread(void *inputData) {
    ThreadData *data = (ThreadData *) inputData;
    int descriptor = data->descriptor;
    ServerStatus *status = data->status;
    int error;
    char sizeBuffer[15];
    char readChar;
    char requestType = '0';
    int delimiterCounter = 0;
    int charCounter = 0;
    int size = 0;
    while ((error = read(descriptor, &readChar, 1)) && error > 0) {
        if (delimiterCounter == 0 && readChar != ';') {
            sizeBuffer[charCounter] = readChar;
            charCounter++;
        } else if (delimiterCounter == 0 && readChar == ';') {
            size = atoi(sizeBuffer);
            delimiterCounter++;
        } else if (delimiterCounter == 1 && readChar != ';') {
            requestType = readChar;
        } else if (delimiterCounter == 1 && readChar == ';') {
            switch (requestType) {
                case '1': // rejestracja
                    error = signUp(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '2': // logowanie
                    error = login(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '3': // dodaj post
                    error = addPost(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '4': // dodaj kanał
                    error = addChannel(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '5': // zasubskrybuj kanał
                    error = subscribeChannel(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '6': // usuń subksrypcję
                    error = unsubscribeChannel(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                default:
                    break;
            }
        }
        if (error == -1) {

        }
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

int signUp(ServerStatus *status, int descriptor, int size) {
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

int login(ServerStatus *status, int descriptor, int size) {
    User user;
    int error;
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].id = user.id;

        }
    }
    return error;
}

int addPost(ServerStatus *status, int descriptor, int size) {
    Post post;
    int error;
    error = insertPost(status, &post);
    return error;
}

int addChannel(ServerStatus *status, int descriptor, int size) {
    Channel channel;
    int error;
    error = insertChannel(status, &channel);
    return error;
}

int subscribeChannel(ServerStatus *status, int descriptor, int size) {
    int userId;
    int channelId;
    int error;
    error = insertSubscription(status, userId, channelId);
    return error;

}

int unsubscribeChannel(ServerStatus *status, int descriptor, int size) {
    int userId;
    int channelId;
    int error;
    error = deleteSubscription(status, userId, channelId);
    return error;
}

void clear(char *requestType, int *charCounter, int *delimiterCounter, char *sizeBuffer) {
    *requestType = '0';
    *charCounter = 0;
    *delimiterCounter = 0;
    memset(sizeBuffer, 0, 15);
}
