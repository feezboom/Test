//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_CLOUD_INTERFACE_H
#define OUR_CLOUD_CLOUD_INTERFACE_H

#include "files.h"
#include "networking.h"
#include "cloud.h"


void perform_getlist(int sock) {
    char dir_list_to_send[DIR_LIST_MAX_BUF_SIZE];
    get_full_dir_list(dir_list_to_send);
    send_message(dir_list_to_send, sock);
}

void perform_download(int sock, char* filename) {
    // Читаем его
    int node_id = chose_node(filename);
    char path[FULL_PATH_MAX_SIZE];
    sprintf(path, "%s/%s", storage[node_id].destination, filename);
    FILE* file = fopen(path, "rb");
    if (!file) {
        // Первый send_message() это обязательно либо OK, либо FAIL
        // Это как идентификатор успеха:

        // Клиент отправляет запрос upload bla.bla
        // - Сервер получает запрос
        // - Видит - 1 (download)
        // - Пробует найти этот файл
        // - Если что-то не так (файл не найден или что-то ещё), то
        // есть по каким-то причинам файл отправить не получится
        // Сервер отправляет клиенту "FAIL", и после сообщение почему FAIL
        // - Если все идёт по плану, то сервер отправит клиенту "OK",
        // а после и сам файл.

        // Клиент же:
        // - Отправляет запрос
        // - Получает ответ в виде "OK"/"FAIL"
        // - Если "OK" - то принимает файл
        // - Если "FAIL" - то файл не принимает,
        // а принимает только сообщение об ошибке, которое отправил сервер.
        send_message("FAIL", sock);
        send_message("No such file on cloud\n", sock);
        return;
    }
    long size = get_file_size(path);
    if (size == -1) {
        send_message("FAIL", sock);
        send_message("internal server error\n", sock);
        throw_error("problem getting file_size (download)", sock);
        fclose(file);
        return;
    }
    void *content = malloc((unsigned long)size);  // Выделяем память для загрузки файла в память.
    if (content == NULL) {
        send_message("FAIL", sock);
        send_message("internal server error\n", sock);
        throw_error("malloc problem", -1);
        fclose(file);
        return;
    }

    send_message("OK", sock);
    fread(content, 1, (unsigned long)size, file); // Читаем файл в память.
    long result = send_file(sock, content, size); // Отправляем уже из памяти.

    if (result != size) {
        throw_error("sending file problem\n", sock);
        free(content);
        fclose(file);
        return;
    }
    send_message("Successfully sent\n", sock);
    free(content);
    fclose(file);
}

void perform_upload(int sock, char* filename) {
    void* content;
    // В receive_file в указатель content выделяется память в content
    // с помощью malloc, её нужно будет освободить.
    long received = receive_file(sock, &content);
    if (received == -1) {
        return;
    }

    int node_id = chose_node(filename);
    char path1[FULL_PATH_MAX_SIZE];
    char path2[FULL_PATH_MAX_SIZE];
    sprintf(path1, "%s/%s", storage[node_id].destination, filename);
    sprintf(path2, "%s/duplicates/%s", storage[(node_id + 1) % storage_size].destination, filename);


    // Создаём этот файл - открываем на запись.
    FILE* new_file = fopen(path1, "wb");
    FILE* new_file_duplicate = fopen(path2, "wb");
    // Пишем в этот файл.
    fwrite(content, 1, (unsigned long)received, new_file);
    fwrite(content, 1, (unsigned long)received, new_file_duplicate);

    fclose(new_file);
    fclose(new_file_duplicate);
    free(content);

    // Шлём сообщение об успехе.
    // Отправим отчёт клиенту о том, что файл успешно загружен.
    send_message("Successfully uploaded.\n", sock);
}

void perform_delete(int sock, char* filename) {
    int node_id = chose_node(filename);
    char path1[FULL_PATH_MAX_SIZE];
    char path2[FULL_PATH_MAX_SIZE];
    sprintf(path1, "%s/%s", storage[node_id].destination, filename);
    sprintf(path2, "%s/duplicates/%s", storage[(node_id + 1) % storage_size].destination, filename);

    int result = remove(path1);
    if (result == -1) {
        send_message("FAIL", sock);
        send_message("No such file on server to delete!\n", sock);
        return;
    }
    result = remove(path2);
    if (result == -1) {
        send_message("FAIL", sock);
        send_message("Duplicate delete problem!\n", sock);
        return;
    }
    send_message("OK", sock);
    send_message("Successfully removed from the server!\n", sock);
    return;
}

void perform_deactivate(char* node_id, int sock) {
    int id = atoi(node_id);
    if (id >= storage_size || id < 0) {
        send_message("wrong input", sock);
        return;
    }
    char* path = storage[id].destination;
    clean_directory(path);
    remove_node(id);

    const char* path_next = storage[(id+1)%storage_size].destination;
    char path_next_dup[FULL_PATH_MAX_SIZE]; // Путь к дубликатам i+1
    const char* path_prev = storage[(id-1)%storage_size].destination;
    char path_prev_dup[FULL_PATH_MAX_SIZE]; // Путь к дубликатам i-1

    sprintf(path_next_dup, "%s/duplicates", path_next);
    sprintf(path_prev_dup, "%s/duplicates", path_prev);

    char prev_dup_list[DIR_LIST_MAX_BUF_SIZE]; // Список дубликатов в i-1
    char next_dup_list[DIR_LIST_MAX_BUF_SIZE]; // Список дубликатов в i+1

    get_dir_list(prev_dup_list, path_prev_dup);
    get_dir_list(next_dup_list, path_next_dup);

    // (1) Все дубликаты из i+1 скопировать в i-1
    // (2) Все не дубликаты из i-1 скопировать в дубликаты i+1



}

#endif //OUR_CLOUD_CLOUD_INTERFACE_H
