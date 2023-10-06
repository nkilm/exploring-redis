#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "../utils.h"
#include "server_utils.h"

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("socket()");
    }

    // this is needed for most server applications
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(PORT);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("bind()");
    }
    printf("---Server started on PORT %d---\n\n", PORT);

    // listen
    rv = listen(fd, MAX_CONNECTIONS);
    if (rv)
    {
        die("listen()");
    }

    while (true)
    {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t sock_len = sizeof(client_addr);
        int conn_fd = accept(fd, (struct sockaddr *)&client_addr, &sock_len);
        if (conn_fd < 0)
        {
            continue; // error
        }

        // do_something(conn_fd);

        // only serves one client connection at once
        while (true)
        {
            int32_t err = one_request(conn_fd);
            if (err)
            {
                break;
            }
        }

        close(conn_fd);
    }

    return 0;
}
