CC = gcc
CFLAGS = -Wall -Wextra -Wunused
TARGETS = server client

.PHONY: all clean

all: $(TARGETS)

server: server.c server_utils.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGETS)
