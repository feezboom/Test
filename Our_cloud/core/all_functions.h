//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_ALL_FUNCTIONS_H
#define OUR_CLOUD_ALL_FUNCTIONS_H

#include <stdint.h>

int main(int argc, char** argv);

// Cloud Functions
typedef struct node node;
void run_cloud(int number, char** destinations, uint16_t port);
void init_storage(int number, char** destinations);
void update_node(node* node_, char* dest, int id, int pair_id);
void start_handling_client_requests(int sock);
void send_files_list(int sock, char* to_send);

// Server
int prepare_server(uint16_t port);
void run_server(int sock);
int* run_threads(int threads_number, int* our_pipe);
void* thread_processing(void* our_pipe);

// Networking
int recv_all(int sfd, void* buf, int len, int flags);
int send_all(int sfd, void* buf, int len, int flags);
int receive_message(char* str, int sock);
int send_message(char* str, int sock);

// Files
void get_dir_list(char* buffer);

// Error function
void throw_error(char* message, int sock) {
    fprintf(stderr, "Error : %s (sock = %d)", message, sock);
    return;
}
#endif //OUR_CLOUD_ALL_FUNCTIONS_H
