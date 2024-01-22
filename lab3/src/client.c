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
    struct data data_message[10];
    char string_message[100];
    double coord_message[3];
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
        printf("Enter command number (1 list of commands): ");
        scanf("%d", &message.command);

        sendto(socket_descriptor, &message, sizeof(message), 0,
               (struct sockaddr *) &server_addr, sizeof(server_addr));

        switch (message.command) {
            case 0: // LOGIN
                printf("Input username:");
                scanf("%19s", user.username);
                printf("Input password");
                scanf("%19s", user.password);

                uuid_generate(user.uuid);

                if (!authenticated) {
                    sendto(socket_descriptor, &user, sizeof(user), 0,
                           (struct sockaddr *) &server_addr, sizeof(server_addr));

                    bytes_received = recvfrom(socket_descriptor, &authenticated, sizeof(authenticated), 0,
                                              (struct sockaddr *) &server_addr, &server_addr_len);
                    if (authenticated) {
                        printf("Successful account login\n");
                    } else {
                        printf("try again\n");
                    }
                } else {
                    bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                              (struct sockaddr *) &server_addr,
                                              &server_addr_len);
                    if (bytes_received < 0) { exit(EXIT_FAILURE); }
                    printf("%s\n", message.string_message);
                }
                break;
            case 1: // LIST
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                printf("List of commands:\n");
                printf("%s\n", message.string_message);
                break;
            case 2: // GET_TASK1

                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                printf("%s\n", message.string_message);

                if (strcmp(message.string_message, "No permissions for GET_TASK1 command") == 0) { break; }

                scanf("%lf %lf %lf", &message.coord_message[0], &message.coord_message[1], &message.coord_message[2]);
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
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                if (bytes_received < 0) {
                    exit(EXIT_FAILURE);
                }
                if (strcmp(message.string_message, "No permissions for GET_TASK2 command") == 0) {
                    printf("%s\n", message.string_message);
                    break;
                }
                display_task2_results(message.data_message);
                break;
            case 4: // LOGOUT
                bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                          (struct sockaddr *) &server_addr, &server_addr_len);
                printf("%s\n", message.string_message);
                close(socket_descriptor);
                exit(EXIT_SUCCESS);
                break;
            default:
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
    printf("Results:\n");
    int i = 0;
    while (result[i].n_point != -1) {
        printf("n_point: %d, temp: %f, dx: %.9f\n", result[i].n_point, result[i].temp, result[i].dx);
        i++;
    }
}

void display_task2_results(struct data *result) {
    printf("Results\n");
    for (int i = 0; i < 7; i++) {
        printf("time: %f, term_mean: %f, dx_mean: %.9f\n", result[i].time, result[i].temp_mean, result[i].dx_mean);
    }
}

