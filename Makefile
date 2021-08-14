CC = gcc -std=c11 -g -O0 -pthread
exe = server

$(exe): server.c
	$(CC) server.c -o $(exe)

clean:
	rm $(exe) -f
