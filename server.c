#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS_NUMBER 10
#define BUFFER_SIZE 1048

unsigned int connectedClientsArray[MAX_CLIENTS_NUMBER];
unsigned int clientsCounter = 0;

pthread_mutex_t aMutex = PTHREAD_MUTEX_INITIALIZER;

void *Handle_Client(void *arg);
void Send_Message_to_ALL_Clients(char msg[], int n, int recentClient);
void Send_Message_to_A_Client(char msg[], char name[], int recieverClient);

int main()
{
    pthread_mutex_init(&aMutex, NULL);

    int listenSockFD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(5555);

    int option = 1;

    if (setsockopt(listenSockFD, SOL_SOCKET, SO_REUSEADDR, (void *)&option, (socklen_t)sizeof(option)) == -1)
    {
        printf("Error in setsockopt()\n");
        exit(-1);
    }

    if (bind(listenSockFD, (struct sockaddr *)&serverAddress, (socklen_t)sizeof(serverAddress)) == -1)
    {
        printf("Error in binding\n");
        exit(-1);
    }

    if (listen(listenSockFD, MAX_CLIENTS_NUMBER) == -1)
    {
        printf("Error in listening\n");
        exit(-1);
    }
    else
        printf("===== Welcome to the chatting room =====\n");

    pthread_t threadID;
    int connectionServerSockFD = 0;

    while (1)
    {
        struct sockaddr_in aClientAddress;
        memset(&aClientAddress, 0, sizeof(aClientAddress));

        socklen_t clientAddrLength = sizeof(aClientAddress);
        connectionServerSockFD = accept(listenSockFD, (struct sockaddr *)&aClientAddress, &clientAddrLength);

        if (connectionServerSockFD == -1)
        {
            printf("Error in accept\n");
        }
        else
        {
            pthread_mutex_lock(&aMutex);
            connectedClientsArray[clientsCounter++] = connectionServerSockFD;
            pthread_mutex_unlock(&aMutex);

            pthread_create(&threadID, NULL, Handle_Client, (void *)&connectionServerSockFD);
            pthread_detach(threadID);

            printf("connected client IP: %s\n", inet_ntoa(aClientAddress.sin_addr));
        }
    }

    close(listenSockFD);

    return 0;
}

void *Handle_Client(void *arg)
{
    int connectedClientSocket = *((int *)arg);

    char message[BUFFER_SIZE];

    int strLen = 0;

    while ((strLen = read(connectedClientSocket, message, BUFFER_SIZE - 1)) != 0)
    {

        char *first, *second, *remaining;

        first = strtok(message, ":");
        second = strtok(NULL, ":");
        remaining = strtok(NULL, ":");

        if (strcmp(second, "all") == 0)
        {
            strcat(first, ":");
            strcat(first, remaining);
            Send_Message_to_ALL_Clients(first, strlen(first), connectedClientSocket);
        }
        else
        {
            Send_Message_to_A_Client(remaining, first, atoi(second));
        }
    }

    // removing serviced client from the array

    int i = 0;

    pthread_mutex_lock(&aMutex);

    for (i = 0; i < clientsCounter; i++)
    {
        if (connectedClientSocket == connectedClientsArray[i])
        {
            while (i < clientsCounter && connectedClientsArray[i] != 0)
            {
                connectedClientsArray[i] = connectedClientsArray[i + 1];
                i++;
            }
            connectedClientsArray[i - 1] = 0;
            break;
        }
    }
    --clientsCounter;
    printf("disconnected client\n");
    pthread_mutex_unlock(&aMutex);

    close(connectedClientSocket);

    return NULL;
}

void Send_Message_to_ALL_Clients(char msg[], int n, int recentClient)
{
    int i;

    pthread_mutex_lock(&aMutex);

    for (i = 0; i < clientsCounter; i++)
    {
        if (connectedClientsArray[i] == recentClient)
            continue;

        char *newmsg = msg, *first, id[];
        first = strtok(newmsg, ":");
        sprintf(id, "%d", connectedClientsArray[i]);
        strcat(first, id);
        write(connectedClientsArray[i], msg, n);
    }

    pthread_mutex_unlock(&aMutex);
}

void Send_Message_to_A_Client(char msg[], char name[], int recieverClient)
{
    int i, matched = 0;

    pthread_mutex_lock(&aMutex);

    for (i = 0; i < clientsCounter; i++)
    {
        if (connectedClientsArray[i] == recieverClient)
        {
            char buffout[BUFFER_SIZE];

            sprintf(buffout, "[%s]:%s", name, msg);

            write(connectedClientsArray[i], buffout, strlen(buffout));

            matched++;
            break;
        }
    }

    if (matched == 0)
        printf("NO such client\n");

    pthread_mutex_unlock(&aMutex);
}
