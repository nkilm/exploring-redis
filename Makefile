CC = gcc
CFLAGS = -Wall -Wextra -Wunused
TARGETS = server client

.PHONY: all clean

all: $(TARGETS)

server:
	$(MAKE) -C server

client:
	$(MAKE) -C client

clean:
	$(MAKE) -C server clean
	$(MAKE) -C client clean
