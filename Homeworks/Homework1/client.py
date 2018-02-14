import zmq
import sys

def main():
    if len(sys.argv) != 4:
        print ("Error !")
        exit()
    ip = sys.argv[1] #server ip
    port = sys.argv[2] #server import
    operation = sys.argv[3] #operation to perform

    context = zmq.Context()
    s = context.socket(zmq.REQ)

    s.connect("tcp://{}:{}".format(ip, port))

    if operation == "list":
        s.send_json({"op": "list"})
        files = s.recv_json()
        print (files)
    elif operation == "download":
        name = input("File to download:")
        s.send_json({"op":"parte", "file":name})

        partes = s.recv_json()

        parteEntero= int(partes["partes"])

        print (" the number of parts of the file is: " , parteEntero)
        conter = 0

        with open ("partes.mp3","wb") as output:
            while conter <= parteEntero:
                s.send_json({"op":"download", "file":name, "parte":conter})
                file = s.recv()
                output.write(file)
                conter += 1
                pass

        #parte = input ("ingrese la parte:")
        #s.send_json({"op":"download", "file":name, "parte":parte})
        #file = s.recv()
        #with open ("descarga.algo", "wb") as output:
        #    output.write(file)

########################################################################
    elif operation == "parte":
        name = input("of what file do you know the parts?: ")
        s.send_json({"op":"parte", "file":name})
        file = s.recv_json()
        print (file)






    else:
        print ("error !! unsupport")

    print ("conectando al servidor {} en {}" .format(ip, port))

if __name__== '__main__':
    main()
