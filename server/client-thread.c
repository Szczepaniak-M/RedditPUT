#include "client-thread.h"

void *clientThread(void *inputData) {
    ThreadData *data = (ThreadData *) inputData;
    int index = data->index;
    ServerStatus *status = data->status;
    int descriptor = data->status->activeUsers[index].descriptor;
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
                case '0': // rejestracja
                    error = signUp(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '1': // logowanie
                    error = login(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '2': // dodaj post
                    error = addPost(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '3': // dodaj kanał
                    error = addChannel(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '4': // zasubskrybuj kanał
                    error = subscribeChannel(status, descriptor, size);
                    clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
                    break;
                case '5': // usuń subksrypcję
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

    char* content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

    free(content);
    char *result = crypt("Haslo", "AA");
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

    char* content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

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

    char* content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

    error = insertPost(status, &post);
    return error;
}

int addChannel(ServerStatus *status, int descriptor, int size) {
    Channel channel;
    int error;

    char* content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

    error = insertChannel(status, &channel);
    return error;
}

int subscribeChannel(ServerStatus *status, int descriptor, int size) {
    int userId;
    int channelId;
    int error;

    char* content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

    error = insertSubscription(status, userId, channelId);
    return error;

}

int unsubscribeChannel(ServerStatus *status, int descriptor, int size) {
    int userId;
    int channelId;
    int error;

    char* content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

    error = deleteSubscription(status, userId, channelId);
    return error;
}

char* readContent(int descriptor, int size, int *error){
    char *content = (char *) malloc(sizeof(char) * size);
    char textBuffer[101];
    memset(textBuffer, 0, 101);
    int counter = 0;
    int readByte = size < 100 ? size : 100;
    while ((*error = read(descriptor, textBuffer, readByte)) && *error > 0) {
        strcpy(content + counter, textBuffer);
        counter += *error;
        size -= *error;
        readByte = size < 100 ? size : 100;
        if (size == 0 || *error == -1) {
            break;
        }
        memset(textBuffer, 0, 100);
    }
    return content;
}

void clear(char *requestType, int *charCounter, int *delimiterCounter, char *sizeBuffer) {
    *requestType = '0';
    *charCounter = 0;
    *delimiterCounter = 0;
    memset(sizeBuffer, 0, 15);
}
