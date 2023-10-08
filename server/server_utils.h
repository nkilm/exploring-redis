#include <stdint.h>
#include <vector>

#define MAX_CONNECTIONS 10
#define k_max_msg 4096

// all possible states for fd
enum
{
    STATE_REQ = 0, // request
    STATE_RES = 1, // response
    STATE_END = 2, // mark the connection for deletion
};

struct Conn
{
    int fd = -1;
    uint32_t state = 0; // either STATE_REQ or STATE_RES

    // buffer for reading
    size_t rbuf_size = 0;
    uint8_t rbuf[4 + k_max_msg];
    // buffer for writing
    size_t wbuf_size = 0;
    size_t wbuf_sent = 0;
    uint8_t wbuf[4 + k_max_msg];
};

// void do_something(int);

int32_t one_request(int fd);

void fd_set_nb(int fd); // set fd to non-blocking mode

int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd);
void connection_io(Conn *conn);
