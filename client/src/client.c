#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug_macros.h"
#include "constants.h"
#include "common.h"
#include "game.h"
#include "client.h"

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

    debug("Connection with server established");
    if (talk_to_server(serv_sock) == -1) {
        close(serv_sock);
        goto error;
    }
error:
    return;
}
