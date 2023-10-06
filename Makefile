CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = server client

.PHONY: all clean

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) $< -o $@

client: client.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGETS)
