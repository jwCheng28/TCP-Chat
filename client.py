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

        self.RESPSIZE = 300
        self.KEYSIZE = 12
        self.KEYS = {"CONN_FAILURE":-1, "CONN_SUCCESS":1}

    def quit(self):
        self.connected = False
        self.clientSocket.close()
        exit()

    def connectToServer(self):
        self.clientSocket.connect((self.host, self.port))
        connResp = self.clientSocket.recv(self.KEYSIZE).decode()
        if connResp in self.KEYS:
            if self.KEYS[connResp] < 0:
                print("Connection Failed; Exiting")
                self.quit()
            elif self.KEYS[connResp] > 0:
                print("Connection Success")
                print("_"*36)
                self.connected = True
                self.clientSocket.send(self.user.encode())
        else:
            print("Error has Occurred; Exiting")
            self.quit()

    def sendMSG(self):
        msg = sys.stdin.readline().strip()
        self.clientSocket.send(msg.encode())
        if msg == "QUIT":
            self.quit()

    def receiveMSG(self):
        resp = self.clientSocket.recv(self.RESPSIZE).decode()
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
    port = int(input("Enter Port Number: "))
    user = input("Enter Username: ")
    client = Client(port=port, user=user)
    client.runClient()
