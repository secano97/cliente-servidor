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

uint avail_films = 17770+1;			// movies amount
uint avail_users = 2649429+1;   // users amount
uint avail_centroids = 5;	      // centroids amount

void get_cent_norm(const dmat& centroids,vector<double>& cent_norm){
	/* it will calculate all centroids norm */
	cent_norm.resize(centroids.numRows());

	for(uint cent_id=0; cent_id < centroids.numRows(); cent_id++){
		double value = 0.0;
		for(uint movie_id=1; movie_id <= centroids.numCols(); movie_id++)
			value += pow(centroids.at(cent_id,movie_id),2);
		cent_norm[cent_id] = sqrt(value);
	}

}

void get_users_norm(const cmat& dataset,vector<double>& users_norm){
	/* it will calculate all users norm */
	users_norm.resize(dataset.numRows());
	uint user_id = 0;

	const vector<cont>& users = dataset.get_cont();
	for(auto& user : users){
		double value = 0.0;
		for(auto& movie : user)
			 value += (double)pow(movie.second,2);
		users_norm[user_id] = sqrt(value);
		user_id ++;
	}

}

void cos_simil(const cmat& dataset,const dmat& centroids,dmat& new_centroids, ulmat& similarity){
	/* This will calculate the cosain similarity between centroids and users */
	vector<double> cent_norm, users_norm;
	get_cent_norm(centroids,cent_norm);
	get_users_norm(dataset,users_norm);
	const vector<cont>& users = dataset.get_cont();

	for(uint user_id=0; user_id < dataset.numRows(); user_id++){
		uint temp_cent_id = 0;
		double temp_simil_val = numeric_limits<double>::min();

		for(uint cent_id = 0; cent_id < centroids.numRows(); cent_id++){
			double Ai_x_Bi = 0.0;

			for(auto& movie : users[user_id]) {
				double cent_rate = centroids.at(cent_id,movie.first);
				double user_rate = movie.second;
				Ai_x_Bi += cent_rate * user_rate;
			}

			double similarity_value = acos(Ai_x_Bi/(cent_norm[cent_id] * users_norm[user_id]) );
			if(similarity_value > temp_simil_val){
				temp_simil_val = similarity_value;
				temp_cent_id = cent_id;
			}

		}

		/* calculating avarage by parts */
		for(auto& movie : users[user_id]){
			double& value = new_centroids.at(temp_cent_id,movie.first);
			value = (movie.second + value) / 2.0;
		}

		similarity.fill_like_list(temp_cent_id,user_id);
	}
}

void find_media(const dmat& centroids,dmat& new_centroids){
	/*  it will find media between two centroids */
	for(uint cent_id=0; cent_id < centroids.numRows(); cent_id++){
		for(uint movie_id=1; movie_id <= centroids.numCols(); movie_id++){
			double& value = new_centroids.at(cent_id,movie_id);
			value = ( centroids.at(cent_id,movie_id) + new_centroids.at(cent_id,movie_id) )/ 2.0;
		}
	}

}

double eucli_dist(const dmat& old_cent,const dmat& new_cent){
	/* This will calculate euclidian distance between two matrix */
	double val = 0.0;
	for(uint cent_id=0; cent_id<old_cent.numRows(); cent_id++){
		for(uint movie_id=1; movie_id<=old_cent.numCols(); movie_id++){
			double old_rate = old_cent.at(cent_id,movie_id);
			double new_rate = new_cent.at(cent_id,movie_id);
			val += pow((old_rate - new_rate),2);
		}
	}
	return sqrt(val);
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
		Matrix <double>new_centroids(avail_centroids,avail_films);
		cos_simil(dataset,centroids,new_centroids,similarity);
		//similarity.print_list();

		/* ----------- phase 4 find media ----------- */
		find_media(centroids,new_centroids);
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
