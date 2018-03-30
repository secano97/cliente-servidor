#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <random>
#include "load.hh"

using namespace std;

// GLOBALS
uint avail_films = 10; //17770; // movies amount
uint avail_users = 2649429;     // users amount
uint avail_centroids = 2;	      // centroids amount

double eucli_dist(Matrix<double>& old_cent,Matrix<double>& new_cent){
	/* This will calculate euclidian distance between two matrix */
	double val = 0.0;
	for(uint i=0; i<old_cent.numRows(); i++){
		for(uint j=0; j<old_cent.numCols(); j++){
			double old_rate = old_cent.at(i,j);
			double new_rate = new_cent.at(i,j);
			val += pow((old_rate - new_rate),2);
		}
	}
	return sqrt(val);
}

int main(int argc, char *argv[]){
	if(argc != 2){
		cerr << "Usage {" << argv[0] << " filename.txt}\n";
		return -1;
	}
	/* ----------- phase 1 loading info into memory ----------- */
	Matrix <cont>dataset;
	load_data(argv[1],avail_users,dataset);
	// dataset.print_dic();

	/* ----------- phase 2 building initial centroids ----------- */
	Matrix <double>centroids(avail_centroids, avail_films);
	centroids.fill_like_num();
	//centroids.print_num();

	while(true){
		/* ----------- phase 3 building similarity sets ----------- */
		Matrix <ulist>similarity(avail_centroids);
		dataset.cos_simil(centroids,similarity);
		//similarity.print_list();

		/* ----------- phase 4 finding new centroids ----------- */
		Matrix <double>new_centroids(avail_centroids,avail_films);
		similarity.find_media(dataset,new_centroids);
		//new_centroids.print_num();

		/* ----------- phase 5 euclidian distance between two centroids ----------- */
		double eucli_dis_val = eucli_dist(centroids,new_centroids);
		cout << "Current euclidian distance value = " << eucli_dis_val << "\n";
		if(eucli_dis_val < 1.0) break;
		centroids = move(new_centroids);
	}

	return 0;
}
