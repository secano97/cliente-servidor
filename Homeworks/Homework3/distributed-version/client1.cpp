#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <random>
#include "load.hh"


using namespace std;
using namespace zmqpp;
using dmat = Matrix<double>;
using ulmat = Matrix<ulist>;

uint avail_films = 17770+1; 		// movies amount
uint avail_users = 2649429+1;   // users amount
uint standard_dev_range = 5;				// variance range
uint avail_centroids = 0;

typedef string str;

// these two fields will have server ip and port
str ip="localhost", port="5000";


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
							cent_norm,vector<double>& similarities_summary) {
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
		for(uint user_id=0; user_id < dataset.numRows(); user_id++) {
			uint temp_cent_id = 0;
			double temp_simil_val = numeric_limits<double>::max();

			for(uint cent_id = 0; cent_id < centroids.numRows(); cent_id++) {
				double Ai_x_Bi = 0.0;

				for(auto& movie : users[user_id]) {
					double cent_rate = centroids.at(cent_id,movie.first);
					double user_rate = movie.second;
					Ai_x_Bi += cent_rate * user_rate;
				}

				double similarity_value = acos( Ai_x_Bi/(cent_norm[cent_id] * \
																	users_norm[user_id]) );
				if(similarity_value < temp_simil_val) {
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

			similarities_summary[temp_cent_id] += temp_simil_val;
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

double individual_similarity(uint cent_id,uint user_id,const mat& dataset,\
													const dmat& centroids){
	/* it will calculate similarity between an user and a centroid */
	const vector<cont>& users = dataset.get_cont();
	double Ai_x_Bi = 0.0, val1 = 0.0, val2 = 0.0;
	for(auto& movie : users[user_id]) {
		double user_rate = movie.second;
		double cent_rate = centroids.at(cent_id,movie.first);
		Ai_x_Bi += user_rate * cent_rate;
		val1 += pow(user_rate,2);
		val2 += pow(cent_rate,2);
	}

	return acos(Ai_x_Bi/(sqrt(val1) * sqrt(val2)));
}

void modify_cent(uint current_cent_id,const mat& dataset, dmat& centroids,\
								vector<double>& cent_norm, ulmat& similarity, \
								vector<double>& similarities_summary) {
	/* it will modify a centroid slightly */

	// ----- finding a centroid with the greater users set than others -----
	vector<uint> upper_cent_id;
	upper_cent_id.resize(4);
	double dchunk = (double)centroids.numRows()/4;
	uint chunk = ceil(dchunk);

	const vector<ulist>& users_set = similarity.get_cont();
	#pragma omp parallel shared(centroids,users_set,upper_cent_id,chunk) \
					num_threads(4)
	{
		uint cent_set_size = 0;
		#pragma omp for schedule(dynamic,chunk) nowait
		for(uint cent_id = 0; cent_id < centroids.numRows(); cent_id++) {
			size_t set_size = users_set[cent_id].size();
			if(set_size > cent_set_size) {
				cent_set_size = set_size;
				upper_cent_id[omp_get_thread_num()] = cent_id;
			}
		}
	}

	uint sel_cent_id = 0, cent_set_size = 0;
	for(auto& cent_id : upper_cent_id)
		if(users_set[cent_id].size() > cent_set_size) {
			cent_set_size = users_set[cent_id].size();
			sel_cent_id = cent_id;
		}

	// -selecting an user from the greater users set and substracting similarity-
	uint sel_user_id = similarity.get_rand_item_id(sel_cent_id);
	double sim = individual_similarity(sel_cent_id,sel_user_id,dataset,centroids);
	similarities_summary[sel_cent_id] -= sim;

	// ----- moving through selected centroid and modifying slightly -----
	vector<double> results;
	results.resize(4,0.0);
	dchunk = (double)centroids.numCols()/4;
	chunk = ceil(dchunk);

	#pragma omp parallel shared(current_cent_id,centroids,chunk) num_threads(4)
	{
		#pragma omp for schedule(dynamic,chunk) nowait
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
			results[omp_get_thread_num()] += pow(movie_rate,2);
		}
	}

	double value = 0.0;
	for(auto& result : results)
		value += result;

	cent_norm[current_cent_id] = sqrt(value);
	similarity.fill_like_list(current_cent_id,sel_user_id);

	// --- calculating similarity between generated centroid and selected user ---
	sim = individual_similarity(current_cent_id,sel_user_id,dataset,centroids);
	similarities_summary[current_cent_id] += sim;
}

void check_empt_cent(const mat& dataset,dmat& centroids,vector<double>& \
											cent_norm,ulmat& similarity,\
											vector<double>& similarities_summary) {
	/* it will check if exist an empty centroid, then modigy it with modify_cent*/
	for(uint cent_id=0; cent_id < centroids.numRows() ; cent_id++)
		if(!cent_norm[cent_id])
			modify_cent(cent_id,dataset,centroids,cent_norm,similarity,\
									similarities_summary);

}

double standard_deviation(vector<double>& errors) {
	/* it will calculate standard deviation */
	double X = 0.0;
	for(auto& Xi : errors)
		X += Xi;
	X /= errors.size();

	double summary = 0.0;
	for(auto& Xi : errors)
		summary += pow( (Xi - X),2);

	return sqrt(summary/errors.size());
}

double print_result(const ulmat& similarity,const vector<double>& \
								 similarities_summary) {
	/* it will print centroids with theirs nearest users */
	double total_summary = 0.0;
	for(uint cent_id=0; cent_id < similarity.numRows(); cent_id++) {
		cout <<"["<< cent_id << " : " << similarity.get_set_size(cent_id)<<"]" \
				 << " <------------> " << similarities_summary[cent_id] << "\n";
		total_summary += similarities_summary[cent_id];
	}
	cout << "Total similarity value is = " << total_summary << "\n\n";
	return total_summary;
}

double calculate_SSD(const ulmat& similarity,const vector<double>& \
								 similarities_summary) {
	/* it will return the total summary of the similarities between each user and his centroid */
	double total_summary = 0.0;
	for(uint cent_id=0; cent_id < similarity.numRows(); cent_id++) {
		total_summary += similarities_summary[cent_id];
	}
	return total_summary;
}

double make_work(uint k, const mat& dataset) { // principal function of kmeans
  avail_centroids = k;	    // centroids amount k

  vector<double> users_norm, cent_norm;
	cent_norm.resize(avail_centroids);
	users_norm.resize(dataset.numRows());
	get_users_norm(dataset,users_norm);

	/* ----------- phase 2 building initial centroids ----------- */
	dmat centroids(avail_centroids, avail_films);
	centroids.fill_like_num();
	get_cent_norm(centroids,cent_norm);
	//centroids.print_num();

	vector<double> errors;
	errors.resize(standard_dev_range);
	uint current_error = 0, iteration = 1;
	bool _exit = false;

	Timer timer;
	while(true){
		vector<double> similarities_summary;
		similarities_summary.resize(avail_centroids,0.0);
		/* ----------- phase 3 building similarity sets ----------- */
		ulmat similarity(avail_centroids);
		dmat new_centroids(avail_centroids,avail_films);
		cos_simil(dataset,centroids,new_centroids,similarity,users_norm,cent_norm,\
							similarities_summary);
		//similarity.print_list();

		/* ----------- phase 4 cosine similraty between two centroids ----------- */
		get_cent_norm(new_centroids,cent_norm);
		check_empt_cent(dataset,new_centroids,cent_norm,similarity, \
										similarities_summary);
		double similarity_val = cent_simil(centroids,new_centroids,similarity);

		errors[current_error] = similarity_val;
		current_error++;
		if(current_error > errors.size()-1)
			current_error = 0;

		cout << "--------------------------------------------------" << "\n";
		if(iteration >= errors.size()) {
			double standard_deviation_val = standard_deviation(errors);
			cout << "Standard deviation = " << standard_deviation_val << "\n";
			if(standard_deviation_val < 0.022)
				_exit = true;
		}

		cout << "Current similarity = " << similarity_val << "\n";
		if(similarity_val < 0.01 || _exit) {
			double SSD = print_result(similarity,similarities_summary);
      return SSD;
			break;
		}
		cout << "--------------------------------------------------" << "\n\n";
		centroids = move(new_centroids);
		iteration++;
	}
	cout << "Transcurred seconds = " <<	(double)timer.elapsed()/1000 << endl;

}


int main(int argc, char const *argv[]) {
  if(argc != 2){
		cerr << "Usage {" << argv[0] << " filename.txt}\n";
		return -1;
	}
	/* ----------- phase 1 loading info into memory ----------- */
	mat dataset;
	load_data(argv[1],avail_users,dataset);
  // making context and server socket
  context ctx;
  socket c(ctx, socket_type::req); // socket_type = requester

  // connecting to server
  c.connect("tcp://"+ip+":"+port);
  message req, reply;

  // decomposing message package
  str rpl = "none";

  while(rpl != "There are no more work!") {
    // making message package
    req << "give me work";

    // seding message package and waiting for server reply
    cout << "Requesting work to the server!\n";
    c.send(req);
    c.receive(reply);

    // decomposing reply
    reply >> rpl;

    // if there is work I will do it
    if(rpl != "There are no more work!" && \
       rpl != "Result was saved successfully!" && \
		 	 rpl != "wait a moment") {
      uint k = stoi(rpl);
      double ssd = make_work(k, ref(dataset));

      // delivering work done
      req << "deliver result" << to_string(k) << to_string(ssd);
      c.send(req);
      c.receive(reply);

      // decomposing reply
      reply >> rpl;
    }
	else if(rpl == "wait a moment"){
		this_thread::sleep_for(chrono::seconds(2));
	}

    // showing decomposed message
    cout << rpl<<"\n";
  }

  return 0;
}
