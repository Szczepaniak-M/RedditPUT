#include "server.h"

int createConnection(ServerStatus *status) {
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
        return 1;
    }

    error = listen(status->serverSocketDescriptor, QUEUE_SIZE);
    if (error < 0) {
        fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", status->programName);
        return 1;
    }
    return 0;
}

int waitForExit(ServerStatus *status) {
    char textBuffer[10];
    int error;
    fprintf(stdout, "Aby bezpiecznie zamknąć serwer wpisz 'exit'\n");
    while ((error = read(1, textBuffer, 10)) && error > 0) {
        if (strcmp(textBuffer, "exit\n") == 0) {
            return 0;
        }
    }
    if (error == -1) {
        perror("Błąd podczas wysyłania danych");
        close(status->serverSocketDescriptor);
        return error;
    }
    return 0;
}

