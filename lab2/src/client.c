#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const int port= 3427;

char message[] = "Hello there!\n";
char buf[sizeof(message)];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    while (1) {
        if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("connect");
            exit(2);
        }
        send(sock, message, sizeof(message), 0);
        recv(sock, buf, sizeof(message), 0);

        printf("%s\n", buf);
    }
    close(sock);

    return 0;
}