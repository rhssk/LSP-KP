#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "debug_macros.h"
#include "common.h"

void get_remote_ip_port(int sock, char *ipstr, int *port)
{
    socklen_t len;
    struct sockaddr_storage addr;
    len = sizeof addr;
    getpeername(sock, (struct sockaddr *)&addr, &len);
    // deal with both IPv4 and IPv6:
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        *port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, INET6_ADDRSTRLEN);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        *port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, INET6_ADDRSTRLEN);
    }
}

int recv_msg(int remote, void *data, size_t size)
{
    int port;
    char ipstr[INET6_ADDRSTRLEN];
    ssize_t status;

    get_remote_ip_port(remote, ipstr, &port);

    status = recv(remote, data, size, 0);
    if (status == 0) {
        log_info("Remote(%s:%d) has disconnected", ipstr, port);
        goto error;
    }
    check(status >= 0, "Failed to read data from remote(%s:%d)", ipstr, port);

    debug("RECEIVED DATA FROM REMOTE(%s:%d)", ipstr, port);

    return 0;
error:
    return -1;
}

int send_msg(int remote, void *data, size_t size)
{
    int port;
    char ipstr[INET6_ADDRSTRLEN];
    ssize_t status;

    get_remote_ip_port(remote, ipstr, &port);

    status = send(remote, data, size, 0);
    if (status == 0) {
        log_info("Remote(%s:%d) has disconnected", ipstr, port);
    }
    check(status >= 0, "Failed to write data to remote(%s:%d)", ipstr, port);

    debug("SENT DATA TO REMOTE(%s:%d)", ipstr, port);

    return 0;
error:
    return -1;
}
