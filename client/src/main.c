#include <string.h>
#include <arpa/inet.h>
#include "debug_macros.h"
#include "common.h"
#include "client.h"
#include "main.h"

#define PORT_SIZE 6

void print_help(void)
{
    printf("Usage: ./client -a/--address SERVER_IP -p/--port SERVER_PORT_NUM\n\
Enter name to request a place in lobby\n\
\n\
Keybindings (all keys sent by pressing Enter):\n\
R           - tell server that you're ready to play\n\
Ctrl+D or Q - disconnect\n\
UP or W     - move up\n\
DOWN or S   - move down\n\
LEFT or A   - move left\n\
RIGHT or D  - move right\n\
P           - plant a dynamite\n\
D           - detonate all planted dynamites\n\
G           - pick up dynamite from the ground\n\
T           - taunt\n\
L           - laugh\n\
F           - dance\n\
");
}

int main(int argc, char **argv)
{
    int i;
    /* Can hold only IPv4 addresses */
    char address[INET_ADDRSTRLEN];
    char port[PORT_SIZE];

    /* Allows to safely check if address or port is empty */
    address[0] = '\0';
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
        } else if (strcmp(argv[i], "-a") == 0 ||
                   strcmp(argv[i], "--address") == 0) {
            check(i < argc - 1,
                  "A IP address is required");
            check(valid_ip(argv[++i]) == 1,
                  "Provided IP address is in invalid format");
            strcpy(address, argv[i]);
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
    check(address[0] != '\0', "An IP address needs to be specified");
    init_client(address, port);

    return 0;
error:
    return 1;
}
