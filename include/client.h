#ifndef CLIENT_H
#define CLIENT_H

extern void client_mode(const char *address, const unsigned long port);

static int recv_msg(int server);
static int send_msg(int server);

#endif /* CLIENT_H */
