CC = g++
# remove -g if debugging is not required
CFLAGS = -Wall -Wextra -Wunused -g

# Target binary name
TARGET = server

# Source files - the order of the files matters!
SRCS = server.cpp ../utils.cpp server_utils.cpp ./hashtable/hashtable.cpp ./avl/avl.cpp ./zset/zset.cpp ./heap/heap.cpp ./thread_pool.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
