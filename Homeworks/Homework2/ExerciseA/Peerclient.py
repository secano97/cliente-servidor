import zmq
import pyaudio
import threading
import math
import sys
import os

#RECORD GLOBALS
CHUNK = 1024
FORMAT =pyaudio.paInt16
CHANELS = 2
RATE = 44100 # what is it ? 
RECORD_SECONDS = 0.3

p = pyaudio.PyAudio()

#RECEIVE

context = zmq.Context()
socket_RECV = context.socket(zmq.REP)

#SEND

context = zmq.Context()
socket_SEND = context.socket(zmq.REQ)


def send_data(stream):
	frames = []
	for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
		data = stream.read(CHUNK)
		frames.append(data)
	data = b"".join(frames)
	return data
    

def threadOfSend():
	stream = p.open(format=FORMAT,
	                channels=CHANELS,
	                rate=RATE,
	                input=True,
	                frames_per_buffer=CHUNK)
	while True:
		data = send_data(stream)
		socket_SEND.send(data)
		msg = socket_SEND.recv_json()
		pass
	pass


def receive_data():

	stream = p.open(format=FORMAT,
	                channels=CHANELS,
	                rate=RATE,
	                output=True,
	                frames_per_buffer=CHUNK)
	while True:
		data = socket_RECV.recv(CHUNK)#EMPEZAR A reproducir
		stream.write(data)
		socket_RECV.send_json({"respuesta" : "holi"})


def menu():
	# MENU:

	ip_server = input("Ip of the server?: ")
	port_server = input("Port of the server?: ")

	ip_mine = input("Ip recv?: ")
	port_mine = input("Port recv?: ")

	#Receive

	socket_RECV.bind("tcp://{}:{}".format(ip_mine,port_mine))
	print ("escuchando por  {} en {}" .format(ip_mine, port_mine))

	#Send

	socket_SEND.connect("tcp://{}:{}".format(ip_server,port_server))
	print ("conectado al servidor {} en {}" .format(ip_server, port_server))

	hilo1 = threading.Thread(target=threadOfSend)
	hilo2 = threading.Thread(target=receive_data)
	hilo1.start()
	hilo2.start()




if __name__== '__main__':
	menu()