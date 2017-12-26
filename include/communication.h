#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdlib.h>

extern int recv_msg(int remote, void *data, size_t size);
extern int send_msg(int remote, void *data, size_t size);

#endif /* COMMUNICATION_H */
