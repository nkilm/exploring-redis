CC = gcc
CFLAGS = -Wall -Wextra -Wunused
TARGET = server

.PHONY: all clean

all: $(TARGET)

# utils.c is present one level above so ../ is used
$(TARGET): server.c server_utils.c ../utils.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
