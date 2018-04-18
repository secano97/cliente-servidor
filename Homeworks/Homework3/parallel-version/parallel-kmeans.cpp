#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <random>
#include "load.hh"

using namespace std;
using dmat = Matrix<double>;
using ulmat = Matrix<ulist>;

uint avail_films = 17770+1; 		// movies amount
uint avail_users = 2649429+1;   // users amount
uint avail_centroids = 100;	    // centroids amount

void get_cent_norm(const dmat& centroids,vector<double>& cent_norm){
	/* it will calculate all centroids norm */
	double dchunk = (double)centroids.numRows()/4;
	uint chunk = ceil(dchunk);

	#pragma omp parallel shared(centroids,cent_norm,chunk) num_threads(4)
	{

		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id=0; cent_id < centroids.numRows(); cent_id++){
			double value = 0.0;
			for(uint movie_id=1; movie_id <= centroids.numCols(); movie_id++)
				value += pow(centroids.at(cent_id,movie_id),2);
			cent_norm[cent_id] = sqrt(value);
		}
	}

}

void get_users_norm(const mat& dataset,vector<double>& users_norm){
	/* it will calculate all users norm */
	double dchunk = (double)dataset.numRows()/4;
	uint chunk = ceil(dchunk);

	const vector<cont>& users = dataset.get_cont();
	#pragma omp parallel shared(dataset,users_norm,users,chunk) num_threads(4)
	{
		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint user_id=0; user_id < dataset.numRows(); user_id++) {
			double value = 0.0;
			for(auto& movie : users[user_id])
				 value += (double)pow(movie.second,2);
			users_norm[user_id] = sqrt(value);
		}
	}

}

double cent_simil(const dmat& old_centroids,const dmat& new_centroids, \
									const ulmat& similarity){
	/* it will calculate cosine similarity between old_cent and new_cent */
	vector<double> results;
	results.resize(old_centroids.numRows());
	double dchunk = (double)old_centroids.numRows()/4;
	uint chunk = ceil(dchunk);
	const vector<ulist>& users_set = similarity.get_cont();

	#pragma omp parallel shared(old_centroids,new_centroids,results,users_set,chunk) \
												num_threads(4)
	{
		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id = 0; cent_id < old_centroids.numRows(); cent_id++){
			double Ai_x_Bi = 0.0, val1 = 0.0, val2 = 0.0;
			for(uint movie_id = 1; movie_id <= old_centroids.numCols(); movie_id++ ){
				double old_cent_rate = old_centroids.at(cent_id,movie_id);
				double new_cent_rate = new_centroids.at(cent_id,movie_id);
				val1 += pow(old_cent_rate,2);
				val2 += pow(new_cent_rate,2);
				Ai_x_Bi += old_cent_rate * new_cent_rate;
			}

			double angle = acos(Ai_x_Bi/(sqrt(val1) * sqrt(val2)));
			results[cent_id] = angle;;
		}
	}

	double similarity_val = 0.0;
	for(auto& data : results)
		similarity_val += data;

	return similarity_val/old_centroids.numRows();
}

void cos_simil(const mat& dataset,const dmat& centroids,dmat& new_centroids, \
							ulmat& similarity,vector<double>& users_norm,vector<double>& \
							cent_norm){
	/* This will calculate the cosain similarity between centroids and users */
	const vector<cont>& users = dataset.get_cont();
	dmat users_rate(avail_centroids,avail_films);
	double dchunk = (double)dataset.numRows()/4;
	uint chunk = ceil(dchunk);

	omp_lock_t writelock;
	omp_init_lock(&writelock);
	#pragma omp parallel shared(dataset,centroids,new_centroids,similarity,\
		cent_norm,users_norm,users,users_rate,chunk) num_threads(4)
	{

		#pragma omp for schedule(dynamic,chunk) nowait
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

				double similarity_value = acos( Ai_x_Bi/(cent_norm[cent_id] * \
																	users_norm[user_id]) );
				if(similarity_value < temp_simil_val){
					temp_simil_val = similarity_value;
					temp_cent_id = cent_id;
				}
			}

			/* ------ users rate summary by parts ------ */
			omp_set_lock(&writelock);
			for(auto& movie : users[user_id]){
				double& movie_rate = new_centroids.at(temp_cent_id,movie.first);
				double& users =  users_rate.at(temp_cent_id,movie.first);
				movie_rate += movie.second;
				users+=1;
			}

			similarity.fill_like_list(temp_cent_id,user_id);
			omp_unset_lock(&writelock);
		}

	}
	omp_destroy_lock(&writelock);

	dchunk = (double)centroids.numRows()/4;
	chunk = ceil(dchunk);
	#pragma omp parallel shared(centroids,new_centroids,users_rate,chunk) \
												num_threads(4)
	{
		/* ------ averaging users rate ------ */
		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id=0; cent_id< centroids.numRows(); cent_id++ ){
			for(uint movie_id=1; movie_id<=centroids.numCols(); movie_id++){
				double& movie_rate = new_centroids.at(cent_id,movie_id);
				double& users = users_rate.at(cent_id,movie_id);
				if(!users) continue;
				movie_rate /= users;
			}
		}
	}

}

// void modify_cent(uint current_cent_id,const mat& dataset,dmat& centroids, \
// 	vector<double>& cent_norm,ulmat& similarity){
// 	/* it will modify a given centroid slightly */
// 	uint upper_cent_id = 0, upper_cent_size = 0;
// 	const vector<ulist>& users_set = similarity.get_cont();
//
// 	// ----- finding a centroid with the greater users set than others -----
// 	for(uint cent_id=0; cent_id< centroids.numRows(); cent_id++) {
// 		size_t set_size = users_set[cent_id].size();
// 		if(set_size > upper_cent_size) {
// 			upper_cent_size = set_size;
// 			upper_cent_id = cent_id;
// 		}
// 	}
//
// 	// ----- selecting an user from the greater users set -----
// 	uint sel_user_id = similarity.get_rand_item_id(upper_cent_id);
//
// 	// ----- passing data between selected user as new centroid -----
// 	double value = 0.0;
// 	const vector<cont>& users = dataset.get_cont();
//
// 	for(auto& movie : users[sel_user_id]) {
// 		 double& movie_rate = centroids.at(current_cent_id,movie.first);
// 		 double rate =  movie.second;
// 		 movie_rate = rate;
// 		 value += pow(rate,2);
// 	}
//
//  	cent_norm[current_cent_id] = sqrt(value);
// 	similarity.fill_like_list(current_cent_id,sel_user_id);
// }

void modify_cent(uint current_cent_id, dmat& centroids,vector<double>& \
								cent_norm, ulmat& similarity){
	/* it will modify a centroid slightly */

	// ----- finding a centroid with the greater users set than others -----
	uint sel_cent_id = 0, cent_size = 0;
	const vector<ulist>& users_set = similarity.get_cont();

	for(uint cent_id=0; cent_id< centroids.numRows(); cent_id++) {
		size_t set_size = users_set[cent_id].size();
		if(set_size > cent_size) {
			cent_size = set_size;
			sel_cent_id = cent_id;
		}
	}

	// ----- selecting an user from the greater users set -----
	uint sel_user_id = similarity.get_rand_item_id(sel_cent_id);
	double value = 0.0;

	for(uint movie_id=1; movie_id <= centroids.numCols(); movie_id++) {
		double movie_rate = centroids.at(sel_cent_id,movie_id);
		double temp_movie_rate = movie_rate;
		movie_rate += 0.5;
		if(movie_rate > 5.0){
			movie_rate = temp_movie_rate;
			movie_rate -= 0.5;
		}
		double& old_movie_rate = centroids.at(current_cent_id,movie_id);
		old_movie_rate = movie_rate;
		value += pow(movie_rate,2);
	}

	cent_norm[current_cent_id] = sqrt(value);
	similarity.fill_like_list(current_cent_id,sel_user_id);
}

void check_empt_cent(const mat& dataset,dmat& centroids,vector<double>& \
											cent_norm,ulmat& similarity){
	/* it will check if exist an empty centroid, then raplaced it with modify cent */
	for(uint cent_id=0; cent_id < centroids.numRows() ; cent_id++)
		if(!cent_norm[cent_id])
			modify_cent(cent_id/*,dataset*/,centroids,cent_norm,similarity);

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
	mat dataset;
	load_data(argv[1],avail_users,dataset);
	// dataset.print_dic();

	vector<double> users_norm, cent_norm;
	cent_norm.resize(avail_centroids);
	users_norm.resize(dataset.numRows());
	get_users_norm(dataset,users_norm);

	/* ----------- phase 2 building initial centroids ----------- */
	dmat centroids(avail_centroids, avail_films);
	centroids.fill_like_num();
	get_cent_norm(centroids,cent_norm);
	//centroids.print_num();

	Timer timer;
	while(true){
		/* ----------- phase 3 building similarity sets ----------- */
		ulmat similarity(avail_centroids);
		dmat new_centroids(avail_centroids,avail_films);
		cos_simil(dataset,centroids,new_centroids,similarity,users_norm,cent_norm);
		//similarity.print_list();

		/* ----------- phase 4 cosine similraty between two centroids ----------- */
		get_cent_norm(new_centroids,cent_norm);
		check_empt_cent(dataset,new_centroids,cent_norm,similarity);
		double similarity_val = cent_simil(centroids,new_centroids,similarity);
		cout << "Current similarity = " << similarity_val << "\n";
		if(similarity_val < 0.01) {
			print_result(similarity);
			break;
		}
		centroids = move(new_centroids);
	}
	cout << "Transcurred seconds = " <<	(double)timer.elapsed()/1000 << endl;

	return 0;
}
