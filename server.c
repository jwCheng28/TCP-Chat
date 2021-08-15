#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENT 5
#define RESPSIZE 300
#define BUFSIZE 256
#define UNAMESIZE 32
#define KEYSIZE 5

void initServerAddr(struct sockaddr_in *servAddr, int port) {
    servAddr->sin_family = AF_INET;
    servAddr->sin_port = htons(port);
    servAddr->sin_addr.s_addr = INADDR_ANY;
}

typedef struct {
    int servSocket;
    int connSocket;
    int occupied;
} clientConn;


static pthread_t threads[MAX_CLIENT];
static clientConn clients[MAX_CLIENT];
static int clientCount = 0;

int availableSlot() {
    if (clientCount >= MAX_CLIENT) return -1;
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (clients[i].occupied != 1) return i;
    }
    return -1;
}

void removeClient(clientConn *client) {
    client->servSocket = -1;
    client->connSocket = -1;
    client->occupied = 0;
    clientCount--;
}

void sendToAllClient(char response[RESPSIZE]) {
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (clients[i].occupied != 1) continue;
        send(clients[i].connSocket, response, strlen(response), 0);
    }
}

void *handleClient(void *args) {
    clientConn client = * (clientConn *) args;
    int servSocket = client.servSocket;
    int connSocket = client.connSocket;
    
    char response[RESPSIZE], buffer[BUFSIZE], user[UNAMESIZE], key[KEYSIZE];
    memset(user, 0, UNAMESIZE);
    recv(connSocket, user, UNAMESIZE-1, 0);
    printf("Connection Log - %s Connected\n", user);
    while (1) {
        memset(response, 0, RESPSIZE);
        memset(buffer, 0, BUFSIZE);
        memset(key, 0, KEYSIZE);
        recv(connSocket, buffer, BUFSIZE-1, 0);
        memcpy(key, buffer, KEYSIZE-1);
        if (strcmp(key, "QUIT")==0) {
            removeClient(&client);
            break;
        }
        printf("Message Log - %s: %s\n", user, buffer);
        sprintf(response, "%s : %s", user, buffer);
        sendToAllClient(response);
    }
    printf("%s Has Disconnected\n", user);
    close(connSocket);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Port Number Not Specified\n");
        return 1;
    }

    int servSocket = socket(AF_INET, SOCK_STREAM, 0), connSocket;
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
    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientSockSize = sizeof(clientAddr);
        connSocket = accept(servSocket, (struct sockaddr *) &clientAddr, &clientSockSize);
        int available = availableSlot();
        if (available < 0) {
            printf("Max client amout has reached, cannot accept more users\n");
            send(connSocket, "FAILED", 6, 0);
            continue;
        }

        send(connSocket, "SUCCESS", 7, 0);
        clients[available].servSocket = servSocket;
        clients[available].connSocket = connSocket;
        clients[available].occupied = 1;
        pthread_create(&threads[available], NULL, handleClient, &clients[available]);
        clientCount++;
    }
    close(servSocket);
    return 0;
}

