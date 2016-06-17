//
// Created by avk on 16.06.16.
//

#include "client.h"

int main(int argc, char** argv) {
    setbuf(stdout, 0);
    int sock = run_client(argc, argv);
    client_processing(sock);
    return 0;
}