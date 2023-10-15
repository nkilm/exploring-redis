CC = g++
CFLAGS = -Wall -Wextra -Wunused

# Target binary name
TARGET = server

# Source files
SRCS = server.cpp server_utils.cpp ../utils.cpp ./zset/zset.cpp ./hashtable/hashtable.cpp ./avl/avl.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
