#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5


void handleClientRequest(int client_socket) {
    send(client_socket, "hello", strlen("hello"), 0);
    return;
}


int main() {
    // Initialize socket vars
    int server_fd, client_socket, max_sd, activity;
    int opt = 1;
    struct sockaddr_in address;
    fd_set readfds;
    int addrlen = sizeof(address);
    int client_sockets[MAX_CLIENTS];

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options. Let server reuse the same addr/port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind server socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Initialize client sockets
    for (int i=0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // Server Loop. Accept incoming connections and handle them
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Wait for activity on any socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // If activity on server socket, accept new connection
        if (FD_ISSET(server_fd, &readfds)) {
            if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("Client connected - IP address: [%s] Port: [%d]\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add new client socket to array
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_socket;
                    FD_SET(client_socket, &readfds);

                    if (client_socket > max_sd) {
                        max_sd = client_socket;
                    }
                    break;
                }
            }
        }

        // Handle client requests
        for (int i = 0; i < MAX_CLIENTS; i++) {
            client_socket = client_sockets[i];

            if (FD_ISSET(client_socket, &readfds)) {
                handleClientRequest(client_socket);
            }
        }   
    }

    close(server_fd);
    return 0;
}