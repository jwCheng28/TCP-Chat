import socket
import sys
import select

class Client:
    def __init__(self, host=None, port=None, user="Anonymous"):
        self.clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = host or socket.gethostname()
        self.port = port or 45001
        self.user = user
        self.clientSocket.connect((self.host, self.port))
        self.connected = True
        self.clientSocket.send(self.user.encode())

    def sendMSG(self):
        # msg = input("Enter Message: ")
        msg = sys.stdin.readline().strip()
        self.clientSocket.send(msg.encode())
        if msg == "QUIT":
            self.connected = False
            self.clientSocket.close()
            exit()

    def receiveMSG(self):
        resp = self.clientSocket.recv(300).decode()
        print(resp)

    def runClient(self):
        while True:
            readList = [sys.stdin, self.clientSocket]
            readable, _, _ = select.select(readList, [], [])
            for r in readable:
                if r == self.clientSocket:
                    self.receiveMSG()
                else:
                    self.sendMSG()

if __name__ == "__main__":
    user = input("Enter Username: ")
    client = Client(user=user)
    client.runClient()
