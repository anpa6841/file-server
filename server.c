#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5
#define MAX_USERS 4

struct User {
    char username[50];
    char password[50];
};

struct User users[MAX_USERS] = {
    {"bob", "pass"},
    {"alice", "pass"},
    {"mike", "pass"},
    {"ross", "pass"}
};

int authenticate(char *username, char *password) {
    int auth = 0;

    for (int i = 0; i < MAX_USERS; i++) {
        if (strcmp(users[i].username, username) == 0  && strcmp(users[i].password, password) == 0) {
            auth = 1;
            break;
        }
    }

    return auth;
}


void remove_client_socket(int client_socket, int *client_sockets) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = 0;
            break;
        }
    }
}


void handleClientRequest(int client_socket, int *client_sockets) {
    char username[50];
    char password[50];

    send(client_socket, "Username: ", strlen("Username: "), 0);
    recv(client_socket, username, 50, 0);
    username[strcspn(username, "\n")] = 0; // remove trailing newline

    send(client_socket, "Password: ", strlen("Password: "), 0);
    recv(client_socket, password, 50, 0);
    password[strcspn(password, "\n")] = 0; // remove trailing newline

    if (authenticate(username, password)) {
        send(client_socket, "Authentication successful.\n", strlen("Authentication successful.\n"), 0);
    } else {
        send(client_socket, "Authentication failed.\n", strlen("Authentication failed.\n"), 0);
        close(client_socket);
        remove_client_socket(client_socket, client_sockets);
    }
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
                handleClientRequest(client_socket, client_sockets);
            }
        }   
    }

    close(server_fd);
    return 0;
}