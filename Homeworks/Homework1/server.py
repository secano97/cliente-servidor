import zmq
import sys
import math
import os

def loadFiles(path):
    files = {}
    dataDir = os.fsencode(path)
    for file in os.listdir(dataDir):
        filename = os.fsdecode(file)
        print ("loading {}".format(filename))
        files[filename] = files
    return files

def main():
    if len(sys.argv) != 3:
        print ("Error !")
        exit()
    directory = sys.argv[2]
    port = sys.argv[1] #server import

    context =zmq.Context()
    s = context.socket(zmq.REP)
    s.bind("tcp://*:{}".format(port))

    files = loadFiles(directory)

    while True:
        msg = s.recv_json()
        if msg ["op"] == "list":
            s.send_json({"files": list(files.keys())})
        elif msg["op"] == "download":
            filename = msg ["file"]
            if filename in files:
                with open(directory + filename, "rb") as input:
                    #numberOfParts = str(math.ceil(len(data)/(1024*1024))) # Numero de partes 
                    #s.send(numberOfParts)


                    conter = msg["parte"]# contador de partes 
                    data = input.seek(conter*1024*1024)
                    realData = input.read(1024*1024)
                    s.send(realData)


        elif msg ["op"] == "parte":
            filename = msg ["file"]
            if filename in files:
                with open(directory + filename, "rb") as input:
                    #data = input.read()
                    data = os.path.getsize(directory)
                    datafinal = str(math.ceil(len(data)/(1024*1024)))
                    s.send_json({"partes":datafinal})

            else:
                print ("jaja!")
                s.send_string("")
        else:
            print ("error!")



    print (files)

if __name__ == '__main__':
    main()

#modificacion, los archivos ya no se van a compartir totalmente, seran por partes, 1 mb, añadir nuevo caso de uso, preguntar al servidor cuantas partes tiene un archivo, (tamaño del archivo y dividirlo por 1024, y redondearlo al numero siguiente) download bye.mp3 parte 2. ("part":1)


# manejo de archivos en parte para enviar en varias partes un archivo y enviarlo. 
#enviar mensajes, recibir mensajes python.
