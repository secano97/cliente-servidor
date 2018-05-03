CC=g++ -std=c++11
ZMQ_PATH=$/usr/local
#ZMQ_PATH=/home/utp/zmq
ZMQ_INC=$(ZMQ_PATH)/include
ZMQ_LIB=$(ZMQ_PATH)/lib

all: hwserver hwclient1 hwclient2 hwclient3

hwserver: server.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) server.cpp -o server.bin -lzmq -lzmqpp -pthread
hwclient1: client1.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) client1.cpp -o client1.bin -fopenmp -lzmq -lzmqpp
hwclient2: client2.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) client2.cpp -o client2.bin -fopenmp -lzmq -lzmqpp
hwclient3: client3.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) client3.cpp -o client3.bin -fopenmp -lzmq -lzmqpp
