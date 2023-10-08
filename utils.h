#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define PORT 5050 // server's PORT number

void msg(const char *);
void die(const char *);

// I/O helper
int32_t read_full(int fd, char *buf, size_t n);
int32_t write_full(int fd, char *buf, size_t n);

#endif // UTILS_H
