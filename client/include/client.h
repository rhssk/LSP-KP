#ifndef CLIENT_H
#define CLIENT_H

extern void init_client(const char *address, const char *port);
static int talk_to_server(int serv_sock);

#endif /* CLIENT_H */
