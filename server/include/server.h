#ifndef MAIN_H
#define MAIN_H

typedef struct worker_args {
    int socket;
} worker_args_t;

void init_server(const char *port);
void *accept_clients(void *args);
int wait_for_client(int serv_sock);
void *service_client(void *args);

#endif /* MAIN_H */
