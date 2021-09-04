# TCP Chat Server and Client
This project is mainly for learning how to create a basic multithreaded server. In particular, we'll be creating a basic TCP chat server that allows multiple users to talk with each other.

## Running the Project
The current implementation only targets linux machines, hence the code will not work unless run on linux or WSL. A Makefile is provided, simply run `make` to compile the C server.
Or run `make server_tp` to compile the thread pool implementation of the server.
### Starting the Server
To start the server, simply run the generated executable with desire port number `./server [port number]`
### Start a Client
The client is currently implemented in Python, simply run `python client.py` and the program will prompt for a Hostname (press enter to just use localhost), Port Number (use the same as server), and Username. Then after a "Connection Success" message is shown, you can start typing messages and send them by pressing enter.
### Quit the Chat
To quit the chat, simply type `QUIT` in the chat.

## Current Support
 - Currently this only runs on linux-type machines (the server part).
 - Currently only support up to 5 users at a time.
