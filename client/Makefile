CC = g++
CFLAGS = -Wall -Wextra -Wunused
TARGET = client

.PHONY: all clean

all: $(TARGET)

# utils.c is present one level above so ../ is used
$(TARGET): client.cpp client_utils.cpp ../utils.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
