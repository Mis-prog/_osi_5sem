#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "src_lib/calculation.h"

#define PORT_NUMBER 3425
#define LENGTH 20


struct User {
    char username[LENGTH];
    char password[LENGTH];
    int id;
};

struct Message {
    struct User user;
    char command[LENGTH];
    double data[3];
};

const char FILE_NAME_USER[] = "../data/users.txt";
const char FILE_NAME_LOG[] = "../bin/log.txt";

// Function prototypes
bool check_user(FILE *file_users, struct Message *user, struct User *user_in_file);
void handle_client(int socket, struct sockaddr_in client_addr, socklen_t client_addr_len,
                   FILE *file_users, FILE *file_log);

int main() {
    int socket_descriptor;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descriptor < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_descriptor, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server is running on port %d...\n", PORT_NUMBER);

    FILE *file_users = fopen(FILE_NAME_USER, "r");
    FILE *file_log = fopen(FILE_NAME_LOG, "a");

    if (file_users == NULL || file_log == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        handle_client(socket_descriptor, client_addr, client_addr_len, file_users, file_log);
    }

    fclose(file_users);
    fclose(file_log);

    return 0;
}

void handle_client(int socket_descriptor, struct sockaddr_in client_addr, socklen_t client_addr_len,
                   FILE *file_users, FILE *file_log) {
    time_t current_time;
    struct tm *local_time;

    struct Message message_user;
    struct User user_in_file;
    bool authenticated = false;
    int bytes_received;

    bytes_received = recvfrom(socket_descriptor, &message_user, sizeof(message_user), 0,
                              (struct sockaddr *)&client_addr, &client_addr_len);

    authenticated = check_user(file_users, &message_user, &user_in_file);
    sendto(socket_descriptor, &authenticated, sizeof(authenticated), 0,
           (struct sockaddr *)&client_addr, client_addr_len);

    if (authenticated) {
        int command;
        current_time = time(NULL);
        local_time = localtime(&current_time);

        if (fprintf(file_log, "%02d:%02d:%02d авторизация %d\n",
                    local_time->tm_hour, local_time->tm_min, local_time->tm_sec, authenticated) < 0) {
            perror("Error writing to log file");
        }

        while (1) {
            bytes_received = recvfrom(socket_descriptor, &command, sizeof(command), 0,
                                      (struct sockaddr *)&client_addr, &client_addr_len);

            if (bytes_received <= 0) {
                break;
            }

            if (command == 1) {
                char message[] = "Введите x, y, z:";
                sendto(socket_descriptor, message, sizeof(message), 0,
                       (struct sockaddr *)&client_addr, client_addr_len);

                double coord[3] = {0};
                bytes_received = recvfrom(socket_descriptor, coord, sizeof(coord), 0,
                                          (struct sockaddr *)&client_addr, &client_addr_len);

                if (bytes_received <= 0) {
                    break;
                }

                struct data *result = task1(coord[0], coord[1], coord[2]);
                sendto(socket_descriptor, result, sizeof(struct data) * 10, 0,
                       (struct sockaddr *)&client_addr, client_addr_len);
                free(result);
            } else if (command == 2) {
                struct data *result = task2();
                sendto(socket_descriptor, result, sizeof(struct data) * 10, 0,
                       (struct sockaddr *)&client_addr, client_addr_len);
                free(result);
            } else {
                exit(EXIT_FAILURE);
            }
        }
    } else {
        current_time = time(NULL);
        local_time = localtime(&current_time);

        if (fprintf(file_log, "%02d:%02d:%02d авторизация %d\n",
                    local_time->tm_hour, local_time->tm_min, local_time->tm_sec, authenticated) < 0) {
            perror("Error writing to log file");
        }
    }
}

bool check_user(FILE *file_users, struct Message *user, struct User *user_in_file) {
    bool authenticated = false;
    fseek(file_users, 0, SEEK_SET);

    while (fscanf(file_users, "%s %s", user_in_file->username, user_in_file->password) == 2) {
        if (strcmp(user_in_file->password, user->user.password) == 0 &&
            strcmp(user_in_file->username, user->user.username) == 0) {
            authenticated = true;
            break;
        }
    }

    return authenticated;
}
