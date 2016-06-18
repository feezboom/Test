//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_CLOUD_INTERFACE_H
#define OUR_CLOUD_CLOUD_INTERFACE_H

void perform_getlist(int sock) {
    char dir_list_to_send[DIR_LIST_MAX_BUF_SIZE];
    get_dir_list(dir_list_to_send);
    send_message(dir_list_to_send, sock);
}

void perform_download(int sock, char* full_path) {
    // Читаем его
    FILE* file = fopen(full_path, "rb");
    if (!file) {
#ifdef server
        // Первый send_message() это обязательно либо OK, либо FAIL
        // Это как идентификатор успеха:

        // Клиент отправляет запрос upload bla.bla
        // - Сервер получает запрос
        // - Парсит его
        // - Видит upload
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
#endif
        return;
    }
    long size = get_file_size(full_path);
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

void perform_upload(int sock, char* full_path_to) {
    void* content;
    // В receive_file в указатель content выделяется память в content
    // с помощью malloc, её нужно будет освободить.
    long received = receive_file(sock, &content);
    if (received == -1) {
        return;
    }
    // Создаём этот файл - открываем на запись.
    FILE* new_file = fopen(full_path_to, "wb");
    // Пишем в этот файл.
    fwrite(content, 1, (unsigned long)received, new_file);

    fclose(new_file);
    free(content);

    // Шлём сообщение об успехе.
    // Отправим отчёт клиенту о том, что файл успешно загружен.
#ifdef server
    send_message("Successfully uploaded.\n", sock);
#endif

}

#endif //OUR_CLOUD_CLOUD_INTERFACE_H
