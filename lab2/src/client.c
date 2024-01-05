#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

const int port = 3427;

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

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    struct User user;

    printf("Введите имя: ");
    scanf("%19s", user.username);

    printf("Введите пароль: ");
    scanf("%19s", user.password);

    send(sock, &user, sizeof(user), 0);

    bool message=false;
    recv(sock, &message, sizeof(message), 0);

    printf("%d\n",message);

    close(sock);

    return 0;
}
