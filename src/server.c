#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "debug_macros.h"
#include "packets.h"
#include "communication.h"
#include "server.h"

#define MAX_CONN_QUEUE 5
#define BUFFER_SIZE 255

void server_mode(const unsigned long port)
{
    int sock, client;

    sock = create_sock(port);
    if (sock == -1) goto error;

    handle_clients(sock);
error:
    close(sock);
    return;
}

void handle_clients(int sock)
{
    int client;

    client = wait_for_client(sock);
    if (client == -1) return;

    char *msg = malloc(BUFFER_SIZE);
    while (1) {
        if (recv_msg(client, msg, BUFFER_SIZE) == -1) goto error;
        debug("RECEIVED MESSAGE: %s", msg);

        strcpy(msg, "SERVER ANSWER");
        if (send_msg(client, msg, BUFFER_SIZE) == -1) goto error;
    }
error:
    /* Client is in a failed state.
     * Clean up and start listening for new clients
     */
    close(client);
    free(msg);
    handle_clients(sock);
}

int create_sock(unsigned long port)
{
    int sock;
    struct sockaddr_in serv_addr;

    /* Create new TCP socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    check(sock >= 0,
          "Couldn't open socket");
    int enable_addr_reuse = 1;
    check(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                     &enable_addr_reuse, sizeof(int)) >= 0,
          "Failed to reuse address for socket");
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

    check(listen(sock, MAX_CONN_QUEUE) == 0,
          "Failed to mark socket to accept incoming connection requests");
    clilen = sizeof(cli_addr);
    client = accept(sock, (struct sockaddr *)&cli_addr, &clilen);
    check(client >= 0,
          "Failed to accept");

    return client;
error:
    return -1;
}
