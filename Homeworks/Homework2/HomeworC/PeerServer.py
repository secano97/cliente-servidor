import zmq
import sys
import math
import os

CHUNK = 1024

context = zmq.Context()
socket_RECV = context.socket(zmq.REP)

Dicitionary_of_connections = {}

def broadcast(ident, audio):
	connections = Dicitionary_of_connections.keys()

	for connection in connections:
		if ident != connection:
			Dicitionary_of_connections[connection].send(audio)
			k = Dicitionary_of_connections[connection].recv_json()
		else:
			pass

def main():

	ip_for_listen = input("ip for listen?: ")
	port_for_listen = input("port for listen?: ")

	socket_RECV.bind("tcp://{}:{}".format(ip_for_listen,port_for_listen))
	print ("escuchando por  {} en {}" .format(ip_for_listen,port_for_listen))

	while True:
		msg = socket_RECV.recv_multipart()
		if  msg[0].decode('ascii') == "inscription":
			ident = msg[1].decode('ascii')
			ipClient = msg[2].decode('ascii')
			portClient= msg[3].decode('ascii')

			socket_Send = context.socket(zmq.REQ)
			socket_Send.connect("tcp://{}:{}".format(ipClient,portClient))

			Dicitionary_of_connections.update({ident:socket_Send})

			if ident in Dicitionary_of_connections:
				socket_RECV.send_json({"respuesta":"ok"})
			else:
				socket_RECV.send_json({"respuesta":"no"})

			# PRUEBA  DE SOCKET DE ESCUCHA CONECTADO
			'''
			socket_Send.send_json({"prueba" : "connected"})
			k = socket_Send.recv_json()

			if k ["respuesta"] == "ok":
				print("socket of listen up")
			else:
				print("socket of listen down")
			'''


		elif msg[0].decode('ascii') == "send":
			ident = msg[1].decode('ascii')
			audio = msg[2]#.decode('ascii')

			k = broadcast(ident,audio)

			socket_RECV.send_json({"respuesta" : "ok"})

		elif msg[0].decode('ascii') == "to":
			print("entrando en la comprobación")
			print(len(Dicitionary_of_connections))
			if len(Dicitionary_of_connections) > 1:
				socket_RECV.send_json({"respuesta":"ok"})
			else:
				socket_RECV.send_json({"respuesta":"no"})
		else:
			print ("error!")


if __name__ == '__main__':
    main()
