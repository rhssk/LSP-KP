#include <unistd.h>
#include <sys/socket.h>
#include "debug_macros.h"
#include "communication.h"

int recv_msg(int remote, void *data, size_t size)
{
    ssize_t status;

    status = recv(remote, data, size, 0);
    if (status == 0) {
        log_info("Remote has disconnected");
        goto error;
    }
    check(status >= 0, "Failed to read data from remote");

    debug("RECEIVED DATA FROM REMOTE(%d)", remote);

    return 0;
error:
    return -1;
}

int send_msg(int remote, void *data, size_t size)
{
    ssize_t status;

    status = send(remote, data, size, 0);
    if (status == 0) {
        log_info("Remote has disconnected");
    }
    check(status >= 0, "Failed to write data to remote");

    debug("SENT DATA TO REMOTE(%d)", remote);

    return 0;
error:
    return -1;
}
