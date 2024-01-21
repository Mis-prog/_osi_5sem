#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "uuid/uuid.h"
#include "src_lib/calculation.h"

#define PORT_NUMBER 3425
#define LENGTH 20
#define MAX_DATA_POINTS 10

struct User {
    char username[20];
    char password[20];
    uuid_t uuid;
};

struct Message {
    int command;
    struct data *data_message;
    char *string_message;
};

void display_task1_results(struct data result[MAX_DATA_POINTS]);

void display_task2_results(struct data result[MAX_DATA_POINTS]);

int main() {
    int socket_descriptor;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);

    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_descriptor < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    struct User user;
    struct Message message;

    bool authenticated;
    while (true) {
        int bytes_received;
        printf("Введите номер команды: ");
        scanf("%d", &message.command);

        sendto(socket_descriptor, &message, sizeof(message), 0,
               (struct sockaddr *) &server_addr, sizeof(server_addr));

        switch (message.command) {
            case 0: // LOGIN
                printf("Введите имя\n");
                scanf("%19s", user.username);
                printf("Введите пароль\n");
                scanf("%19s", user.password);

                uuid_generate(user.uuid);

                sendto(socket_descriptor, &user, sizeof(user), 0,
                       (struct sockaddr *) &server_addr, sizeof(server_addr));

                bytes_received = recvfrom(socket_descriptor, &authenticated, sizeof(authenticated), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (authenticated) {
                    printf("Успешная авторизация\n");
                } else {
                    printf("Неуспешная авторизация\n");
                }
                break;
            case 1: // LIST
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                printf("Список команд:\n");
                printf("%s\n", message.string_message);
                break;
            case 2: // GET_TASK1
                message.data_message = NULL;

                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                printf("%s\n", message.string_message);

                if (strcmp(message.string_message, "Нет доступа") == 0) { break; }

                scanf("%lf %lf %lf", &message.data_message->x, &message.data_message->y, &message.data_message->z);
                sendto(socket_descriptor, &message, sizeof(message), 0,
                       (struct sockaddr *) &server_addr, sizeof(server_addr));

                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                display_task1_results(message.data_message);
                break;
            case 3: // GET_TASK2
                message.data_message = NULL;
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                if (strcmp(message.string_message, "Нет доступа") == 0) {
                    printf("%s\n", message.string_message);
                    break;
                }
                display_task2_results(message.data_message);
                break;
            case 4: // LOGOUT
                message.string_message = NULL;
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                printf("%s\n", message.string_message);
                close(socket_descriptor);
                break;
            default:
                message.data_message = NULL;
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                printf("%s\n", message.string_message);
                break;
        }
    }

    close(socket_descriptor);

    return 0;
}

void display_task1_results(struct data *result) {
    printf("Результаты эксперемента\n");
    int i = 0;
    while (result[i].n_point != -1) {
        printf("n_point: %d, temp: %f, dx: %.9f\n", result[i].n_point, result[i].temp, result[i].dx);
        i++;
    }
}

void display_task2_results(struct data *result) {
    printf("Результаты эксперемента\n");
    for (int i = 0; i < 7; i++) {
        printf("time: %f, term_mean: %f, dx_mean: %.9f\n", result[i].time, result[i].temp_mean, result[i].dx_mean);
    }
}