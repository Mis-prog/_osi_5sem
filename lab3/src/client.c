#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "src_lib/calculation.h"

#define LENGTH 20
#define DEFAULT_PORT 3425
#define ID_MAX 100
#define MAX_DATA_POINTS 10

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

void set_user_info(struct Message *message_user);
void handle_server(int sock, struct sockaddr_in server_addr, socklen_t server_addr_len, int command);
void display_task1_results(struct data result[MAX_DATA_POINTS]);
void display_task2_results(struct data result[MAX_DATA_POINTS]);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int enter_port_addd = 0;
    printf("Enter port: ");
    scanf("%d", &enter_port_addd);
    getchar();
    if (enter_port_addd == 0) {
        enter_port_addd = DEFAULT_PORT;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(enter_port_addd);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    struct Message message_user;
    set_user_info(&message_user);

    sendto(sock, &message_user, sizeof(message_user), 0, (struct sockaddr*)&server_addr, server_addr_len);

    bool authentication_result = false;
    recvfrom(sock, &authentication_result, sizeof(authentication_result), 0, (struct sockaddr*)&server_addr, &server_addr_len);

    int command;
    double coord[3] = {0};

    if (authentication_result) {
        while (1) {
            printf("Enter task number (1, 2, or any other number to exit): ");
            scanf("%d", &command);
            getchar();

            if (command == 1 || command == 2) {
                handle_server(sock, server_addr, server_addr_len, command);
            } else {
                break;
            }
        }
    } else {
        printf("Authentication error\n");
    }

    close(sock);

    return 0;
}

void set_user_info(struct Message *message_user) {
    printf("Enter username: ");
    scanf("%19s", message_user->user.username);
    getchar();

    printf("Enter password: ");
    scanf("%19s", message_user->user.password);
    getchar();

    srand(time(NULL));
    message_user->user.id = rand() % ID_MAX;
}

void handle_server(int sock, struct sockaddr_in server_addr, socklen_t server_addr_len, int command) {
    sendto(sock, &command, sizeof(command), 0, (struct sockaddr*)&server_addr, server_addr_len);

    if (command == 1) {
        char message[30];
        recvfrom(sock, message, sizeof(message), 0, (struct sockaddr*)&server_addr, &server_addr_len);
        printf("%s\n", message);

        struct data result[MAX_DATA_POINTS];
        double coord[3] = {0};
        scanf("%lf %lf %lf", &coord[0], &coord[1], &coord[2]);
        sendto(sock, coord, sizeof(coord), 0, (struct sockaddr*)&server_addr, server_addr_len);
        recvfrom(sock, &result[0], sizeof(result), 0, (struct sockaddr*)&server_addr, &server_addr_len);

        display_task1_results(result);
    } else if (command == 2) {
        struct data result[MAX_DATA_POINTS];
        recvfrom(sock, &result[0], sizeof(result), 0, (struct sockaddr*)&server_addr, &server_addr_len);
        display_task2_results(result);
    }
}

void display_task1_results(struct data result[MAX_DATA_POINTS]) {
    int i = 0;
    while (result[i].n_point != -1) {
        printf("n_point: %d, temp: %f, dx: %.9f\n", result[i].n_point, result[i].temp, result[i].dx);
        i++;
    }
}

void display_task2_results(struct data result[MAX_DATA_POINTS]) {
    for (int i = 0; i < 7; i++) {
        printf("time: %f, term_mean: %f, dx_mean: %.9f\n", result[i].time, result[i].temp_mean, result[i].dx_mean);
    }
}
