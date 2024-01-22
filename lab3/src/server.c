#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <uuid/uuid.h>
#include "src_lib/calculation.h"

#define PORT_NUMBER 3425
#define LENGTH 20
#define MAX_DATA_POINTS 10;

struct Key_Command {
    char *command;
    int key;
};

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


struct Key_Command command_list[5];

const char FILE_NAME_USER[] = "../data/users.txt";
const char FILE_NAME_LOG[] = "../bin/log.txt";

void set_command();

char *get_command();

bool check_user(struct User *user);

void write_log(struct User user, const char *user_command, int server_responsive);

void display_task1_results(struct data *result);

void display_task2_results(struct data *result);

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

    if (bind(socket_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(socket_descriptor);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server is running on port %d...\n", PORT_NUMBER);

    set_command();
    struct Message message;

    int command;
    bool authenticated = false;

    struct User user;
    uuid_generate(user.uuid);


    if (!authenticated) { uuid_generate(user.uuid); }

    while (true) {

        int bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                      (struct sockaddr *) &client_addr, &client_addr_len);
        if (bytes_received < 0) {
            perror("recvfrom");
            write_log(user, "Fail input", 530);
            close(socket_descriptor);
            exit(EXIT_FAILURE);
        }

        switch (message.command) {

            case 0: //LOGIN
                if (!authenticated) {
                    bytes_received = recvfrom(socket_descriptor, &user, sizeof(user), 0,
                                              (struct sockaddr *) &client_addr, &client_addr_len);
                    if (bytes_received < 0) {
                        perror("recvfrom");
                        close(socket_descriptor);
                        exit(EXIT_FAILURE);
                    }

                    authenticated = check_user(&user);
                    sendto(socket_descriptor, &authenticated, sizeof(authenticated), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "LOGIN", 200);
                } else {
                    strncpy(message.string_message, "LOGIN again", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "LOGIN again", 400);
                }
                break;
            case 1: //LIST
                if (authenticated) {
                    char *list_command = get_command();
                    strncpy(message.string_message, list_command, sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "LIST", 200);
                } else {
                    strncpy(message.string_message, "No permissions for LIST command", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0, (struct sockaddr *) &client_addr,
                           client_addr_len);
                    write_log(user, "No permissions for LIST command", 403);
                }
                break;
            case 2: //GET_TASK1
                if (authenticated) {
                    strncpy(message.string_message, "Input x,y,z", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);

                    bytes_received = recvfrom(socket_descriptor, &message, sizeof(message), 0,
                                              (struct sockaddr *) &client_addr, &client_addr_len);
                    if (bytes_received < 0) {
                        write_log(user, "Fail input", 500);
                        exit(EXIT_FAILURE);
                    }

                    struct data *result = task1(message.coord_message[0], message.coord_message[1],
                                                message.coord_message[2]);
                    memcpy(message.data_message, result, sizeof(struct data) * 10);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "GET_TASK1", 200);
                    free(result);

                } else {
                    strncpy(message.string_message, "No permissions for GET_TASK1 command", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0, (struct sockaddr *) &client_addr,
                           client_addr_len);
                    write_log(user, "No permissions for GET_TAKS1 command", 403);
                }

                break;
            case 3: //GET_TASK2
                if (authenticated) {
                    struct data *result = task2();
                    memcpy(message.data_message, result, sizeof(struct data) * 10);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "GET_TASK2", 200);
                    free(result);

                } else {
                    strncpy(message.string_message, "No permissions for GET_TASK2 command", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0, (struct sockaddr *) &client_addr,
                           client_addr_len);
                    write_log(user, "No permissions for GET_TASK2 command", 403);
                }
                break;
            case 4: //LOGOUT
                if (authenticated) {
                    authenticated = false;

                    strncpy(message.string_message, "Exit of account", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "LOGOUT", 200);
                } else {
                    strncpy(message.string_message, "LOGOUT again", sizeof(message.string_message) - 1);
                    sendto(socket_descriptor, &message, sizeof(message), 0,
                           (struct sockaddr *) &client_addr, client_addr_len);
                    write_log(user, "LOGOUT again", 400);
                }

                uuid_generate(user.uuid);
                break;
            default:
                strncpy(message.string_message, "Request not  supported", sizeof(message.string_message) - 1);
                sendto(socket_descriptor, &message, sizeof(message), 0,
                       (struct sockaddr *) &client_addr, client_addr_len);
                write_log(user, "Request not  supported", 501);

        }
    }

    close(socket_descriptor);

    return 0;
}

void set_command() {
    command_list[0].command = "LOGIN";
    command_list[0].key = 0;

    command_list[1].command = "LIST";
    command_list[1].key = 1;

    command_list[2].command = "GET_TASK1";
    command_list[2].key = 2;

    command_list[3].command = "GET_TASK2";
    command_list[3].key = 3;

    command_list[4].command = "LOGOUT";
    command_list[4].key = 4;
}

char *get_command() {
    static char result[100];

    result[0] = '\0';

    for (int i = 0; i < 5; i++) {
        char temp[20];
        sprintf(temp, "%d:%s ", command_list[i].key, command_list[i].command);
        strcat(result, temp);
    }

    return result;
}

bool check_user(struct User *user) {
    FILE *file_users = fopen(FILE_NAME_USER, "r");

    bool authenticated = false;
    char username[20], password[20];

    while (fscanf(file_users, "%s %s", username, password) == 2) {
        if (strcmp(password, user->password) == 0 &&
            strcmp(username, user->username) == 0) {
            authenticated = true;
            break;
        }
    }

    fclose(file_users);

    return authenticated;
}

void write_log(struct User user, const char *user_command, int server_responsive) {
    FILE *file = fopen(FILE_NAME_LOG, "a");

    char uuid_str[37];
    uuid_unparse(user.uuid, uuid_str);

    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);

    fprintf(file, "time: %d:%d:%d User: %s %s, Server response: %d\n", localTime->tm_hour, localTime->tm_min,
            localTime->tm_sec, uuid_str, user_command, server_responsive);

    fclose(file);
}
