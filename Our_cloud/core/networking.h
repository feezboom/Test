//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_NETWORKING_H
#define OUR_CLOUD_NETWORKING_H

// networking
#include <sys/socket.h>
#include <netinet/in.h>


// получение заранее известного числа байт
int recv_all(int sfd, void* buf, int len, int flags) {
    int total = 0;
    while(total < len)
    {
        ssize_t n = recv(sfd, buf + total, len - total, flags);
        if (n == -1)
            return -1;
        if (n == 0)
            return total;
        total += n;
    }
    return total;
}

// отправка заранее известного числа байт
int send_all(int sfd, void* buf, int len, int flags)
{
    int total = 0;
    while(total < len)
    {
        ssize_t n = send(sfd, buf + total, len - total, flags);
        if(n == -1)
            return -1;
        total += n;
    }
    return total;
}

int receive_message(char* str, int sock) {
    int size;

    int bytes_received = recv_all(sock, &size, sizeof(int), 0);
    if (bytes_received == -1) {
        return -1;
    }

    bytes_received = recv_all(sock, str, size, 0);
    if (bytes_received == -1) {
        return -1;
    }

    str[bytes_received] = '\0';
    return bytes_received;
}

int send_message(char* str, int sock) {
    int size = strlen(str);
    int bytes_sended = send_all(sock, &size, sizeof(int), 0);
    if (bytes_sended == -1) {
        return -1;
    }

    bytes_sended = send_all(sock, str, size, 0);
    if (bytes_sended == -1) {
        return -1;
    }

    return bytes_sended;
}

int receive_file(int sock, void* pointer_where_to_alloc)
{
    long file_size;
    int result = recv_all(sock, &file_size, sizeof(int), 0);
    if (result != 4) {
        throw_error("receiving file_size", sock);
        return -1;
    }
    assert(file_size > 0);

    pointer_where_to_alloc = malloc((unsigned long)file_size);
    result = recv_all(sock, pointer_where_to_alloc, file_size, 0);
    if (result < file_size) {
        throw_error("receiving file", sock);
        return -1;
    }


    return received_bytes;
}
#endif //OUR_CLOUD_NETWORKING_H
