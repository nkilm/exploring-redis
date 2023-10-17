#include <stdint.h>
#include <vector>
#include <map>
#include <string>

#include "common.h"
#include "../utils.h"
// #include "./hashtable/hashtable.h" // don't include as zset already has required definitions
#include "./zset/zset.h"
#include "list.h"
#include "./heap/heap.h"
#include "thread_pool.h"

#define MAX_CONNECTIONS 10
#define k_max_args 1024 // max number of arguments/queries
const size_t k_max_msg = 4096;

struct Conn;

// global variables
static struct
{
    HMap db;
    // a map of all client connections, keyed by fd
    std::vector<Conn *> fd2conn;
    // timers for idle connections
    DList idle_list;
    // timers for TTLs
    std::vector<HeapItem> heap;
    // the thread pool
    TheadPool tp;
} g_data;

enum
{
    STATE_REQ = 0,
    STATE_RES = 1,
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
    uint64_t idle_start = 0;
    // timer
    DList idle_list;
};

enum
{
    T_STR = 0,
    T_ZSET = 1,
};

// the structure for the key
struct Entry
{
    struct HNode node;
    std::string key;
    std::string val;
    uint32_t type = 0;
    ZSet *zset = NULL;
    // for TTLs
    size_t heap_idx = -1;
};

enum
{
    ERR_UNKNOWN = 1,
    ERR_2BIG = 2,
    ERR_TYPE = 3,
    ERR_ARG = 4,
};

void entry_del(Entry *ent);
uint64_t get_monotonic_usec();
void state_req(Conn *conn);
void state_res(Conn *conn);
