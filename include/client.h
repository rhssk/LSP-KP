#ifndef CLIENT_H
#define CLIENT_H

extern void client_mode(const char *address, const char *port);
static int talk_to_server(int serv_sock);

#endif /* CLIENT_H */
