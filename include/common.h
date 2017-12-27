#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

extern int recv_msg(int remote, void *data, size_t size);
extern int send_msg(int remote, void *data, size_t size);
extern void get_remote_ip_port(int sock, char *ipstr, int *port);

#endif /* COMMON_H */
