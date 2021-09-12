CC = gcc -std=c11 
BUILD = -O3 -w -s
DEBUG = -g -Wall
THREAD = -pthread
exe = server

$(exe): server.c
	$(CC) $(BUILD) server.c $(THREAD) -o $(exe)

server_tp: server_tp.c
	$(CC) $(BUILD) server_tp.c $(THREAD) -o $(exe)

clean:
	rm $(exe) -f
