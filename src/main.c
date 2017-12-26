#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "debug_macros.h"
#include "server.h"
#include "client.h"
#include "main.h"

void print_help(void)
{
    printf("This is help!\n");
}

int valid_ip(char *address)
{
    struct sockaddr_in sa;

    return inet_pton(AF_INET, address, &(sa.sin_addr)) == 1;
}

int valid_port(char *port_str, unsigned long *port)
{
    char *endptr;

    /* Check for negative numbers */
    check(strchr(port_str, '-') == NULL,
          "Port number should be positive");

    *port = strtoul(port_str, &endptr, 10);

    /* Check for non-integer numbers */
    check(*endptr == '\0',
          "Port number should be a natural number");

    /* Check for range */
    check(*port >= 2000 && *port <= 65535,
          "Port number should be between 2000 and 65535");

    return 1;
error:
    return 0;
}

int main(int argc, char **argv)
{
    /* Choose operating mode. 0 = client, 1 = server, -1 = unrecognized */
    int mode = -1;
    /* Can hold only IPv4 addresses */
    char address[15];
    unsigned long port = 0;

    /* Allows to safely check if address is empty */
    address[0] = '\0';

    if (argc == 1) {
        print_help();
        sentinel("Too few arguments");
    }

    /* Parse command line arguments */
    int i;
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[i], "-m") == 0 ||
                   strcmp(argv[i], "--mode") == 0) {
            check(i < argc - 1,
                  "An operating mode needs to be specified");
            i++;
            if (strcmp(argv[i], "client") == 0) mode = 0;
            if (strcmp(argv[i], "server") == 0) mode = 1;
        } else if (strcmp(argv[i], "-a") == 0 ||
                   strcmp(argv[i], "--address") == 0) {
            check(i < argc - 1,
                  "An IPv4 address is required");
            check(valid_ip(argv[++i]) == 1,
                  "Provided IP address is in invalid format");
            strcpy(address, argv[i]);
        } else if (strcmp(argv[i], "-p") == 0 ||
                   strcmp(argv[i], "--port") == 0) {
            check(i < argc - 1,
                  "A port number is required");
            if (valid_port(argv[++i], &port) == 0)
                goto error;
        } else {
            print_help();
            sentinel("Unrecognized arguments");
        }
    }

    /* See if mandatory arguments have been provided */
    check(port != 0, "A port number needs to be specified");

    switch (mode) {
    case 0: {
        /* Client mode specific argument checks */
        check(address[0] != '\0', "Client mode requires an IP address");
        client_mode(address, port);
        break;
    }
    case 1: {
        if (address[0] != '\0')
            log_warn("Address is not used in server mode");
        server_mode(port);
        break;
    }
    default:
        sentinel("Unrecognized operating mode specified");
        break;
    }

    return 0;
error:
    return 1;
}
