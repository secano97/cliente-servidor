CC=g++ -std=c++11
VENT_PATH=$/./Distributed_kmeans/taskVentilator.cpp
WORKER_PATH=$/./Distributed_kmeans/taskWorker.cpp
SINK_PATH=$/./Distributed_kmeans/taskSink.cpp

all: vent worker sink

vent: $(VENT_PATH)
	${CC} ${VENT_PATH} -o task-vent.bin -fopenmp -lzmq -lzmqpp

worker: $(WORKER_PATH)
	${CC} ${WORKER_PATH} -o task-worker.bin -fopenmp -lzmq -lzmqpp

sink: $(SINK_PATH)
	${CC} ${SINK_PATH} -o task-sink.bin -fopenmp -lzmq -lzmqpp
