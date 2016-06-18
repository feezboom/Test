//
// Created by avk on 16.06.16.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "../core/cloud.h"


void clean_all() {};

void parse(char* cmd, int sock) {
    char* type = strtok(cmd, " \n\0");
    if (!strcasecmp(type, "upload")) {
        char* filename = strtok(NULL, " \n\0");
        char path[1024];
        getcwd(path, sizeof(path));
        strcat(path, "/");
        strcat(path, filename);

        FILE* new_file = fopen(path, "rb");
        struct stat st;
        if (lstat(filename, &st) == -1) {
            fprintf(stderr, "No such file\n");
            return;
        };
        long size = st.st_size;
        void* ptr = malloc(size);
        fread(ptr, 1, size, new_file);
        send_file(sock, ptr, size);
    }
}

int client_processing(int sock) {
    char cmd[10000];
    while (strcmp(cmd, "exit")) {
        printf("Введите действие --> ");
        fgets (cmd, 10000, stdin);
//        printf("output command - %s\n", cmd);
        // sending size
        size_t size = strlen(cmd);
        send_all(sock, &size, sizeof(int), 0);
        // sending data
        send_all(sock, cmd, strlen(cmd), 0);
        parse(cmd, sock);
        char buffer[16384];
        receive_message(buffer, sock);
        printf(buffer);
    }
    printf("Клиент завершил свою работу...\n");
    return 0;
}


int run_client(int argc, char** argv) {
    if (argc < 2) {
        // Это означает, что ip мы не получили
        clean_all();
        printf("smth\n");
        exit(23);
    }
    char* ip = strtok(argv[1], ":");
    uint16_t port = (uint16_t)atoi(strtok(NULL, "\0 \n:"));

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(ip);


    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(client_socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        printf("Ошибка подключения.\n"); //%d. Повтор через 2 секунды...\n", errno);
        exit(23);
        //sleep(2);
    }
    printf("Подключение успешно :)\n");
    return client_socket;
}


#endif //CLIENT_CLIENT_H
