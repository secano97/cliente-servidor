#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <random>
#include "load.hh"

using namespace std;
using dmat = Matrix<double>;
using ulmat = Matrix<ulist>;
using cmat = Matrix<cont>;

uint avail_films = 17770+1; 		// movies amount
uint avail_users = 2649429+1;   // users amount
uint avail_centroids = 2;	      // centroids amount

void cos_simil(const cmat& dataset,const dmat& centroids,ulmat& similarity){
	/* This will calculate the cosain similarity between centroids and users */
	double dchunk = (double)dataset.numRows()/4;
	uint chunk = ceil(dchunk);

	omp_lock_t writelock;
	omp_init_lock(&writelock);
	#pragma omp parallel shared(dataset,centroids,similarity,chunk) num_threads(4)
	{

		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint user_id=0; user_id < dataset.numRows(); user_id++){
			uint temp_cent_id = 0;
			double temp_simil_val = numeric_limits<double>::max();

			for(uint cent_idx = 0; cent_idx < centroids.numRows(); cent_idx++){
				double Ai_x_Bi = 0.0, Ai2 = 0.0, Bi2 = 0.0;

				for(uint movie_id=1; movie_id <= centroids.numCols(); movie_id++){
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

			omp_set_lock(&writelock);
			similarity.fill_like_list(temp_cent_id,user_id);
			omp_unset_lock(&writelock);
		}
	}
	omp_destroy_lock(&writelock);
}

void find_media(const ulmat& similarity,const cmat& dataset,dmat& new_centroids){
	/* This will calculate media between users into one set from similarity */
	double dchunk = (double)dataset.numRows()/4;
	uint chunk = ceil(dchunk);

	#pragma omp parallel shared(similarity,dataset,new_centroids,chunk) num_threads(4)
	{

		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id=0 ;cent_id < new_centroids.numRows(); cent_id++){
			if(!similarity.get_set_size(cent_id)) continue;
			for(uint movie_id=1; movie_id <= new_centroids.numCols(); movie_id++){
				double user_rate_summary = 0.0;
				for(auto& user_id : similarity.data[cent_id])
					user_rate_summary += (double)dataset.user_movie_rate(user_id,movie_id);
				double media = user_rate_summary / (double)similarity.get_set_size(cent_id);
				double &d = new_centroids.at(cent_id,movie_id);
				d = media;
			}
		}
	}
}

double eucli_dist(const dmat& old_cent,const dmat& new_cent){
	/* This will calculate euclidian distance between two matrix */
	vector<double> thread_values;
	thread_values.resize(4);
	double dchunk = (double)old_cent.numRows()/4;
	uint chunk = ceil(dchunk);

	#pragma omp parallel shared(thread_values,old_cent,new_cent,chunk) num_threads(4)
	{
		double value = 0.0;
		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id=0; cent_id<old_cent.numRows(); cent_id++){
			for(uint movie_id=1; movie_id<=old_cent.numCols(); movie_id++){
				double old_rate = old_cent.at(cent_id,movie_id);
				double new_rate = new_cent.at(cent_id,movie_id);
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

void print_result(const ulmat& similarity){
	for(uint cent_id=0; cent_id < similarity.numRows(); cent_id++)
		cout << cent_id << " : " << similarity.get_set_size(cent_id) << "\n";
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

	Timer timer;
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
		if(eucli_dis_val < 1.0){
			print_result(similarity);
			break;
		}
		centroids = move(new_centroids);
	}
	cout << "Transcurred seconds = " <<	(double)timer.elapsed()/1000 << endl;

	return 0;
}
