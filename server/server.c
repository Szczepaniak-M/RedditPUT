#include "server.h"

int createSocket(ServerStatus *status) {
    int error;
    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(struct sockaddr));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(status->serverPort);

    status->serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (status->serverSocketDescriptor < 0) {
        fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda.\n", status->programName);
        return 1;
    }

    error = bind(status->serverSocketDescriptor, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr));
    if (error < 0) {
        fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", status->programName);
        close(status->serverSocketDescriptor);
        return 1;
    }

    error = listen(status->serverSocketDescriptor, QUEUE_SIZE);
    if (error < 0) {
        fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", status->programName);
        close(status->serverSocketDescriptor);
        return 1;
    }
    return 0;
}

int createConnectionHandlerThread(ServerStatus *status){
    int error;
    pthread_t thread;

    error = pthread_create(&thread, NULL, connectionHandler, (void *) status);
    if (error) {
        printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", error);
        close(status->serverSocketDescriptor);
    }
    return error;
}

void *connectionHandler(void *data) {
    pthread_detach(pthread_self());
    int connectionSocketDescriptor;
    ServerStatus *status = (ServerStatus *) data;
    while (1) {
        connectionSocketDescriptor = accept(status->serverSocketDescriptor, NULL, NULL);
        if (connectionSocketDescriptor < 0) {
            fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", status->programName);
            clean(status);
            exit(1);
        }

        pthread_mutex_lock(&status->mutex);
        int success = 0;
        int i = 0;
        for (i = 0; i < ACTIVE_USER_LIMIT; i++) {
            if (status->activeUsers[i].descriptor == -1) {
                status->activeUsers[i].descriptor = connectionSocketDescriptor;
                success = 1;
                break;
            }
        }
        pthread_mutex_unlock(&status->mutex);

        if (success) {
            handleConnection(status, connectionSocketDescriptor, i);
        } else {
            close(connectionSocketDescriptor);
        }
    }
}

void handleConnection(ServerStatus *status, int descriptor, int index) {
    int error = 0;
    pthread_t thread;

    ThreadData *data = (ThreadData *) malloc(sizeof(ThreadData));
    data->status = status;
    data->descriptor = descriptor;

    error = pthread_create(&thread, NULL, clientThread, (void *) data);
    if (error) {
        printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", error);
        clean(status);
        exit(1);
    } else {
        status->pthreads[index] = thread;
    }
}


int waitForExit(ServerStatus *status) {
    char textBuffer[5];
    int error;
    fprintf(stdout, "Aby bezpiecznie zamknąć serwer wpisz 'exit' i wciśnij Enter\n");
    while ((error = read(1, textBuffer, 5)) && error > 0) {
        if (strcmp(textBuffer, "exit\n") == 0) {
            clean(status);
            return 0;
        }
    }
    if (error == -1) {
        clean(status);
        return error;
    }
    return 0;
}

void clean(ServerStatus *status){
    pthread_mutex_lock(&status->mutex);
    for (int i = 0; i < ACTIVE_USER_LIMIT; i++) {
        if (status->activeUsers[i].descriptor != -1) {
            shutdown(status->activeUsers[i].descriptor, SHUT_RDWR);
            pthread_join(status->pthreads[i], NULL);
        }
    }
    close(status->serverSocketDescriptor);
    sqlite3_close(status->db);
    pthread_mutex_unlock(&status->mutex);
    pthread_mutex_destroy(&status->mutex);
}

