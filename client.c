#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1048
#define NAME_SIZE 20

char name[NAME_SIZE];

void *Send_Message(void *arg);
void *Recieve_Message(void *arg);

int main()
{
    pthread_t sendThreadID;
    pthread_t recvThreadID;

    int clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD == -1)
    {
        printf("Error in socket creation\n");
        exit(-1);
    }
    else
    {
        printf("Client's Socket: %d\n", clientSocketFD);
    }

    struct sockaddr_in passiveServerAddress;
    memset(&passiveServerAddress, 0, sizeof(passiveServerAddress));

    passiveServerAddress.sin_family = AF_INET;
    passiveServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    passiveServerAddress.sin_port = htons(5555);
    if (connect(clientSocketFD, (struct sockaddr *)&passiveServerAddress, sizeof(passiveServerAddress)) != 0)
    {
        printf("error in connection\n");
        exit(-1);
    }
    else
    {
        printf("Connected....\n");
        printf("Enter your first name: ");

        fgets(name, NAME_SIZE, stdin);
        name[strlen(name) - 1] = '\0';

        pthread_create(&sendThreadID, NULL, Send_Message, (void *)&clientSocketFD);
        pthread_create(&recvThreadID, NULL, Recieve_Message, (void *)&clientSocketFD);
    }

    pthread_join(sendThreadID, NULL);
    pthread_join(recvThreadID, NULL);

    close(clientSocketFD);
    return 0;
}

void *Send_Message(void *arg)
{
    int connectedClientSocket = *((int *)arg);
    char message[BUFFER_SIZE] = {0};
    char NamedMessage[BUFFER_SIZE + NAME_SIZE + 20] = {0};

    while (1)
    {
        fgets(message, BUFFER_SIZE, stdin);
        if (message[0] == 'q' || message[0] == 'Q')
        {
            close(connectedClientSocket);
            exit(0);
        }
        else
        {
            sprintf(NamedMessage, "%s:%s", name, message);
            printf("%d\n", connectedClientSocket);
            write(connectedClientSocket, NamedMessage, strlen(NamedMessage));
        }
    }

    return NULL;
}

void *Recieve_Message(void *arg)
{
    int connectedClientSocket = *((int *)arg);
    char NamedMessage[BUFFER_SIZE + NAME_SIZE + 1] = {0};

    int strlen = 0;

    while (1)
    {
        strlen = read(connectedClientSocket, NamedMessage, BUFFER_SIZE + NAME_SIZE);
        if (strlen == -1)
        {
            exit(-1);
        }
        else
        {
            NamedMessage[strlen] = '\0';
            printf("%s", NamedMessage);
        }
    }

    return NULL;
}
