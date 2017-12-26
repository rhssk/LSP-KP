#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "debug_macros.h"
#include "packets.h"
#include "server.h"

static jmp_buf env;

void sig_handler(int signal)
{
    if (signal == SIGINT)
        longjmp(env, 1);
}

void server_mode(const unsigned long port)
{
    int sock, client;
    struct sigaction sa;

    /* Allow program to clean-up when user wants to quit */
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    check(sigaction(SIGINT, &sa, NULL) != -1, "Failed to handle SIGINT");

    sock = create_sock(port);
    if (sock == -1) goto error;

    while (1) {
        /* Catch termination by user */
        if (setjmp(env) == 1) {
            log_info("Terminated by user, cleaning up...");
            goto error;
        }

        client = wait_for_client(sock);
        if (client == -1) {
            close(sock);
            goto error;
        }

        check(recv_msg(client) != -1, "Failed to read from client");
        check(send_msg(client) != -1, "Failed to send to client");

        close(client);
        close(sock);
    }
error:
    /* close(client); */
    /* close(sock); */
    return;
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
