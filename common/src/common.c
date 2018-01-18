#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "debug_macros.h"
#include "common.h"

int valid_ip(const char *address)
{
    struct sockaddr_in sa;

    return inet_pton(AF_INET, address, &(sa.sin_addr)) == 1;
}

int valid_port(const char *port_str)
{
    unsigned long tmp_port;
    char *endptr;

    /* Check for negative numbers */
    check(strchr(port_str, '-') == NULL,
          "Port number should be positive");

    tmp_port = strtoul(port_str, &endptr, 10);

    /* Check for non-integer numbers */
    check(*endptr == '\0',
          "Port number should be a natural number");

    /* Check for range */
    check(tmp_port >= 2000 && tmp_port <= 65535,
          "Port number should be between 2000 and 65535");

    return 1;
error:
    return 0;
}

void get_remote_ip_port(const int sock, char *ipstr, int *port)
{
    socklen_t len;
    struct sockaddr_storage addr;

    len = sizeof(addr);
    getpeername(sock, (struct sockaddr *)&addr, &len);

    /* Deal with both IPv4 and IPv6: */
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        *port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, INET6_ADDRSTRLEN);
    } else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        *port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, INET6_ADDRSTRLEN);
    }
}

int recv_msg(const int remote, void *data, const size_t size)
{
    int port;
    char ipstr[INET6_ADDRSTRLEN];
    ssize_t bytes;

    get_remote_ip_port(remote, ipstr, &port);

    bytes = recv(remote, data, size, 0);
    if (bytes == 0) {
        log_info("Remote(%s:%d) has disconnected", ipstr, port);
        goto error;
    }
    check(bytes >= 0, "Failed to read data from remote(%s:%d)", ipstr, port);

    debug("RECEIVED FROM REMOTE(%s:%d)", ipstr, port);

    return 0;
error:
    return -1;
}

int send_msg(const int remote, void *data, const size_t size)
{
    int port;
    char ipstr[INET6_ADDRSTRLEN];
    ssize_t bytes;

    get_remote_ip_port(remote, ipstr, &port);

    bytes = send(remote, data, size, 0);
    if (bytes == 0) {
        log_info("Remote(%s:%d) has disconnected", ipstr, port);
    }
    check(bytes >= 0, "Failed to write data to remote(%s:%d)", ipstr, port);

    debug("SENT TO REMOTE(%s:%d)", ipstr, port);

    return 0;
error:
    return -1;
}
