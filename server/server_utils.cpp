#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "../utils.h"
#include "server_utils.h"

/*
void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}
*/

// The parser
int32_t one_request(int fd)
{
    // 1. first we extract the header and the actual data
    char rbuf[4 + k_max_msg + 1]; // header(4 bytes) + MAX_MSG_SIZE + 1(terminator)

    int32_t err = read_full(fd, rbuf, 4); // first read only the header

    if (err)
    {
        if (errno == 0)
            // connection closed by the client
            msg("EOF");

        else
            msg("read() error");

        return err;
    }

    // 2. check the actual length of the data
    int32_t len = 0;
    memcpy(&len, rbuf, 4);

    if (len > k_max_msg)
    {
        msg("Message is too long");
        return -1;
    }

    // request body - the actual message, stored in the buffer starting from 5th byte
    err = read_full(fd, &rbuf[4], len);
    if (err)
    {
        msg("read() error");
        return err;
    }

    rbuf[4 + len] = '\0'; // null terminated string

    printf("client says: %s\n", &rbuf[4]);

    // reply using the same protocol
    const char reply[] = "received...";

    char wbuf[4 + sizeof(reply)];

    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);

    return write_full(fd, wbuf, 4 + len);
}

void fd_set_nb(int fd)
{
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0); // fetch current flags
    if (errno)
    {
        die("fcntl error");
        return;
    }

    flags |= O_NONBLOCK; // fet non-blocking flag to true

    errno = 0;
    (void)fcntl(fd, F_SETFL, flags);
    if (errno)
    {
        die("fcntl error");
    }
}
