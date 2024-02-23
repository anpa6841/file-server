#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
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


int handleClientAuth(int client_socket, int *client_sockets) {
    char username[50];
    char password[50];

    send(client_socket, "Username: ", strlen("Username: "), 0);
    recv(client_socket, username, 50, 0);
    username[strcspn(username, "\n")] = 0;

    send(client_socket, "Password: ", strlen("Password: "), 0);
    recv(client_socket, password, 50, 0);
    password[strcspn(password, "\n")] = 0;

    if (authenticate(username, password)) {
        send(client_socket, "Authentication successful.\n", strlen("Authentication successful.\n"), 0);
        return 1;
    } else {
        send(client_socket, "Authentication failed.\n", strlen("Authentication failed.\n"), 0);
        close(client_socket);
        remove_client_socket(client_socket, client_sockets);
    }
    return 0;
}


void handleDownload(int client_socket, char *filename) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Open file for reading
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("File open");
        return;
    }

    // Send filecontents to client
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    send(client_socket, "Transfer Complete", strlen("Transfer Complete"), 0);
    fclose(file);
    printf("File sent: %s\n", filename);
}


void handleUpload(int client_socket, char *filename) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Open file for writing
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("File open");
    }

    // Receive file contents and write to file
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    send(client_socket, "Transfer Complete", strlen("Transfer Complete"), 0);
    fclose(file);
    printf("File uploaded successfully: %s\n", filename);
}


void handleClientRequest(int client_socket, int *client_sockets) {
    char action[20];
    char filename[20];

    char select_action[] = "Please select 'upload' or 'download' action: ";
    char select_file[] = "Enter Filename: ";

    send(client_socket, select_action, strlen(select_action), 0);
    recv(client_socket, action, 20, 0);

    send(client_socket, select_file, strlen(select_file), 0);
    recv(client_socket, filename, 20, 0);

    // remove trailing newline
    action[strcspn(action, "\n")] = 0;
    filename[strcspn(filename, "\n")] = 0;

    if (strcmp(action, "download") == 0) {
        handleDownload(client_socket, filename);
    } else if (strcmp(action, "upload") == 0) {
        handleUpload(client_socket, filename);
    }
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
                if (handleClientAuth(client_socket, client_sockets)) {
                    handleClientRequest(client_socket, client_sockets);
                }
            }
        }   
    }

    close(server_fd);
    return 0;
}