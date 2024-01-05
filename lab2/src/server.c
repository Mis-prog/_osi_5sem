#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

struct User {
    char username[20];
    char password[20];
};

const char file_name_user[] = "users.txt";

const int port = 3427;

int main() {
    FILE *file_users = fopen(file_name_user, "r");

    int sock, listener;
    struct sockaddr_in addr;
    struct User user, user_in_file;
    bool message = false;
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    while (1) {
        sock = accept(listener, NULL, NULL);
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        while (1) {
            bytes_read = recv(sock, &user, sizeof(user), 0);
            if (bytes_read <= 0)
                break;

            while (fscanf(file_users, "%s %s", user_in_file.username, user_in_file.password) == 2) {
                if (strcmp(user_in_file.password, user.password) == 0 &&
                    strcmp(user_in_file.username, user.username) == 0) {
                    message = true;
                    break;
                }
            }
            send(sock, &message, sizeof(message), 0);
            message = false;
        }

        close(sock);
    }
    fclose(file_users);
    return 0;
}
