.PHONEY: clean
all:
	make server
	make client

server:
	gcc -Wall -Wextra server.c -o server

client:
	gcc -Wall -Wextra client.c -o client

clean:
	rm -rf *.out server client