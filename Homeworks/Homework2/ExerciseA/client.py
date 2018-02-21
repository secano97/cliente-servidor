import argparse
import zmq
import os
import pyaudio
import wave
import _thread

from pynput.keyboard import Key, Listener
from src.helpers.utils import *

# CONSTANTS SECTION
PORTS = []
WAVE_OUTPUT_FILENAME = "voice.wav"
ID_USERINFO = {}  # User information
CLIENT_LPORT = 4000

def receive_data():
    while True:
        try:
            # data = s.recv(CHUNK)      #Send socket 
            receive_stream.write(data)
        except:
            pass


def send_data():
    while True:
        try:
            data = send_stream.read(CHUNK)
            #s.sendall(data)        #send socket sender
        except:
            pass


def main():
    """ Client of the Chatvoice application
        Evil-Labs, UTP
    By default, this python file will start the development server 
    on the internal IP at a give port, if not port is supplied port 4000 will be
    used.

    Args:
          --port: An integer representing your Port on the localhost.  
          --ip_address: be considered to be IPv4 by default 0.0.0.0 or localhost.
    Returns:
        An open socket/threated connection, listening on port parameter and ipv4 address.
    Raises:
        ValueError: if there are too many connections or reach the 
        limit of 30 concurrent clients, Raise a phthreat error, given by the 
        zmq address
    """
    # Implement a Python arg parser using the standard lib
    parser = argparse.ArgumentParser(
        description=bannerc(),
        prog='ChatVoice-Server',
        epilog="Written by Hector F. Jimenez and Sebastian Cano"
    )
    parser.add_argument(
        'raddress', help="Remote Ipv4 server socket.")
    parser.add_argument(
        'rport', help="Remote server port to connect.")
    parser.add_argument(
        'username', help="identify yourself against the server")
    parser.add_argument('--otherusername',
                        help="Insert the name of your friend")
    parser.add_argument('--ping',
                        help="Test your client and server connection. You should get an Ok acknowledgement")
    args = parser.parse_args()
    # Creates a 0mq context, from docs You should create and use exactly one context
    # in your process. Technically, the context is the container for all sockets
    # in a single process
    context = zmq.Context()
    REMOTE_PORT = int(args.rport)
    REMOTE_IP = args.raddress
    ports = [REMOTE_PORT, CLIENT_LPORT]
    # Create two different sockets
    listener_client = context.socket(zmq.REP)                   # Listen data
    sender_client = context.socket(zmq.REQ)                     # Send data to server
    print('{} Setting Networking Context and Address Family{}'.format(ok, reset))
    # sleep(1)
    try:
        # All client will be the same, but every client should be in a
        # different ip, and using different ports
        listener_client.bind("tcp://*:{}".format(ports[1]))
        #print('{} Client is Listening on Port: {}{} {}for incomming voice message'.format(
        #    ok, bold, ports[1], reset))
        print('{} Trying to contact server {}:{}'.format(atn,REMOTE_IP, REMOTE_PORT))
        sender_client.connect("tcp://{}:{}".format(REMOTE_IP, REMOTE_PORT))
        #Send an initial connection to the server 
        #sender_client.send_json({"cmdquery": "hello"})
        
        #ToDo: Confirm Connection with server, something like an ack


    except zmq.error.ZMQError as e:
        print('{} Failed to create a Socket in Port {}, due to {}'.format(
            err, REMOTE_PORT, e))
    p = pyaudio.PyAudio()
    receive_stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, output=True, frames_per_buffer=CHUNK)
    send_stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True, frames_per_buffer=CHUNK)
    print("Voice chat running")
    _thread.start_new_thread(receive_data, ())
    _thread.start_new_thread(send_data, ())

    while True:
        pass

if __name__ == '__main__':
    main()
