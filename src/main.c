#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "debug_macros.h"
#include "main.h"

void print_help()
{
    printf("This is help!\n");
}

bool valid_ip(char *address)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, address, &(sa.sin_addr));
    return result == 1;
}

bool valid_port(char *port_str, unsigned long *port)
{
    char *endptr;

    // Check for negative numbers
    check(strchr(port_str, '-') == NULL,
          "Port number should be positive");

    *port = strtoul(port_str, &endptr, 10);

    // Check for non-integer numbers
    check(*endptr == '\0',
          "Port number should be a natural number");

    // Check for range
    check(*port >= 2000 && *port <= 65535,
          "Port number should be between 2000 and 65535");

    return true;

error:
    return false;
}

int create_sock(unsigned long port)
{
    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    check(sock >= 0, "Couldn't open socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    check(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0,
          "Couldn't bind the socket");

    return sock;
error:
    close(sock);
    return -1;
}

int wait_for_client(int sock)
{
    int client;
    socklen_t clilen;
    struct sockaddr_in cli_addr;

    listen(sock, 5);
    clilen = sizeof(cli_addr);
    client = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
    check(client >= 0, "Failed to accept");

    return client;
error:
    close(sock);
    close(client);
    return -1;
}

int recv_msg(int client)
{
    char buffer[255];

    check(read(client, buffer, sizeof(buffer)) >= 0,
          "Failed to read from socket");

    debug("FROM CLIENT: %d RECEIVE: %s", client, buffer);

    return 0;
error:
    return -1;
}

int send_msg(int client)
{
    char buffer[255] = "JOIN RESPONSE";

    check(write(client, buffer, sizeof(buffer)) >= 0,
          "Failed to write to socket");

    debug("TO CLIENT: %d SEND: %s", client, buffer);

    return 0;
error:
    return -1;
}

int main(int argc, char **argv)
{
    int sock, client;
    unsigned long port = 0;

    if (argc == 1) {
        print_help();
        sentinel("Too few arguments");
    }

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h")  == 0 ||
            strcmp(argv[i], "--help") == 0) {
            print_help();
        } else if (strcmp(argv[i], "-p")  == 0 ||
                   strcmp(argv[i], "--port") == 0) {
            check(i < argc - 1,
                  "A port number is required");
            if (!valid_port(argv[++i], &port)) goto error;
        } else {
            print_help();
            sentinel("Unrecognized arguments");
        }
    }

    sock = create_sock(port);
    if (sock == -1) goto error;

    client = wait_for_client(sock);
    if (client == -1) {
        close(sock);
        goto error;
    }

    check(recv_msg(client) != -1, "Failed to read from client");
    check(send_msg(client) != -1, "Failed to send to client");

    close(client);
    close(sock);

    return 0;

error:
    return 1;
}
