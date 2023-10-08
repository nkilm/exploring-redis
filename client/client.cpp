#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <vector>
#include <string>

#include "../utils.h"
#include "client_utils.h"

int main(int argc, char **argv)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(PORT);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("connect");
    }

    /*
    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        die("read");
    }
    printf("server says: %s\n", rbuf);
    close(fd);
    return 0;
    */

    /*
    // multiple requests to the server
    int32_t err = query(fd, "hello1");
    if (err)
        goto L_DONE;

    err = query(fd, "hello2");
    if (err)
        goto L_DONE;

    err = query(fd, "hello3");
    if (err)
        goto L_DONE;

    */

    /*
    // multiple pipelined requests
    const char *query_list[3] = {"hello1", "hello2", "hello3"};
    for (size_t i = 0; i < 3; ++i)
    {
        int32_t err = send_req(fd, query_list[i]);
        if (err)
        {
            goto L_DONE;
        }
    }
    for (size_t i = 0; i < 3; ++i)
    {
        int32_t err = read_res(fd);
        if (err)
        {
            goto L_DONE;
        }
    }
    */

    std::vector<std::string> cmd;
    for (int i = 1; i < argc; ++i)
    {
        cmd.push_back(argv[i]);
    }
    int32_t err = send_req(fd, cmd);
    if (err)
    {
        goto L_DONE;
    }
    err = read_res(fd);
    if (err)
    {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}
