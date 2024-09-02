#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctime>
#include <cstdlib>
#include <sys/select.h>
#include <cstdio>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error in connect");
        exit(EXIT_FAILURE);
    }

    int expectedSeqNum = 0;

    while (true) {
        char message[1024];
        memset(message, 0, sizeof(message));
        ssize_t bytesReceived = recv(clientSocket, message, sizeof(message), 0);

        if (bytesReceived <= 0) {
            std::cerr << "Connection closed." << std::endl;
            break;
        }

        std::cout << "Received Message: " << message << std::endl;

        char ack[1024];
        snprintf(ack, sizeof(ack), "ACK %d", expectedSeqNum);

        struct timeval timeout;
        timeout.tv_sec = 1; // 1 second timeout
        timeout.tv_usec = 0;
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(clientSocket, &read_fds);

        int ready = select(clientSocket + 1, &read_fds, NULL, NULL, &timeout);
        if (ready == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        } else if (ready == 0) {
            std::cout << "Timeout occurred. Resending acknowledgment." << std::endl;
        } else {
            send(clientSocket, ack, strlen(ack), 0);

            expectedSeqNum++; 
        }
    }

    close(clientSocket);
    return 0;
}

