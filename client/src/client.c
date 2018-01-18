#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug_macros.h"
#include "constants.h"
#include "common.h"
#include "client.h"

#define BUFFER_SIZE 255

void init_client(const char *address, const char *port)
{
    int serv_sock, status;
    struct addrinfo hints, *res, *p = NULL;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* Don't care about IP version */
    hints.ai_socktype = SOCK_STREAM; /* TCP stream socket */

    status = getaddrinfo(address, port, &hints, &res);
    check(status  == 0,
          "%s", gai_strerror(status));

    /* Find at least one working socket */
    for (p = res; p != NULL; p = p->ai_next) {
        serv_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (serv_sock == -1) {
            continue;
        }

        if (connect(serv_sock, res->ai_addr, res->ai_addrlen) == -1) {
            close(serv_sock);
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    check(p != NULL,
          "Failed to get any usable sockets");

    debug("Connction with server established");
    if (talk_to_server(serv_sock) == -1) {
        close(serv_sock);
        goto error;
    }
error:
    return;
}

int talk_to_server(int serv_sock)
{
    char *msg = malloc(BUFFER_SIZE);

    while (1) {
        join_request_t join_request;
        join_request.packet_id = P_JOIN_REQUEST;

        memset(msg, '\0', BUFFER_SIZE); // Clear msg
        memcpy(msg, &join_request, sizeof(join_request));
        if (send_msg(serv_sock, msg, BUFFER_SIZE) == -1) goto error;

        memset(msg, '\0', BUFFER_SIZE);
        if (recv_msg(serv_sock, msg, BUFFER_SIZE) == -1) goto error;
    }

error:
    free(msg);
    return -1;
}
