import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = "127.0.0.1"
port = 45001

s.connect((host, port))
msg = input()
s.send(msg.encode())
#resp = s.recv(64)
#print("Message Received: {}".format(resp))
s.close()
