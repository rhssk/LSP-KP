#ifndef MAIN_H
#define MAIN_H

void print_help(void);
void address(const char *address, const unsigned long port);
int valid_ip(char *address);
int valid_port(char *port_str, unsigned long *port);

#endif /* MAIN_H */
