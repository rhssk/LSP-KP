#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

extern int valid_ip(const char *address);
extern int valid_port(const char *port_str);
extern void get_remote_ip_port(int sock, char *ipstr, int *port);
extern int recv_msg(const int remote, void *data, const size_t size);
extern int send_msg(const int remote, void *data, const size_t size);

#endif /* COMMON_H */
