<<<<<<< HEAD
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "src_lib/calculation.h"



struct User {
    char username[20];
    char password[20];
};

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    int default_port_addd = 3425;
    int enter_port_addd = 0;
    printf("Enter port ");
    scanf("%d", &enter_port_addd);
    if (enter_port_addd == 0){
        enter_port_addd = default_port_addd;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(enter_port_addd);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    struct User user;

    printf("Введите имя: ");
    scanf("%19s", user.username);

    printf("Введите пароль: ");
    scanf("%19s", user.password);

    send(sock, &user, sizeof(user), 0);

    bool message = false;
    recv(sock, &message, sizeof(message), 0);

    int command;
    double coord[3]={0};
    if (message) {
        while (1) {
            printf("Введите номер задания (1,2):");
            scanf("%d", &command);
            if (command == 1) {
                char message[30];
                send(sock, &command, sizeof(command), 0);
                recv(sock, message, sizeof(message), 0);
                printf("%s\n", message);
                double coord[3]={0};
                struct data result[10];
                scanf("%lf %lf %lf", &coord[0], &coord[1], &coord[2]);
                send(sock, coord, sizeof(coord), 0);
                recv(sock, &result[0], sizeof(result), 0);
                int i=0;
                while (result[i].n_point != -1) {
                    printf("n_point: %d, temp: %f, dx: %.9f\n", result[i].n_point,result[i].temp, result[i].dx);
                    i++;
                }
//                send(sock, &command, sizeof(command), 0);
            } else if (command == 2) {
                struct data result[10];
                send(sock, &command, sizeof(command), 0);
                recv(sock, &result[0], sizeof(result), 0);
                for (int i = 0; i < 7; i++) {
                    printf("time: %f,term_mean: %f,dx_mean: %.9f\n",result[i].time,result[i].temp_mean,result[i].dx_mean);
                }
            }
            if (command == 0) {
                break;
            }
        }
    } else {
        printf("Ошибка с аунтефикацией\n");
    }

    close(sock);
=======
#include <stdio.h>

int main(){
>>>>>>> origin/lab3

    return 0;
}