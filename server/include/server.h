#ifndef MAIN_H
#define MAIN_H

typedef struct worker_args {
    int socket;
} worker_args_t;

extern void init_server(const char *port);
static void *accept_clients(void *args);
static int wait_for_client(int serv_sock);
static void *service_client(void *args);

#endif /* MAIN_H */
