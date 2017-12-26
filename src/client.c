#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug_macros.h"
#include "packets.h"
#include "client.h"

void client_mode(const char *address, const unsigned long port)
{
    int sock;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    check(sock >= 0, "Couldn't open the socket");
    server = gethostbyname(address);
    check(server != NULL, "Couldn't find the host");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    check(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0,
          "Failed to connect to server");

    check(send_msg(sock) != -1, "Failed to send to client");
    check(recv_msg(sock) != -1, "Failed to read from client");

    close(sock);
error:
    return;
}

int recv_msg(int server)
{
    char buffer[255];

    check(read(server, buffer, sizeof(buffer)) >= 0,
          "Failed to read from socket");

    debug("FROM SERVER: %d RECEIVE: %s", server, buffer);

    return 0;
error:
    return -1;
}

int send_msg(int server)
{
    char buffer[255] = "CLIENT MESSAGE";

    check(write(server, buffer, sizeof(buffer)) >= 0,
          "Failed to write to socket");

    debug("TO SERVER: %d SEND: %s", server, buffer);

    return 0;
error:
    return -1;
}
