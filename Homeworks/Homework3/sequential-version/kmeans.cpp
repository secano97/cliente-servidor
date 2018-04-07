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

double cent_simil(const dmat& old_centroids,dmat& new_centroids){
	/* it will calculate cosine similarity between old_cent and new_cent */

	double similarity_value = 0.0;
	for(uint cent_id = 0; cent_id < old_centroids.numRows(); cent_id++){
		double Ai_x_Bi = 0.0, val1 = 0.0, val2 = 0.0;
		for(uint movie_id = 1; movie_id <= old_centroids.numCols(); movie_id++ ){
			double old_cent_rate = old_centroids.at(cent_id,movie_id);
			double new_cent_rate = new_centroids.at(cent_id,movie_id);
			val1 += pow(old_cent_rate,2);
			val2 += pow(new_cent_rate,2);
			Ai_x_Bi += old_cent_rate * new_cent_rate;
		}

		similarity_value += acos(Ai_x_Bi/(sqrt(val1) * sqrt(val2)) );
	}

	double similarity = similarity_value / old_centroids.numRows();
	return similarity;
}

void cos_simil(const cmat& dataset,const dmat& centroids,dmat& new_centroids,\
							ulmat& similarity,vector<double>& users_norm){
	/* it will calculate the cosain similarity between centroids and users */
	vector<double> cent_norm;
	get_cent_norm(centroids,cent_norm);
	const vector<cont>& users = dataset.get_cont();
	dmat users_rate(avail_centroids,avail_films);

	for(uint user_id=0; user_id < dataset.numRows(); user_id++){
		uint temp_cent_id = 0;
		double temp_simil_val = numeric_limits<double>::max();

		for(uint cent_id = 0; cent_id < centroids.numRows(); cent_id++){
			double Ai_x_Bi = 0.0;

			for(auto& movie : users[user_id]) {
				double cent_rate = centroids.at(cent_id,movie.first);
				double user_rate = movie.second;
				Ai_x_Bi += cent_rate * user_rate;
			}

			double similarity_value = acos(Ai_x_Bi/(cent_norm[cent_id] *\
				 												users_norm[user_id]) );
			if(similarity_value < temp_simil_val){
				temp_simil_val = similarity_value;
				temp_cent_id = cent_id;
			}

		}

		/* ------ users rate summary by parts ------ */
		for(auto& movie : users[user_id]){
			double& movie_rate = new_centroids.at(temp_cent_id,movie.first);
			double& users =  users_rate.at(temp_cent_id,movie.first);
			movie_rate += movie.second;
			users+=1;
		}

		similarity.fill_like_list(temp_cent_id,user_id);
	}

	/* ------ averaging users rate ------ */
	for(uint cent_id=0; cent_id< centroids.numRows(); cent_id++ ){
		for(uint movie_id=1; movie_id<=centroids.numCols(); movie_id++){
			double& movie_rate = new_centroids.at(cent_id,movie_id);
			double& users = users_rate.at(cent_id,movie_id);
			if(!users) continue;
			movie_rate /= users;
		}
	}
}

void print_result(const ulmat& similarity){
	/* it will print centroids with theirs nearest users */
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
	vector<double> users_norm;
	get_users_norm(dataset,users_norm);
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
		cos_simil(dataset,centroids,new_centroids,similarity,users_norm);
		//similarity.print_list();

		/* ----------- phase 4 cosine similraty between two centroids ----------- */
		double similarity_val = cent_simil(centroids,new_centroids);
		cout << "Current similarity = " << similarity_val << "\n";
		if(similarity_val < 0.1){
			print_result(similarity);
			break;
		}
		centroids = move(new_centroids);
	}
	cout << "Transcurred seconds = " <<	(double)timer.elapsed()/1000 << endl;

	return 0;
}
