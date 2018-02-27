import zmq
import sys
import math
import os

CHUNK = 1024

context = zmq.Context()
socket_RECV = context.socket(zmq.REP)

Dicitionary_of_connections = {}

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
			dest = msg[1].decode('ascii')

			#print("estoy enviando mensajes a: {} ".format(dest))

			audio = msg[2]#.decode('ascii')
			#print("sending audio")
			Dicitionary_of_connections[dest].send(audio)
			#print(socket)
			
			#socket.send(audio)
			print(" waiting for msg")
			k = Dicitionary_of_connections[dest].recv_json()
			print("end of the sending")
			socket_RECV.send_json({"respuesta" : "lol"})

		elif msg[0].decode('ascii') == "to":
			dest = msg[1].decode('ascii')

			print("Asked me for: {} ".format(dest))

			if dest in Dicitionary_of_connections:
				socket_RECV.send_json({"respuesta":"ok"})
			else:
				socket_RECV.send_json({"respuesta":"no"})


		else:
			print ("error!")


if __name__ == '__main__':
    main()