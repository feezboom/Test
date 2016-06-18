//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_CLOUD_INTERFACE_H
#define OUR_CLOUD_CLOUD_INTERFACE_H

void perform_get(int sock) {
    char dir_list_to_send[DIR_LIST_MAX_BUF_SIZE];
    get_dir_list(dir_list_to_send);
    send_message(dir_list_to_send, sock);
}

void perform_download(int sock, char* fullpath_from) {
    // todo
}

void perform_upload(int sock, char* fullpath_to, char* filename) {
    void* file;
    long received = receive_file(sock, &file);
    if (received == -1) {
        return;
    }
    char full_name[FULL_PATH_MAX_SIZE];
    sprintf(full_name, "%s/%s", fullpath_to, filename);
    FILE* new_file = fopen(full_name, "wb");
    fwrite(file, 1, received, new_file);
    fclose(new_file);
    free(file);
    send_message("Successfully uploaded.\n", sock);
    // todo
}

#endif //OUR_CLOUD_CLOUD_INTERFACE_H
