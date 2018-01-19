#include <string.h>
#include "debug_macros.h"
#include "common.h"
#include "server.h"
#include "game.h"
#include "main.h"

#define PORT_SIZE 6

void print_help(void)
{
    printf("Usage: ./server -p/--port PORT_NUM\n");
}

int main(int argc, char **argv)
{
    int i;

    /* Can hold only IPv4 addresses */
    char port[PORT_SIZE];

    /* Allows to safely check if port is empty */
    port[0] = '\0';

    if (argc == 1) {
        print_help();
        sentinel("Too few arguments");
    }

    /* Parse command line arguments */
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[i], "-p") == 0 ||
                   strcmp(argv[i], "--port") == 0) {
            check(i < argc - 1,
                  "A port number is required");
            strcpy(port, argv[++i]);
            if (valid_port(port) == 0)
                goto error;
        } else {
            print_help();
            sentinel("Unrecognized arguments");
        }
    }

    /* See if mandatory arguments have been provided */
    check(port[0] != '\0', "A port number needs to be specified");
    init_game(); // Create shared memory before serving clients
    init_server(port);

    return 0;
error:
    return 1;
}
