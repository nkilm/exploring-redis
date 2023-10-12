#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define PORT 5050      // server's PORT number
#define k_max_msg 4096 // maximum message size

// different datatypes
enum
{
    SER_NIL = 0, // like NULL
    SER_ERR = 1, // error
    SER_STR = 2, // string
    SER_INT = 3, // integer
    SER_ARR = 4, // array
};

void msg(const char *);
void die(const char *);

// I/O helper
int32_t read_full(int fd, char *buf, size_t n);
int32_t write_full(int fd, char *buf, size_t n);

#endif // UTILS_H
