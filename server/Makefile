CC = g++
CFLAGS = -Wall -Wextra -Wunused
TARGET = server

.PHONY: all clean

all: $(TARGET)

# utils.c is present one level above so ../ is used
$(TARGET): server.cpp server_utils.cpp ../utils.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
