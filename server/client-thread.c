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
        error = 0;
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
                case '0': // registration
                    error = signUp(status, descriptor, size);
                    break;
                case '1': // login
                    error = login(status, descriptor, size);
                    break;
                case '2': // add post
                    error = addPost(status, descriptor, size);
                    break;
                case '3': // add channel
                    error = addChannel(status, descriptor, size);
                    break;
                case '4': // subscribe channel
                    error = subscribeChannel(status, descriptor, size);
                    break;
                case '5': // delete subscription
                    error = unsubscribeChannel(status, descriptor, size);
                    break;
                case '8': // show post in channel
                    error = getPostByChannelId(status, descriptor, size);
                    break;
                default:
                    break;
            }
            clear(&requestType, &charCounter, &delimiterCounter, sizeBuffer);
        }
        if (error != 0) {
            break;
        }
    }

    pthread_mutex_lock(&status->activeUsersMutex);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].descriptor = -1;
            if (status->isCleaning == 0) {
                status->pthreads[i].isInitialized = 0;
                pthread_detach(pthread_self());
            }
        }
    }
    pthread_mutex_unlock(&status->activeUsersMutex);
    free(inputData);
    close(descriptor);
    pthread_exit(NULL);
}

int signUp(ServerStatus *status, int descriptor, int size) {
    int error;
    User user;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }

    char *savePointer;
    char *name = strtok_r(content, ";", &savePointer);
    user.name = name;
    user.password = strtok_r(NULL, ";", &savePointer);

    // checking if name is duplicated
    error = selectUserByName(status, &user);
    if (user.name != NULL) {
        sendResponse('0', 1, descriptor);
        free(content);
        return error;
    }
    user.name = name;

    // encrypt password
    pthread_mutex_lock(&status->cryptMutex);
    char *password = crypt(user.password, "$6$pt4wu5ns");
    user.password = (char *) malloc(sizeof(char) * (strlen(password) + 1));
    strcpy(user.password, password);
    pthread_mutex_unlock(&status->cryptMutex);

    // insert user
    error = insertUser(status, &user);
    if (error != 0) {
        sendResponse('0', 1, descriptor);
        free(content);
        return error;
    }

    // get user id
    error = selectUserByName(status, &user);
    if (error != 0) {
        sendResponse('0', 1, descriptor);
        free(content);
        return error;
    }
    free(user.password);
    free(user.name);
    free(content);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].id = user.id;
        }
    }

    // send confirmation
    if (error == 0) {
        sendResponse('0', 0, descriptor);
    } else {
        sendResponse('0', 1, descriptor);
    }
    return 0;
}

int login(ServerStatus *status, int descriptor, int size) {
    User user;
    int error;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }
    char *savePointer;
    user.name = strtok_r(content, ";", &savePointer);
    char *password = strtok_r(NULL, ";", &savePointer);

    // check if login exists in database
    error = selectUserByName(status, &user);
    if (user.name == NULL) {
        sendResponse('0', 1, descriptor);
        free(content);
        return error;
    }

    // encrypt password
    pthread_mutex_lock(&status->cryptMutex);
    char *cryptPassword = crypt(password, "$6$pt4wu5ns");
    error = strcmp(user.password, cryptPassword);
    pthread_mutex_unlock(&status->cryptMutex);

    free(user.name);
    free(user.password);
    free(content);

    // send confirmation if password is correct
    if (error != 0) { // wrong password
        sendResponse('1', 1, descriptor);
        return 0;
    } else {
        sendResponse('1', 0, descriptor);
    }

    // add user to active user list
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor == descriptor) {
            status->activeUsers[i].id = user.id;
        }
    }

    // send list of subscribed channels
    sqlite3_stmt *stmt = NULL;
    Channel channel;
    error = selectChannelsByUserId(status, user.id, &stmt);
    while (error == SQLITE_ROW) {
        channel.id = sqlite3_column_int(stmt, 0);
        channel.name = (char *) sqlite3_column_text(stmt, 0);
        sendChannel(&channel, descriptor);
        error = sqlite3_step(stmt);
    }
    if (error != SQLITE_DONE) {
        executeError(status, "selectChannelsByUserId", stmt);
        return error;
    }
    sqlite3_finalize(stmt);

    // send all notifications
    int channelId;
    error = selectNoticesByUserId(status, user.id, &stmt);
    while (error == SQLITE_ROW) {
        channelId = sqlite3_column_int(stmt, 0);
        sendNotice(channelId, descriptor);
        error = sqlite3_step(stmt);
    }
    if (error != SQLITE_DONE) {
        executeError(status, "selectNoticesByUserId", stmt);
        return error;
    }
    sqlite3_finalize(stmt);

    return 0;
}

int addPost(ServerStatus *status, int descriptor, int size) {
    Post post;
    int error;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }
    char *savePointer;
    post.userId = atoi(strtok_r(content, ";", &savePointer));
    post.channelId = atoi(strtok_r(NULL, ";", &savePointer));
    post.content = strtok_r(NULL, ";", &savePointer);

    // insert data
    error = insertPost(status, &post);
    free(content);

    // send confirmation
    if (error == 0) {
        sendResponse('2', 0, descriptor);
    } else {
        sendResponse('2', 1, descriptor);
        return error;
    }

    // send notification to other users
    sqlite3_stmt *stmt = NULL;
    int userId;
    error = selectNewPostIdByUserId(status, post.userId, &post.id);
    if (error != 0) {
        return error;
    }
    error = selectUsersByChannelId(status, post.channelId, &stmt);
    while (error == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
        error = insertNotice(status, userId, post.channelId, post.id);
        if (error != SQLITE_OK) {
            break;
        }
        pthread_mutex_lock(&status->activeUsersMutex);
        for(int i = 0; i < ACTIVE_USER_LIMIT; i++){
            if (status->activeUsers[i].id == userId){
                if (descriptor == status->activeUsers[i].descriptor) {
                    break;
                } else {
                    sendNotice(post.channelId, status->activeUsers[i].descriptor);
                }
            }
        }
        pthread_mutex_unlock(&status->activeUsersMutex);
        error = sqlite3_step(stmt);
    }
    if (error != SQLITE_DONE) {
        executeError(status, "selectUsersByChannelId", stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int addChannel(ServerStatus *status, int descriptor, int size) {
    Channel channel;
    int error;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }
    channel.name = content;

    // insert data
    error = insertChannel(status, &channel);
    free(content);

    // send confirmation
    if (error == 0) {
        sendResponse('3', 0, descriptor);
    } else {
        sendResponse('3', 1, descriptor);
    }
    return error;
}

int subscribeChannel(ServerStatus *status, int descriptor, int size) {
    int userId;
    int channelId;
    int error;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }
    char *savePointer;
    userId = atoi(strtok_r(content, ";", &savePointer));
    channelId = atoi(strtok_r(NULL, ";", &savePointer));

    // insert data
    error = insertSubscription(status, userId, channelId);
    free(content);

    // send confirmation
    if (error == 0) {
        sendResponse('4', 0, descriptor);
    } else {
        sendResponse('4', 1, descriptor);
    }
    return error;
}

int unsubscribeChannel(ServerStatus *status, int descriptor, int size) {
    int userId;
    int channelId;
    int error;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }
    char *savePointer;
    userId = atoi(strtok_r(content, ";", &savePointer));
    channelId = atoi(strtok_r(NULL, ";", &savePointer));

    // delete data
    error = deleteSubscription(status, userId, channelId);
    free(content);

    // send confirmation
    if (error == 0) {
        sendResponse('5', 0, descriptor);
    } else {
        sendResponse('5', 1, descriptor);
    }
    return error;
}
int getPostByChannelId(ServerStatus *status, int descriptor, int size) {
    int error;

    // transform data
    char *content = readContent(descriptor, size, &error);
    if (error == -1) {
        free(content);
        return error;
    }
    int channelId = atoi(content);

    // get and send data
    Post post;
    sqlite3_stmt *stmt = NULL;
    error = selectPostByChannelId(status, channelId, &stmt);
    while (error == SQLITE_ROW) {
        post.id = sqlite3_column_int(stmt, 0);
        post.userName = (char *) sqlite3_column_text(stmt, 1);
        post.content = (char *) sqlite3_column_text(stmt, 2);
        sendPost(&post, descriptor);
        error = sqlite3_step(stmt);
    }
    if (error != SQLITE_DONE) {
        executeError(status, "selectPostByChannelId", stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int sendNotice(int channelId, int descriptor) {
    int error;
    int channelIdSize = intLength(channelId);
    int dataSize = intLength(channelIdSize);
    char *response = (char *) malloc(sizeof(char) * (dataSize + channelIdSize + 4));
    sprintf(response, "%d;6;%d", channelIdSize, channelId);
    error = write(descriptor, response, strlen(response) * sizeof(char));
    free(response);
    return error;
}

int sendChannel(Channel *channel, int descriptor) {
    int error;
    int channelIdSize = intLength(channel->id);
    int channelNameSize = strlen(channel->name);
    int dataLength = channelIdSize + channelNameSize + 1;
    int dataSize = intLength(dataLength);
    char *response = (char *) malloc(sizeof(char) * (dataSize + dataLength + 3));
    sprintf(response, "%d;7;%d;%s", dataLength, channel->id, channel->name);
    error = write(descriptor, response, strlen(response) * sizeof(char));
    free(response);
    return error;
}

int sendPost(Post *post, int descriptor) {
    int error;
    int postIdSize = intLength(post->id);
    int userNameSize = strlen(post->userName);
    int contentSize = strlen(post->content);
    int dataLength = postIdSize + userNameSize + contentSize + 2;
    int dataSize = intLength(dataLength);
    char *response = (char *) malloc(sizeof(char) * (dataSize + dataLength + 3));
    sprintf(response, "%d;8;%d;%s;%s", dataLength, post->id, post->userName, post->content);
    error = write(descriptor, response, strlen(response) * sizeof(char));
    free(response);
    return error;
}

int sendResponse(char type, int fail, int descriptor) {
    int error;
    char *response = (char *) malloc(sizeof(char) * 7);
    sprintf(response, "1;%c;%d", type, fail);
    error = write(descriptor, response, strlen(response) * sizeof(char));
    free(response);
    return error;
}

char *readContent(int descriptor, int size, int *error) {
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

int intLength(int n) {
    int counter = 0;
    while (n > 0) {
        n /= 10;
        counter++;
    }
    return counter;
}
