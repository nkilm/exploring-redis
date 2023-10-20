#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define PORT 5050 // server's PORT number

// different datatypes
enum
{
    SER_NIL = 0,
    SER_ERR = 1,
    SER_STR = 2,
    SER_INT = 3,
    SER_DBL = 4,
    SER_ARR = 5,
};

void msg(const char *);
void die(const char *);

// I/O helper
int32_t read_full(int fd, char *buf, size_t n);
int32_t write_full(int fd, char *buf, size_t n);

#endif // UTILS_H
