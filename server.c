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
#define KEYSIZE 12

void initServerAddr(struct sockaddr_in *servAddr, int port) {
    servAddr->sin_family = AF_INET;
    servAddr->sin_port = htons(port);
    servAddr->sin_addr.s_addr = INADDR_ANY;
}

typedef struct {
    int servSocket;
    int connSocket;
    int occupied;
    int index;
} clientConn;


static pthread_t threads[MAX_CLIENT];
static clientConn clients[MAX_CLIENT];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int clientCount = 0;

int availableSlot() {
    /* Functionality:
     *  - Search through client pool to see if there's an available slot for a new client
     * Return:
     *  - Returns index of available slot or -1 if there's no available slot
     */
    pthread_mutex_lock(&mutex);
    if (clientCount >= MAX_CLIENT) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (clients[i].occupied != 1) {
            pthread_mutex_unlock(&mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

void removeClient(clientConn *client) {
    /* Functionality:
     *  - Used to clean the values inside a given clientConn struct
     * Params:
     *  - clientConn *client : clientConn struct object that we want to remove
     */
    client->servSocket = -1;
    client->connSocket = -1;
    client->occupied = 0;
    client->index = -1;
    clientCount--;
}

void sendToAllClient(char response[RESPSIZE]) {
    /* Functionality:
     *  - Send a given response to every connected client
     * Params:
     *  - char response[RESPSIZE] : the response that we want to send to all the client
     */
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (clients[i].occupied != 1) continue;
        send(clients[i].connSocket, response, strlen(response), 0);
    }
    pthread_mutex_unlock(&mutex);
}

void sendToAllButIndex(char response[RESPSIZE], int index) {
    /* Functionality:
     *  - Send a given response to every connected client except the client with the given index
     * Params:
     *  - char response[RESPSIZE] : the response that we want to send
     *  - int selfIndex : specific client index
     */
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (clients[i].occupied != 1 || i == index) continue;
        send(clients[i].connSocket, response, strlen(response), 0);
    }
    pthread_mutex_unlock(&mutex);
}

void *handleClient(void *args) {
    /* Functionality:
     *  - Thread function that handles a particular client
     *  - Logs the client activity and broadcase client message to all the other clients
     */
    clientConn *client = (clientConn *) args;
    int servSocket = client->servSocket;
    int connSocket = client->connSocket;
    
    char response[RESPSIZE], buffer[BUFSIZE], user[UNAMESIZE], key[KEYSIZE];
    memset(user, 0, UNAMESIZE);
    memset(response, 0, RESPSIZE);
    recv(connSocket, user, UNAMESIZE-1, 0);
    printf("Connection Log - %s Connected\n", user);
    sprintf(response, "> %s has joined", user);
    sendToAllButIndex(response, client->index);
    while (1) {
        memset(response, 0, RESPSIZE);
        memset(buffer, 0, BUFSIZE);
        memset(key, 0, KEYSIZE);
        recv(connSocket, buffer, BUFSIZE-1, 0);
        memcpy(key, buffer, KEYSIZE-1);
        if (strcmp(key, "QUIT")==0) {
            sprintf(response, "> %s has disconnected", user);
            sendToAllButIndex(response, client->index);
            removeClient(client);
            break;
        }
        printf("Message Log - %s: %s\n", user, buffer);
        sprintf(response, "%s : %s", user, buffer);
        sendToAllButIndex(response, client->index);
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

    // Create server socket
    int servSocket = socket(AF_INET, SOCK_STREAM, 0), connSocket;
    if (servSocket < 0) {
        printf("Server Socket Open Failed\n");
        return 1;
    }

    // Initialize server information and bind server
    struct sockaddr_in servAddr;
    initServerAddr(&servAddr, atoi(argv[1]));
    if (bind(servSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        printf("Server Socket Binding Failed\n");
        return 1;
    }

    printf("Server had started and is now running\n");
    // Start listening and handle clients
    listen(servSocket, 5);
    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientSockSize = sizeof(clientAddr);
        connSocket = accept(servSocket, (struct sockaddr *) &clientAddr, &clientSockSize);
        int available = availableSlot();
        if (available < 0) {
            printf("Max client amout has reached, cannot accept more users\n");
            send(connSocket, "CONN_FAILURE", KEYSIZE, 0);
            continue;
        }

        send(connSocket, "CONN_SUCCESS", KEYSIZE, 0);
        clients[available].servSocket = servSocket;
        clients[available].connSocket = connSocket;
        clients[available].occupied = 1;
        clients[available].index = available;
        pthread_create(&threads[available], NULL, handleClient, &clients[available]);
        clientCount++;
    }
    close(servSocket);
    return 0;
}

