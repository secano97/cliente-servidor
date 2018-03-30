CC=g++ -std=c++11
SEQUENTIAL_PATH=$/./sequential-version/kmeans.cpp
PARALLEL_PATH=$/./parallel-version/parallel-kmeans.cpp

all: sequential_version parallel_version

sequential_version: $(SEQUENTIAL_PATH)
	${CC} ${SEQUENTIAL_PATH} -o kmeans.bin

parallel_version: $(PARALLEL_PATH)
	${CC} ${PARALLEL_PATH} -o parallel-kmeans.bin -fopenmp
