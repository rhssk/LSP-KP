#ifndef SERVER_H
#define SERVER_H

extern void server_mode(const unsigned long port);

static int create_sock(unsigned long port);
void handle_clients(int sock);
static int wait_for_client(int sock);

#endif /* SERVER_H */
