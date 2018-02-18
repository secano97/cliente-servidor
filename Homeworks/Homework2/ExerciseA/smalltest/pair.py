import zmq 
import time 
import sys 
from multiprocessing import Process

def pairserver(port = "5556"):
    
    context = zmq.Context()
    socket = context.socket(zmq.PAIR)
    socket.bind("tcp://*:%s" % port)
    n = 5

    socket.send_string("client message to server2")
    msg = socket.recv_string()
    print(msg)
    time.sleep(1)

def pairclient(port = "5556"):
    context = zmq.Context()
    socket = context.socket(zmq.PAIR)
    socket.connect("tcp://localhost:%s" % port)
    n = 6
    while n:
        msg = socket.recv_string()
        print(msg)
        socket.send_string("client message to server1")
        socket.send_string("client message to server2")
        time.sleep(1)
        n-=1

def main():

    Process(target= server, args=(server_port,)).start()
    Process(target=client, args=(server_ports,)).start()
