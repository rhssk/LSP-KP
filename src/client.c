#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug_macros.h"
#include "packets.h"
#include "communication.h"
#include "client.h"

#define BUFFER_SIZE 255

void client_mode(const char *address, const unsigned long port)
{
    int sock;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    check(sock >= 0,
          "Couldn't open the socket");
    server = gethostbyname(address);
    check(server != NULL,
          "Couldn't find the host");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    check(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0,
          "Failed to connect to server");

    char *msg = malloc(BUFFER_SIZE);
    while (1) {
        fgets(msg, BUFFER_SIZE, stdin);
        if (send_msg(sock, msg, BUFFER_SIZE) == -1) goto error;

        memset(msg, '\0', BUFFER_SIZE);
        if (recv_msg(sock, msg, BUFFER_SIZE) == -1) goto error;
    }

    free(msg);
    close(sock);
error:
    return;
}
