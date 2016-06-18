//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_FILES_H
#define OUR_CLOUD_FILES_H

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
#include <stdio.h>

#define DIR_LIST_MAX_BUF_SIZE 16384
#define FULL_PATH_MAX_SIZE 1024

void get_current_dir_path(char *path) {
    // Берет значение переменной окружения - PWD.
    // Именно в этой переменной хранится путь к текущей директории.
    getcwd(path, FULL_PATH_MAX_SIZE);
    return;
}

int is_dir(char* filename, char* path) {
    char full_path[1024];
    sprintf(full_path, "%s/%s", path, filename);

    struct stat item_info;
    int result = lstat(full_path, &item_info);
    assert(result != -1);

    return S_ISDIR(item_info.st_mode);
}

void get_dir_list(char* future_dir_list, const char* current_path_) {
    char current_path[FULL_PATH_MAX_SIZE];
    sprintf(current_path, current_path_);
// get_current_dir_path(current_path);
    strcpy(future_dir_list, "");

    DIR* directory = opendir(current_path);
    struct dirent* item;
    while(NULL != (item = readdir(directory))) {
// ^^^
// |||
// Следующий элемент папки

        char* item_name = item->d_name;
        if(!strcmp(item_name, ".") ||
           !strcmp(item_name, "..") || (item_name[0] == '.')) {
// Если это текущая директория (.),
// либо родительская (..),
// либо начинается с точки - значит скрытая (.*),
// то её показывать никому не надо.
            continue;
        }

        if (!is_dir(item_name, current_path)) {
            strcat(future_dir_list, item_name);
            strcat(future_dir_list, "\n");
        }
    }
    closedir(directory);
}


void get_full_dir_list(char * future_full_list){
//int i;
    strcpy(future_full_list, "");
    char part_full_list[DIR_LIST_MAX_BUF_SIZE];
    int num_nodes = storage_size;
    for (i = 0; i < num_nodes; i++){
        char* cur_dir = (storage + i)->destination;
        get_dir_list(part_full_list, cur_dir);
        strcat(future_full_list, part_full_list);
    }
}

long get_file_size(const char* full_path) {
    struct stat file_info;
    int result = lstat(full_path, &file_info);
    if (result == -1) {
        fprintf(stderr, "Can't get file size : %s\n", full_path);
        return -1;
    }
    return file_info.st_size;
}

// Возвращает id узла по хэшу
int chose_node(const char *filename) {
    return HashFAQ6(filename) % storage_size;
}

void create_duplicates_directories() {
    int num_nodes = storage_size;
    char* duplicate = "/duplicates";
    for (i = 0; i < num_nodes; i++){
        char duplicate_dir[1000];
        char* cur_dir = (storage + i)->destination;
        sprintf(duplicate_dir, "%s%s", cur_dir, duplicate);
        mkdir(duplicate_dir, 0777);
    }
}
// copy взял со stack_overflow
int copy(const char *to, const char *from) {
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

    out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}


int remove_file(char* path, char* filename) {
    char temp[FULL_PATH_MAX_SIZE];
    sprintf(temp, "%s/%s", path, filename);
    return remove(temp);
}


// Копирует все файлы из списка(раделенных\n) из from в to
void copy_all(const char* from, const char* to, const char* file_list) {
    char temp[DIR_LIST_MAX_BUF_SIZE];
    strcpy(temp, file_list);
    char* filename = strtok(temp, "\n\0");
    while (filename != NULL) {

    }
}

void clean_directory(char* path) {
    DIR* directory = opendir(path);
    struct dirent* item;
    while(NULL != (item = readdir(directory))) {
// ^^^
// |||
// Следующий элемент папки

        char* item_name = item->d_name;
        if(!strcmp(item_name, ".") ||
           !strcmp(item_name, "..") || (item_name[0] == '.')) {
// Если это текущая директория (.),
// либо родительская (..),
// либо начинается с точки - значит скрытая (.*),
// то её показывать никому не надо.
            continue;
        }

        if (!is_dir(item_name, path)) {
            char path_to_remove[10000];
            sprintf(path_to_remove, path);
            strcat(path_to_remove, item_name);
            remove(path_to_remove);
        }
    }
    closedir(directory);
    char path_duplicate[10000];
    char * duplicates = "/duplicates";
    sprintf(path_duplicate, "%s%s", path, duplicates);
    rmdir(path_duplicate);
}

#endif //OUR_CLOUD_FILES_H
