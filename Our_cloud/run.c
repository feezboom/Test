//
// Created by avk on 16.06.16.
//

#define DEFAULT_PORT 2288

#define server

#include "core/cloud.h"

// На вход подаются места для хранения - наши узлы (nodes)
int main(int argc, char** argv) {
    int nodes_number = argc;
    char** nodes = argv;

    run_cloud(nodes_number, nodes, DEFAULT_PORT);
    return 0;
}