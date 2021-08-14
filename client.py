import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostname()
port = 45001

s.connect((host, port))
user = input("Enter Username: ")
s.send(user.encode())
while True:
    msg = input("Enter MSG: ")
    s.send(msg.encode())
    if msg == "QUIT":
        break
    response = [e.strip() for e in s.recv(300).decode().split(":")]
    respUser, respMsg = response[0], response[1]
    print("{}: {}".format(respUser, respMsg))
s.close()
