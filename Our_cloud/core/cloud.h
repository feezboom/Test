//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_CLOUD_H
#define OUR_CLOUD_CLOUD_H

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#include "all_functions.h"

#include "networking.h"

#define BUF_SIZE 1024

// commands
#define get_list "0"
#define download "1"
#define upload "2"
#define finish "3"
#define delete_ "4"
#define deactivate "5"


// Файл, который будет загружаться на сервер, будет попадать
// в узел с номером, который будет вычислен хэш-функцией.
unsigned int HashFAQ6(const char * str) {
    unsigned int hash = 0;

    for (; *str; str++)
    {
        hash += (unsigned char)(*str);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

// Дубликат этого файла, будет, предположительно, храниться
// в соседнем узле (т.е. в (i+1)-ом storage[i] -> storage[i+1])

// В каждом узле будет храниться список его файлов.


typedef struct node {
    int id;
    int pair_id;
    char* destination;
} node;


static int i,j;
node* storage; // Динамическая память, выделяется в init_storage
static int storage_size = -1;

void update_node(node* node_, char* dest, int id, int pair_id) {
    node_->destination = dest;
    node_->id = id;
    node_->pair_id = pair_id;
}
void remove_node(int node_id){
    for (i = node_id; i < storage_size - 1; i++){
        (storage + i)->destination = (storage + i + 1)->destination;
    }
    realloc(storage, (storage_size - 1) * sizeof(node));
    --storage_size;
}
void init_storage(int number, char** destinations) {
    storage = (node*)malloc(number*sizeof(node));
    for (i = 0; i < number; ++i) {
        update_node(storage+i, destinations[i+1], i, (i+1) % number);
    }
    storage_size = number-1;
    create_duplicates_directories();
}


#include "server.h"
#include "files.h"
#include "cloud_interface.h"

// Принимает и обрабатывает запросы уже подключившегося клиента
// с сокетом sock
void start_handling_client_requests(int sock) {
    char full_path[FULL_PATH_MAX_SIZE];
    char current_dir[FULL_PATH_MAX_SIZE];
    get_current_dir_path(current_dir);

    while (1) {
        char handled_message[BUF_SIZE];
        receive_message(handled_message, sock);


        if (handled_message == NULL || !strcmp("", handled_message)) {
            // Клиент отключился?
            printf("Клиент (sock_fd = %d) отключился?\n", sock);
            return;
        }

        if (!strcasecmp(get_list, handled_message)) { // getlist

            perform_getlist(sock);

        } else if (!strcasecmp(download, handled_message)) { // download

            char filename[1024];
            receive_message(filename, sock);
            perform_download(sock, filename);

        } else if (!strcasecmp(upload, handled_message)) { // upload

            char filename[1024];
            receive_message(filename, sock);
            perform_upload(sock, filename);

        } else if (!strcasecmp(finish, handled_message)) { // exit
            // С этим клиентом - всё.
            // Будем подбирать следующего в thread_processing
            return;
        } else if (!strcasecmp(delete_, handled_message)) { // delete
            char filename[1024];
            receive_message(filename, sock);
            perform_delete(sock, filename);
        } else if (!strcasecmp(deactivate, handled_message)) {
            char node_id[1024];
            receive_message(node_id, sock);
            perform_deactivate(node_id, sock);

        } else {
            fprintf(stderr, "Недопустимое действие (\"%s\") со стороны sock_fd = %d.\n", handled_message, sock);
        }
    }
}

void run_cloud(int number, char** destinations, uint16_t port) {
    setbuf(stdout, 0);
    setbuf(stderr, 0);

    init_storage(number, destinations);
    int sock = prepare_server(port);
    run_server(sock);
}



#endif //OUR_CLOUD_CLOUD_H
