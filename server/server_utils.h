#include <stdint.h>
#include <vector>
#include <map>
#include <string>

#define MAX_CONNECTIONS 10
#define k_max_args 1024 // max number of arguments/queries

// all possible states for fd
enum
{
    STATE_REQ = 0, // request
    STATE_RES = 1, // response
    STATE_END = 2, // mark the connection for deletion
};

// For client querires
enum
{
    RES_OK = 0,
    RES_ERR = 1,
    RES_NX = 2,
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

// The data structure for the key space. We'll be replaced with actual Hashmap
static std::map<std::string, std::string> g_map; // key -> value

// void do_something(int);
int32_t one_request(int fd);

void fd_set_nb(int fd); // set fd to non-blocking mode

int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd);
void connection_io(Conn *conn);

// CRUD operation for map - get, set, del
static uint32_t do_get(
    const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);

static uint32_t do_set(
    const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);

static uint32_t do_del(
    const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);

static bool cmd_is(const std::string &word, const char *cmd);

// req, reqLen, resCode, res, resLen
int32_t do_request(const uint8_t *req, uint32_t reqlen, uint32_t *rescode, uint8_t *res, uint32_t *reslen);
