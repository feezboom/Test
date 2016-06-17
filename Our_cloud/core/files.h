//
// Created by avk on 16.06.16.
//

#ifndef OUR_CLOUD_FILES_H
#define OUR_CLOUD_FILES_H

#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
#include <stdio.h>

#define DIR_LIST_MAX_BUF_SIZE 16384
#define FULL_PATH_MAX_SIZE 1024

char* get_current_dir() {
    return getenv("PWD");
}

int is_dir(char* filename, char* path) {
    char full_path[1024];
    sprintf(full_path, "%s/%s", path, filename);

    struct stat item_info;
    int result = lstat(full_path, &item_info);
    assert(result != -1);

    return S_ISDIR(item_info.st_mode);
}

void get_dir_list(char* future_dir_list) {
    char* current_path = get_current_dir();
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

#endif //OUR_CLOUD_FILES_H
