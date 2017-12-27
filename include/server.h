#ifndef SERVER_H
#define SERVER_H

typedef struct worker_args {
    int socket;
} worker_args_t;

extern void server_mode(char *port);

static int server_thread(unsigned long port);
static void *accept_clients(void *args);
static int wait_for_client(int serv_sock);
static void *service_client(void *args);

#endif /* SERVER_H */
