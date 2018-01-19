#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

enum return_codes {
    C_OK = 0,
    C_DATA = -1,
    C_DISCONNECT = -2,
    C_TIMEOUT = -3,
    C_POLL = -4
};

extern int valid_ip(const char *address);
extern int valid_port(const char *port_str);
extern void get_remote_ip_port(int sock, char *ipstr, int *port);
extern int recv_msg(const int remote, void *data, const size_t size,
                    const int timeout);
extern int send_msg(const int remote, void *data, const size_t size,
                    const int timeout);

#endif /* COMMON_H */
