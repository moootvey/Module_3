#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void listFiles(const char *path) {
    DIR *dir;
    struct dirent *entry;

    // Открыть каталог
    dir = opendir(path);
    if (dir == NULL) {
        printf("Ошибка открытия каталога '%s'\n", path);
        return;
    }

    // Просмотр содержимого каталога
    while ((entry = readdir(dir)) != NULL) {
        // Пропустить "." и ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Определение типа файла: каталог или обычный файл
        if (entry->d_type == DT_DIR) {
            printf("[Папка] %s\n", entry->d_name);
        } else {
            printf("[Файл] %s\n", entry->d_name);
        }
    }

    closedir(dir);
}


int main() {
    char path[1000];

    printf("Введите путь к каталогу: ");
    scanf("%s", path);

    listFiles(path);

    int choice;
    do {
        printf("\nВыберите действие:\n");
        printf("1. Перейти в подкаталог\n");
        printf("2. Вернуться на один уровень назад\n");
        printf("0. Выйти из программы\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);

        if (choice == 1) {
            char subdir[1000];
            printf("Введите имя подкаталога: ");
            scanf("%s", subdir);

            size_t new_path_size = strlen(path) + 1 + strlen(subdir) + 1;
            char *new_path = (char*)malloc(new_path_size);

            if (new_path == NULL) {
                printf("Ошибка выделения памяти.\n");
                exit(EXIT_FAILURE);
            }

            snprintf(new_path, new_path_size, "%s/%s", path, subdir);

            DIR *temp_dir = opendir(new_path);
            if (temp_dir != NULL) {
                closedir(temp_dir);
                strcpy(path, new_path);
                listFiles(path);
            } else {
                printf("Подкаталог '%s' не существует или это не каталог.\n", subdir);
            }

            free(new_path);
        } else if (choice == 2) {
            // Возврат на один уровень назад
            char *last_slash = strrchr(path, '/');
            if (last_slash != NULL) {
                *last_slash = '\0'; // Удаляем последний слэш, чтобы вернуться на уровень назад
            } else {
                printf("Невозможно вернуться на один уровень назад.\n");
            }

            // Отображаем содержимое нового каталога
            listFiles(path);
        } else if (choice != 0) {
            printf("Некорректный выбор. Пожалуйста, выберите 0, 1 или 2.\n");
        }
    } while (choice != 0);

    return 0;
}
