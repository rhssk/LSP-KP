#ifndef MAIN_H
#define MAIN_H

static void print_help(void);
static void address(const char *address, const unsigned long port);
static int valid_ip(char *address);
static int valid_port(const char *port_str);

#endif /* MAIN_H */
