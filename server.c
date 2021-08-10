#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void initServerAddr(struct sockaddr_in *servAddr, int port);

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

    int connSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientSockSize = sizeof(clientAddr);
    char buffer[256], key[5];
    for (;;) {
        connSocket = accept(servSocket, (struct sockaddr *) &clientAddr, &clientSockSize);
        printf("Connected to client\n");

        memset(buffer, 0, 256);
        memset(key, 0, 5);
        recv(connSocket, buffer, 255, 0);
        memcpy(key, buffer, 4);
        if (strcmp(key, "QUIT")==0) {
            close(connSocket);
            break;
        }
        printf("Msg from client: %s\n", buffer);
        close(connSocket);
    }
    close(servSocket);
    return 0;
}

void initServerAddr(struct sockaddr_in *servAddr, int port) {
    servAddr->sin_family = AF_INET;
    servAddr->sin_port = htons(port);
    servAddr->sin_addr.s_addr = INADDR_ANY;
}
