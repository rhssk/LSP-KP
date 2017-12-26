#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "debug_macros.h"
#include "packets.h"
#include "server.h"

void server_mode(const unsigned long port)
{
    int sock, client;

    sock = create_sock(port);
    if (sock == -1) goto error;

    client = wait_for_client(sock);
    if (client == -1) goto error;

    check(recv_msg(client) != -1, "Failed to read from client");
    check(send_msg(client) != -1, "Failed to send to client");

    close(client);
    close(sock);
error:
    return;
}

int create_sock(unsigned long port)
{
    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    check(sock >= 0, "Couldn't open socket");
    int enable_addr_reuse = 1;
    check(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                     &enable_addr_reuse, sizeof(int)) >= 0,
          "Failed to set SO_REUSEADDR for socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    check(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0,
          "Couldn't bind the socket");

    return sock;
error:
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
