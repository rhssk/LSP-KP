#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "debug_macros.h"
#include "packets.h"
#include "common.h"
#include "server.h"

#define MAX_CONN_QUEUE 5
#define BUFFER_SIZE 255

void server_mode(const char *port)
{
    pthread_t server_thread;

    check(pthread_create(&server_thread, NULL, accept_clients, (char *)port) == 0,
          "Failed to create a new thread to accept connections");

    pthread_join(server_thread, NULL);
    pthread_exit(NULL);
error:
    return;
}

void *accept_clients(void *args)
{
    int serv_sock, status;
    struct addrinfo hints, *res, *p = NULL;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* Don't care about IP version */
    hints.ai_socktype = SOCK_STREAM; /* TCP stream socket */
    hints.ai_flags = AI_PASSIVE; /* Use local host IP */

    status = getaddrinfo(NULL, args, &hints, &res);
    check(status  == 0,
          "%s", gai_strerror(status));

    /* Find at least one working socket */
    for (p = res; p != NULL; p = p->ai_next) {
        serv_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (serv_sock == -1) {
            log_err("Failed to open socket");
            continue;
        }

        int reuse_addr = 1;
        if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR,
                       &reuse_addr, sizeof(int)) == -1) {
            log_err("Failed set address for socket as reusable");
            close(serv_sock);
            continue;
        }

        if (bind(serv_sock, p->ai_addr, p->ai_addrlen) == -1) {
            log_err("Failed to bind the socket to a port");
            close(serv_sock);
            continue;
        }

        if (listen(serv_sock, MAX_CONN_QUEUE) == -1) {
            log_err("Failed to listen for connections on the socket");
            close(serv_sock);
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    check(p != NULL,
          "Failed to get any usable sockets");

    debug("Waiting for incoming connections");
    if (wait_for_client(serv_sock) == -1) {
        close(serv_sock);
        pthread_exit(NULL);
    }

error:
    pthread_exit(NULL);
}

int wait_for_client(int serv_sock)
{
    int client_sock;
    struct sockaddr_storage *client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    worker_args_t *wa;
    pthread_t worker_thread;
    char ipstr[INET6_ADDRSTRLEN];
    int port;

    while (1) {
        client_addr = malloc(sin_size);
        client_sock = accept(serv_sock, (struct sockaddr *) client_addr, &sin_size);
        if (client_sock == -1) {
            free(client_addr);
            log_err("Failed to accept connection from client");
            continue;
        }

        get_remote_ip_port(client_sock, ipstr, &port);
        log_info("Remote(%s:%d) has connected", ipstr, port);

        wa = malloc(sizeof(worker_args_t));
        wa->socket = client_sock;

        if (pthread_create(&worker_thread, NULL, service_client, wa) != 0)
            goto error;
    }

error:
    log_err("Failed to create a worker thread");
    free(wa);
    free(client_addr);
    close(client_sock);
    close(serv_sock);
    pthread_exit(NULL);
    return -1;
}

void *service_client(void *args)
{
    int sock;
    char *msg = NULL;
    worker_args_t *wa;

    wa = (worker_args_t *) args;
    sock = wa->socket;

    /* Detach current thread so noone has to wait for it when client quits */
    check(pthread_detach(pthread_self()) == 0,
          "Failed to detach current thread");

    msg = malloc(BUFFER_SIZE);
    while (1) {
        if (recv_msg(sock, msg, BUFFER_SIZE) == -1) goto error;
        debug("SENT MSG: %s", msg);

        strcpy(msg, "SERVER ANSWER");
        if (send_msg(sock, msg, BUFFER_SIZE) == -1) goto error;
        debug("RECEIVED MSG: %s", msg);
    }


error:
    if (!msg) free(msg);
    pthread_exit(NULL);
}
