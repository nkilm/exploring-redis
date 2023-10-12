#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <assert.h>
#include <vector>

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

// Data Serialization
void out_nil(std::string &out)
{
    out.push_back(SER_NIL);
}

void out_str(std::string &out, const std::string &val)
{
    out.push_back(SER_STR);
    uint32_t len = (uint32_t)val.size();
    out.append((char *)&len, 4);
    out.append(val);
}

void out_int(std::string &out, int64_t val)
{
    out.push_back(SER_INT);
    out.append((char *)&val, 8);
}

void out_err(std::string &out, int32_t code, const std::string &msg)
{
    out.push_back(SER_ERR);
    out.append((char *)&code, 4);
    uint32_t len = (uint32_t)msg.size();
    out.append((char *)&len, 4);
    out.append(msg);
}

void out_arr(std::string &out, uint32_t n)
{
    out.push_back(SER_ARR);
    out.append((char *)&n, 4);
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

static void conn_put(std::vector<Conn *> &fd2conn, struct Conn *conn)
{
    if (fd2conn.size() <= (size_t)conn->fd)
    {
        fd2conn.resize(conn->fd + 1);
    }
    fd2conn[conn->fd] = conn;
}

int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd)
{
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0)
    {
        msg("accept() error");
        return -1; // error
    }

    // set the new connection fd to nonblocking mode
    fd_set_nb(connfd);
    // creating the struct Conn
    struct Conn *conn = (struct Conn *)malloc(sizeof(struct Conn));
    if (!conn)
    {
        close(connfd);
        return -1;
    }
    conn->fd = connfd;
    conn->state = STATE_REQ;
    conn->rbuf_size = 0;
    conn->wbuf_size = 0;
    conn->wbuf_sent = 0;
    conn_put(fd2conn, conn);
    return 0;
}

static void state_req(Conn *conn);
static void state_res(Conn *conn);

bool cmd_is(const std::string &word, const char *cmd)
{
    return 0 == strcasecmp(word.c_str(), cmd);
}

static int32_t parse_req(
    const uint8_t *data, size_t len, std::vector<std::string> &out)
{
    if (len < 4)
    {
        return -1;
    }
    uint32_t n = 0;
    memcpy(&n, &data[0], 4);
    if (n > k_max_args)
    {
        return -1;
    }

    size_t pos = 4;
    while (n--)
    {
        if (pos + 4 > len)
        {
            return -1;
        }
        uint32_t sz = 0;
        memcpy(&sz, &data[pos], 4);
        if (pos + 4 + sz > len)
        {
            return -1;
        }
        out.push_back(std::string((char *)&data[pos + 4], sz));
        pos += 4 + sz;
    }

    if (pos != len)
    {
        return -1; // trailing garbage
    }
    return 0;
}
static void cb_scan(HNode *node, void *arg)
{
    std::string &out = *(std::string *)arg;
    out_str(out, container_of(node, Entry, node)->key);
}

void h_scan(HTab *tab, void (*f)(HNode *, void *), void *arg)
{
    if (tab->size == 0)
    {
        return;
    }
    for (size_t i = 0; i < tab->mask + 1; ++i)
    {
        HNode *node = tab->tab[i];
        while (node)
        {
            f(node, arg);
            node = node->next;
        }
    }
}

void do_keys(std::vector<std::string> &cmd, std::string &out)
{
    (void)cmd;
    out_arr(out, (uint32_t)hm_size(&g_data.db));
    h_scan(&g_data.db.ht1, &cb_scan, &out);
    h_scan(&g_data.db.ht2, &cb_scan, &out);
}

void do_request(std::vector<std::string> &cmd, std::string &out)
{
    if (cmd.size() == 1 && cmd_is(cmd[0], "keys"))
    {
        do_keys(cmd, out);
    }
    else if (cmd.size() == 2 && cmd_is(cmd[0], "get"))
    {
        do_get(cmd, out);
    }
    else if (cmd.size() == 3 && cmd_is(cmd[0], "set"))
    {
        do_set(cmd, out);
    }
    else if (cmd.size() == 2 && cmd_is(cmd[0], "del"))
    {
        do_del(cmd, out);
    }
    else
    {
        // cmd is not recognized
        out_err(out, ERR_UNKNOWN, "Unknown cmd");
    }
}

bool try_one_request(Conn *conn)
{
    // try to parse a request from the buffer
    if (conn->rbuf_size < 4)
    {
        // not enough data in the buffer. Will retry in the next iteration
        return false;
    }
    uint32_t len = 0;
    memcpy(&len, &conn->rbuf[0], 4);
    if (len > k_max_msg)
    {
        msg("too long");
        conn->state = STATE_END;
        return false;
    }
    if (4 + len > conn->rbuf_size)
    {
        // not enough data in the buffer. Will retry in the next iteration
        return false;
    }

    // parse the request
    std::vector<std::string> cmd;
    if (0 != parse_req(&conn->rbuf[4], len, cmd))
    {
        msg("bad req");
        conn->state = STATE_END;
        return false;
    }

    // got one request, generate the response.
    std::string out;
    do_request(cmd, out);

    // pack the response into the buffer
    if (4 + out.size() > k_max_msg)
    {
        out.clear();
        out_err(out, ERR_2BIG, "response is too big");
    }

    uint32_t wlen = (uint32_t)out.size();
    memcpy(&conn->wbuf[0], &wlen, 4);
    memcpy(&conn->wbuf[4], out.data(), out.size());
    conn->wbuf_size = 4 + wlen;

    // remove the request from the buffer.
    // note: frequent memmove is inefficient.
    // note: need better handling for production code.
    size_t remain = conn->rbuf_size - 4 - len;
    if (remain)
    {
        memmove(conn->rbuf, &conn->rbuf[4 + len], remain);
    }
    conn->rbuf_size = remain;

    // change state
    conn->state = STATE_RES;
    state_res(conn);

    // continue the outer loop if the request was fully processed
    return (conn->state == STATE_REQ);
}

static bool try_fill_buffer(Conn *conn)
{
    // try to fill the buffer
    assert(conn->rbuf_size < sizeof(conn->rbuf));
    ssize_t rv = 0;
    do
    {
        size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0 && errno == EAGAIN)
    {
        // got EAGAIN, stop.
        return false;
    }
    if (rv < 0)
    {
        msg("read() error");
        conn->state = STATE_END;
        return false;
    }
    if (rv == 0)
    {
        if (conn->rbuf_size > 0)
        {
            msg("unexpected EOF");
        }
        else
        {
            msg("EOF");
        }
        conn->state = STATE_END;
        return false;
    }

    conn->rbuf_size += (size_t)rv;
    assert(conn->rbuf_size <= sizeof(conn->rbuf));

    // Try to process requests one by one.
    // Why is there a loop? Please read the explanation of "pipelining".
    while (try_one_request(conn))
    {
    }
    return (conn->state == STATE_REQ);
}

static void state_req(Conn *conn)
{
    while (try_fill_buffer(conn))
    {
    }
}

static bool try_flush_buffer(Conn *conn)
{
    ssize_t rv = 0;
    do
    {
        size_t remain = conn->wbuf_size - conn->wbuf_sent;
        rv = write(conn->fd, &conn->wbuf[conn->wbuf_sent], remain);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0 && errno == EAGAIN)
    {
        // got EAGAIN, stop.
        return false;
    }
    if (rv < 0)
    {
        msg("write() error");
        conn->state = STATE_END;
        return false;
    }
    conn->wbuf_sent += (size_t)rv;
    assert(conn->wbuf_sent <= conn->wbuf_size);
    if (conn->wbuf_sent == conn->wbuf_size)
    {
        // response was fully sent, change state back
        conn->state = STATE_REQ;
        conn->wbuf_sent = 0;
        conn->wbuf_size = 0;
        return false;
    }
    // still got some data in wbuf, could try to write again
    return true;
}

static void state_res(Conn *conn)
{
    while (try_flush_buffer(conn))
    {
    }
}

void connection_io(Conn *conn)
{
    if (conn->state == STATE_REQ)
    {
        state_req(conn);
    }
    else if (conn->state == STATE_RES)
    {
        state_res(conn);
    }
    else
    {
        assert(0); // not expected
    }
}

static bool entry_eq(HNode *lhs, HNode *rhs)
{
    struct Entry *le = container_of(lhs, struct Entry, node);
    struct Entry *re = container_of(rhs, struct Entry, node);
    return lhs->hcode == rhs->hcode && le->key == re->key;
}

uint64_t str_hash(const uint8_t *data, size_t len)
{
    uint32_t h = 0x811C9DC5;
    for (size_t i = 0; i < len; i++)
    {
        h = (h + data[i]) * 0x01000193;
    }
    return h;
}

void do_get(std::vector<std::string> &cmd, std::string &out)
{
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node)
    {
        return out_nil(out);
    }

    const std::string &val = container_of(node, Entry, node)->val;
    out_str(out, val);
}

void do_set(std::vector<std::string> &cmd, std::string &out)
{
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (node)
    {
        container_of(node, Entry, node)->val.swap(cmd[2]);
    }
    else
    {
        Entry *ent = new Entry();
        ent->key.swap(key.key);
        ent->node.hcode = key.node.hcode;
        ent->val.swap(cmd[2]);
        hm_insert(&g_data.db, &ent->node);
    }
    return out_nil(out);
}

void do_del(std::vector<std::string> &cmd, std::string &out)
{
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    HNode *node = hm_pop(&g_data.db, &key.node, &entry_eq);
    if (node)
    {
        delete container_of(node, Entry, node);
    }
    return out_int(out, node ? 1 : 0);
}
