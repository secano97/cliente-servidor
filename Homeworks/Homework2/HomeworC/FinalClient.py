import zmq
import pyaudio
import threading
import math
import sys
import os
import time

#RECORD GLOBALS
CHUNK = 5120
FORMAT =pyaudio.paInt16
CHANELS = 2
RATE = 20100 # what is it ?
RECORD_SECONDS = 0.3

p = pyaudio.PyAudio()

#CONTEXT
context = zmq.Context()

#RECEIVE
socket_RECV = context.socket(zmq.REP)

#SEND
socket_SEND = context.socket(zmq.REQ)



def capture_data(stream):
	frames = []
	for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
		data = stream.read(CHUNK)
		frames.append(data)
	data = b"".join(frames)
	return data


def threadOfSend(ident):
	print("entre al hilo 1")
	hilo2 = threading.Thread(target=receive_data)
	hilo2.start()

	stream = p.open(format=FORMAT, channels=CHANELS, rate=RATE, input=True, frames_per_buffer=CHUNK)

	while True:
		data = capture_data(stream)
		socket_SEND.send_multipart([bytes("send",'ascii'),bytes(ident,'ascii'),data])
		msg = socket_SEND.recv_json()
		if msg ["respuesta"] == "ok":
			print("ok")
		else:
			print("Error! in execute the send function")
			print(msg["respuesta"])
			time.sleep(5)
			sys.exit(0)



def receive_data():
	stream = p.open(format=FORMAT,
	                channels=CHANELS,
	                rate=RATE,
	                output=True,
	                frames_per_buffer=CHUNK)
	while True:
		#print("entre al while")
		data = socket_RECV.recv(CHUNK)
		#audio = data[0]#.decode('ascii')
		stream.write(data)
		socket_RECV.send_json({"respuesta" : "received"})


def menu():
	# MENU:

	ip_server = input("Ip of the server?: ")
	port_server = input("Port of the server?: ")

	ip_mine = input("Ip recv?: ")
	port_mine = input("Port recv?: ")

	ident = input("your username for the chat?: ")
	#dest = input("the username of the person which you wanna talk: ")

	#Receive

	socket_RECV.bind("tcp://{}:{}".format(ip_mine,port_mine))
	print ("escuchando por  {} en {}" .format(ip_mine, port_mine))

	#Send

	socket_SEND.connect("tcp://{}:{}".format(ip_server,port_server))
	print ("conectado al servidor {} en {}" .format(ip_server, port_server))

	#socket_SEND.send_json({"op" :"inscription","ident" : ident, "myIp": ip_mine, "port" :port_mine})
	socket_SEND.send_multipart([bytes("inscription",'ascii'),bytes(ident,'ascii'),bytes(ip_mine,'ascii'),bytes(port_mine,'ascii')])

	msg = socket_SEND.recv_json()

	if msg ["respuesta"] == "ok":
		print(" el servido recibio mi ip y mi puerto de escucha ")
	else:
		print("Error! in the inscription")
		time.sleep(5)
		sys.exit(0)

	#PRUEBA DE SOCKET DE ESCUCHA
	'''
	k = socket_RECV.recv_json()
	print(k["prueba"])

	if k ["prueba"] == "connected":
		#print("socket of listen up")
		socket_RECV.send_json({"respuesta" : "ok"})
	else:
		#print("socket of listen down")
		socket_RECV.send_json({"respuesta" : "no"})
	'''

	socket_SEND.send_multipart([bytes("to",'ascii')])

	msgto = socket_SEND.recv_json()

	while msgto["respuesta"] != "ok":
		print("preguntando si hay alguien mas")
		#print("asking for: {}".format(dest))
		socket_SEND.send_multipart([bytes("to",'ascii')])
		msgto = socket_SEND.recv_json()
		#print (msgto["respuesta"])

	#print("sali del ciclo")
	hilo1 = threading.Thread(target=threadOfSend (ident, ))
	#print("hilo1")


if __name__== '__main__':
	menu()
