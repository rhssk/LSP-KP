#ifndef SERVER_H
#define SERVER_H

extern void server_mode(const unsigned long port);

static int create_sock(unsigned long port);
static int wait_for_client(int sock);
static int recv_msg(int client);
static int send_msg(int client);

#endif /* SERVER_H */
