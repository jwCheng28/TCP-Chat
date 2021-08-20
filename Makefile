CC = gcc -std=c11 -g -O0 -pthread
exe = server

$(exe): server.c
	$(CC) server.c -o $(exe)

server_tp: server_tp.c
	$(CC) server_tp.c -o $(exe)

clean:
	rm $(exe) -f
