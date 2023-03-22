import threading
import socket
import os

HOST = "127.0.0.1"
PORT = 5050


def udpServer():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    sock.bind((HOST, PORT))

    while True:

        # 지정한 sock으로 들어온 data 수신 (client 주소를 같이 기록)
        data, addr = sock.recvfrom(2048)

        data = data.decode().upper()  # data 가공

        sock.sendto(data.encode(), addr)  # data를 지정한 addr로 송신

    sock.close()


def udpClient():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while True:

        msg = input("'UDP client' : ")

        sock.sendto(msg.encode(), (HOST, PORT))  # 지정된 포트로 IP, port로 송신

        recvMsg, addr = sock.recvfrom(2048)  # 메시지 수신

        print("from UDP server :", recvMsg.decode())

    sock.close()


def tcpServer():

    serv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    serv_sock.bind((HOST, PORT))  # 바인딩
    serv_sock.listen()  
    clnt_sock, addr = serv_sock.accept()  # 새로운 연결 대기

    while True:

        data = clnt_sock.recv(1024)
        if not data:
            break

        data = data.decode().upper()  # data 가공

        clnt_sock.send(data.encode())  # data 송신

    clnt_sock.close()
    serv_sock.close()


def tcpClient():

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock.connect((HOST, PORT))

    while True:
        msg = input("'TCP client' : ")

        sock.send(msg.encode())

        data = sock.recv(1024)

        print("from TCP server :", data.decode())

    sock.close()


if __name__ == "__main__":
    
    thread1 = threading.Thread(target=udpServer)
    thread2 = threading.Thread(target=udpClient)

    thread1.start()
    thread2.start()

    thread1.join()
    thread2.join()
