#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct join_request {
    unsigned char id;
    char msg[23]; // Without ending \0
} join_request_t;

typedef struct join_response {
    unsigned char id;
    unsigned char code;
    unsigned char player_id;
} join_response_t;

void print_help(void);
void address(const char *address, const unsigned long port);
bool valid_ip(char *address);
bool valid_port(char *port_str, unsigned long *port);

#endif /* MAIN_H */
