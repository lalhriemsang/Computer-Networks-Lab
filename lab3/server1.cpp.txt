#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctime>
#include <cstdlib>
#include <sys/select.h>

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); 
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is waiting for connections..." << std::endl;

    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
    if (clientSocket == -1) {
        perror("Error in accept");
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected." << std::endl;

    int seqNum = 0;  

    while (true) {
        char message[1024];
        snprintf(message, sizeof(message), "Message with SeqNum %d.", seqNum);

        send(clientSocket, message, strlen(message), 0);

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
            std::cout << "Timeout occurred. Resending message." << std::endl;
        } else {
            char ack[1024];
            memset(ack, 0, sizeof(ack));
            ssize_t bytesReceived = recv(clientSocket, ack, sizeof(ack), 0);

            if (bytesReceived <= 0) {
                std::cerr << "Connection closed." << std::endl;
                break;
            }

            std::cout << "Received Acknowledgment: " << ack << std::endl;

            int ackSeqNum;
            if (sscanf(ack, "ACK %d", &ackSeqNum) == 1 && ackSeqNum == seqNum) {
                std::cout << "ACK received. Message delivered successfully." << std::endl;
            } else {
                std::cout << "NACK received. Resending message." << std::endl;
            }

            seqNum++; 
        }
    }

    close(serverSocket);
    return 0;
}

