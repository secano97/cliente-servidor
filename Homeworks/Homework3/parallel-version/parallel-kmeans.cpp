#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <random>
#include "load.hh"

using namespace std;
using c_dmat = const Matrix<double>;
using dmat = Matrix<double>;
using ulmat = Matrix<ulist>;
using cmat = Matrix<cont>;

// GLOBALS
uint avail_films = 10; //17770; // movies amount
uint avail_users = 2649429;     // users amount
uint avail_centroids = 2;	      // centroids amount

void cos_simil(cmat& dataset,c_dmat& centroids,ulmat& similarity){
	/* This will calculate the cosain similarity between this and centroids */
	double dchunk = (double)dataset.numRows()/4;
	uint chunk = ceil(dchunk);

	#pragma omp parallel shared(dataset,centroids,similarity,chunk) num_threads(4)
	{

		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint user_id=0; user_id < dataset.numRows(); user_id++){
			uint temp_cent_id = 0;
			double temp_simil_val = numeric_limits<double>::max();

			for(uint cent_idx = 0; cent_idx < centroids.numRows(); cent_idx++){
				double Ai_x_Bi = 0.0, Ai2 = 0.0, Bi2 = 0.0;

				for(uint movie_id=0; movie_id < centroids.numCols(); movie_id++){
					double cent_rate = centroids.at(cent_idx,movie_id);
					Ai2 += pow(cent_rate,2);
				}

				for(auto& movie : dataset.data[user_id]) {
					double cent_rate = centroids.at(cent_idx,movie.first);
					double user_rate = movie.second;
					Ai_x_Bi += cent_rate * user_rate;
					Bi2 += pow(user_rate,2);
				}

				double similarity_value = Ai_x_Bi/(sqrt(Ai2) * sqrt(Bi2));
				if(similarity_value < temp_simil_val){
					temp_simil_val = similarity_value;
					temp_cent_id = cent_idx;
				}

			}

			similarity.fill_like_list(temp_cent_id,user_id);
		}
	}
}

void find_media(ulmat& similarity,cmat& dataset,dmat& new_centroids){
	/* This will calculate media between users into one set from similarity */
	double dchunk = (double)dataset.numRows()/4;
	uint chunk = ceil(dchunk);

	#pragma omp parallel shared(similarity,dataset,new_centroids,chunk) num_threads(4)
	{

		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id=0 ;cent_id < new_centroids.numRows(); cent_id++){
			for(uint movie_id=0; movie_id < new_centroids.numCols(); movie_id++){
				double user_rate_summary = 0.0;
				for(auto& user_id : similarity.data[cent_id])
					user_rate_summary += dataset.user_movie_rate(user_id,movie_id);
				double media = user_rate_summary / similarity.data[cent_id].size();
				double &d = new_centroids.at(cent_id,movie_id);
				d = media;
			}
		}
	}
}

double eucli_dist(dmat& old_cent,dmat& new_cent){
	/* This will calculate euclidian distance between two matrix */
	vector<double> thread_values;
	thread_values.resize(4);
	double dchunk = (double)old_cent.numRows()/4;
	uint chunk = ceil(dchunk);

	#pragma omp parallel shared(thread_values,old_cent,new_cent,chunk) num_threads(4)
	{
		double value = 0.0;
		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint i=0; i<old_cent.numRows(); i++){
			for(uint j=0; j<old_cent.numCols(); j++){
				double old_rate = old_cent.at(i,j);
				double new_rate = new_cent.at(i,j);
				value += pow((old_rate - new_rate),2);
			}
		}
		thread_values[omp_get_thread_num()] = value;
	}

	double total = 0.0;
	for(auto& value : thread_values)
		total += value;

	return sqrt(total);
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
		cos_simil(dataset,centroids,similarity);
		//similarity.print_list();

		/* ----------- phase 4 finding new centroids ----------- */
		Matrix <double>new_centroids(avail_centroids,avail_films);
		find_media(similarity,dataset,new_centroids);
		//new_centroids.print_num();

		/* ----------- phase 5 euclidian distance between two centroids ----------- */
		double eucli_dis_val = eucli_dist(centroids,new_centroids);
		cout << "Current euclidian distance value = " << eucli_dis_val << "\n";
		if(eucli_dis_val < 1.0) break;
		centroids = move(new_centroids);
	}

	return 0;
}
