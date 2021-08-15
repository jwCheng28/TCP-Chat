import socket
import sys
import select

class Client:
    def __init__(self, host=None, port=None, user="Anonymous"):
        self.clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = host or socket.gethostname()
        self.port = port or 45001
        self.user = user
        self.connected = False

    def quit(self):
        self.connected = False
        self.clientSocket.close()
        exit()

    def connectToServer(self):
        self.clientSocket.connect((self.host, self.port))
        connResp = self.clientSocket.recv(10).decode()
        if connResp == "FAILED":
            print("Connection Failed; Exiting")
            self.quit()
        else:
            self.connected = True
            self.clientSocket.send(self.user.encode())

    def sendMSG(self):
        msg = sys.stdin.readline().strip()
        self.clientSocket.send(msg.encode())
        if msg[:4] == "QUIT":
            self.quit()

    def receiveMSG(self):
        resp = self.clientSocket.recv(300).decode()
        print(resp)

    def runClient(self):
        self.connectToServer()
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
