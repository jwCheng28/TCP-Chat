# TCP Chat Server and Client
This project is mainly for learning how to create a basic multithreaded server. In particular, we'll be creating a basic TCP chat server that allows multiple users to talk with each other.

## Running the Project
The current implementation only targets linux machines, hence the code will not work unless run on linux or WSL. Otherwise, a Makefile is provided, simply run `make` to compile the C server.
### Starting the Server
To start the server, simply run the executable with desire port number `./server [port number]`
### Start a Client
The client is currently implemented in Python, simply run `python client.py` and the program will prompt for a Username and Port Number (use the same as server). Then after a "Connection Success" message is shown, you can start typing messages and send them by pressing enter.
