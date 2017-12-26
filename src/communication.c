#include <unistd.h>
#include "debug_macros.h"
#include "communication.h"

int recv_msg(int remote, void *data, size_t size)
{
    ssize_t status;

    status = read(remote, data, size);
    if (status == 0) {
        log_info("Remote has disconnected");
        goto error;
    }
    check(status >= 0, "Failed to read data from remote");

    debug("RECEIVED FROM REMOTE: %d", remote);

    return 0;
error:
    return -1;
}

int send_msg(int remote, void *data, size_t size)
{
    ssize_t status;

    status = write(remote, data, size);
    if (status == 0) {
        log_info("Wrote 0 bytes to remote: %d", remote);
    }
    check(status >= 0, "Failed to write data to remote");

    debug("SENT TO REMOTE: %d", remote);

    return 0;
error:
    return -1;
}
