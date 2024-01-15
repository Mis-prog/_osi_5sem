#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "src_lib/calculation.h"

struct User {
    char username[20];
    char password[20];
};

const char file_name_user[] = "../data/users.txt";

const int port = 3425;

bool check_user(FILE *file_users, struct User *user, struct User *user_in_file);

void handle_client(int client_socket, FILE *file_users);

void *handle_client_thread(void *arg);

int main(int argc, char *argv[]) {
    FILE *file_users = fopen(file_name_user, "r");

    if (file_users == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int sock, listener;
    struct sockaddr_in addr;
    struct User user, user_in_file;
    bool message = false;
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(2);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(3);
    }

    listen(listener, 5);
    if (strcmp(argv[0],"1")) {
        while (1) {
            int client_socket = accept(listener, NULL, NULL);
            if (client_socket < 0) {
                perror("Ошибка при принятии соединения");
                continue;
            }

            pid_t pid = fork();

            if (pid == 0) {
                close(listener);
                handle_client(client_socket, file_users);
                fclose(file_users);
                exit(0);
            } else if (pid > 0) {
                close(client_socket);
            } else {
                perror("Ошибка при создании дочернего процесса");
                exit(4);
            }
        }
    }
    if (strcmp(argv[0],"2")==0) {
        while (1) {
            int *client_socket = malloc(sizeof(int));
            *client_socket = accept(listener, NULL, NULL);
            if (*client_socket < 0) {
                perror("Ошибка при принятии соединения");
                free(client_socket);
                continue;  // Перейти к следующей итерации цикла
            }

            pthread_t tid;
            if (pthread_create(&tid, NULL, handle_client_thread, (void *)client_socket) != 0) {
                perror("Ошибка при создании потока");
                close(*client_socket);
                free(client_socket);
                continue;
            }

            pthread_detach(tid);
        }

    }

    fclose(file_users);
    return 0;
}

void handle_client(int client_socket, FILE *file_users) {
    struct User user, user_in_file;
    bool authenticated = false;
    int bytes_read;

    bytes_read = recv(client_socket, &user, sizeof(user), 0);
    if (bytes_read <= 0)
        exit(3);

    authenticated = check_user(file_users, &user, &user_in_file);
    send(client_socket, &authenticated, sizeof(authenticated), 0);

    if (authenticated) {
        int command;

        while (1) {
            bytes_read = recv(client_socket, &command, sizeof(command), 0);
            if (bytes_read <= 0)
                break;
            if (command == 1) {
                char message[] = "Введите x,y,z:";
                send(client_socket, message, sizeof(message), 0);
                double coord[3] = {0};
                bytes_read = recv(client_socket, coord, sizeof(coord), 0);
                if (bytes_read <= 0)
                    break;
                struct data *result = task1(coord[0], coord[1], coord[2]);
                send(client_socket, result, sizeof(struct data) * 10, 0);
                free(result);
            } else if (command == 2) {
                struct data *result = task2();
                send(client_socket, result, sizeof(struct data) * 10, 0);
                free(result);
            } else {
                exit(5);
            }

        }
    }

    close(client_socket);
}

void *handle_client_thread(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);

    struct User user, user_in_file;
    bool authenticated = false;
    int bytes_read;

    bytes_read = recv(client_socket, &user, sizeof(user), 0);
    if (bytes_read <= 0) {
        close(client_socket);
        pthread_exit(NULL);
    }

    FILE *file_users = fopen(file_name_user, "r");
    if (file_users == NULL) {
        perror("Ошибка при открытии файла");
        close(client_socket);
        pthread_exit(NULL);
    }

    authenticated = check_user(file_users, &user, &user_in_file);
    send(client_socket, &authenticated, sizeof(authenticated), 0);

    if (authenticated) {
        int command;

        while (1) {
            bytes_read = recv(client_socket, &command, sizeof(command), 0);
            if (bytes_read <= 0)
                break;
            if (command == 1) {
                char message[] = "Введите x,y,z:";
                send(client_socket, message, sizeof(message), 0);
                double coord[3] = {0};
                bytes_read = recv(client_socket, coord, sizeof(coord), 0);
                if (bytes_read <= 0)
                    break;
                struct data *result = task1(coord[0], coord[1], coord[2]);
                send(client_socket, result, sizeof(struct data) * 10, 0);
                free(result);
            } else if (command == 2) {
                struct data *result = task2();
                send(client_socket, result, sizeof(struct data) * 10, 0);
                free(result);
            } else {
                break;
            }
        }
    }

    fclose(file_users);
    close(client_socket);
    pthread_exit(NULL);
}

bool check_user(FILE *file_users, struct User *user, struct User *user_in_file) {
    bool message = false;

    fseek(file_users, 0, SEEK_SET);

    while (fscanf(file_users, "%s %s", user_in_file->username, user_in_file->password) == 2) {
        if (strcmp(user_in_file->password, user->password) == 0 &&
            strcmp(user_in_file->username, user->username) == 0) {
            message = true;
            break;
        }
    }
    return message;
}