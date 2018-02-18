import zmq 
import time 
import sys 
from multiprocessing import Process
"""
hfjimenez@utp.edu.co, using zmq official documentation
There is a posibility to run a client and server using the same file, 
we could use multiprocessing
"""

def server(port="8000"):
    context = zmq.Context()
    socket = context.socket(zmq.REP) #Will reply in  each request
    socket.bind("tcp://*:{}".format(port))
    print('Server is Running at port:{}'.format(port))
    for reqnum in range(5):
        message = socket.recv_string()
        print('Received request #:{} with message:{}'.format(reqnum,message))
        socket.send_string('Reply from server #:{}'.format(reqnum))
        
def client(ports=["8000"]):
    context = zmq.Context()
    socket = context.socket(zmq.REQ) #Will reply in  each request
    print('Trying to connect to server with ports #:{}'.format(ports))
    
    for port in ports: 
        socket.connect("tcp://localhost:{}".format(port))

    for request in range(20):
        #Will loose to request 
        print('Client: Sending request #:{}'.format(request))
        socket.send_string("allo")
        message = socket.recv()
        print("Received reply ", request, "[", message, "]")
        time.sleep (1) 

def main():
    server_ports = range(8000,8008,2)
    for server_port  in server_ports: 
        Process(target= server, args=(server_port,)).start()
    
    Process(target=client, args=(server_ports,)).start()

if __name__ == '__main__':
    main()