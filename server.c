#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void initServerAddr(struct sockaddr_in *servAddr, int port);
int handleClient(int servSocket);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Port Number Not Specified\n");
        return 1;
    }

    int servSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (servSocket < 0) {
        printf("Server Socket Open Failed\n");
        return 1;
    }

    struct sockaddr_in servAddr;
    initServerAddr(&servAddr, atoi(argv[1]));
    if (bind(servSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        printf("Server Socket Binding Failed\n");
        return 1;
    }

    listen(servSocket, 5);
    int running = 1;
    while (running) {
        running = handleClient(servSocket);
    }
    close(servSocket);
    return 0;
}

void initServerAddr(struct sockaddr_in *servAddr, int port) {
    servAddr->sin_family = AF_INET;
    servAddr->sin_port = htons(port);
    servAddr->sin_addr.s_addr = INADDR_ANY;
}

int handleClient(int servSocket) {
    int servRunning = 1;
    char response[300], buffer[256], user[32], key[5];
    memset(user, 0, 32);
    struct sockaddr_in clientAddr;
    socklen_t clientSockSize = sizeof(clientAddr);
    int connSocket = accept(servSocket, (struct sockaddr *) &clientAddr, &clientSockSize);
    recv(connSocket, user, 31, 0);
    printf("Connected to user %s\n", user);
    while (1) {
        memset(response, 0, 288);
        memset(buffer, 0, 256);
        memset(key, 0, 5);
        recv(connSocket, buffer, 255, 0);
        memcpy(key, buffer, 4);
        if (strcmp(key, "QUIT")==0) {
            servRunning = 0;
            break;
        }
        printf("Log %s message: %s\n", user, buffer);
        sprintf(response, "%s : %s", user, buffer);
        send(connSocket, response, strlen(response), 0);
    }
    printf("Closed connection with %s\n", user);
    close(connSocket);
    return servRunning;
}
