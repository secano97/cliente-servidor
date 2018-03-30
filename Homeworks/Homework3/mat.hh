#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <list>
#include <thread>
#include <cmath>
#include <limits>
#include "timer.hh"

using namespace std;
using ulist = list<uint>;
using cont = map<uint,uint>;
typedef string str;

template <class T>
class Matrix {
private:
  vector<T> data;
  size_t rows;
  size_t cols;

public:
  Matrix(size_t r, size_t c) {
    rows = r;
    cols = c;
    data.resize(r*c,0.0);
  }

  Matrix(const size_t& total){
    data.resize(total);
  }

  Matrix(){}

  size_t numRows() const { return rows; }
  size_t numCols() const { return cols; }

  void resize(const size_t& new_size){
    data.resize(new_size);
  }

  void clear(){
    data.clear();
  }

/* -------------------- K-means section -------------------- */

  void cos_simil(const Matrix<double>& centroids,Matrix<ulist>& similarity){
  	/* This will calculate the cosain similarity between this and centroids */
    uint user_id = 0;
    for(auto& user : data){
      uint temp_cent_id = 0;
      double temp_simil_val = numeric_limits<double>::max();

      for(uint cent_idx = 0; cent_idx < centroids.numRows(); cent_idx++){
        double Ai_x_Bi = 0.0, Ai2 = 0.0, Bi2 = 0.0;

        for(uint movie_id=0; movie_id < centroids.numCols(); movie_id++){
          double cent_rate = centroids.at(cent_idx,movie_id);
          Ai2 += pow(cent_rate,2);
        }

        for(auto& movie : user){
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
      user_id ++;
    }
  }

  void find_media(Matrix<cont>& dataset,Matrix<double>& new_centroids){
    /* This will calculate media between users into one set from similarity */
    for(uint cent_id=0 ;cent_id < new_centroids.numRows(); cent_id++){
      for(uint movie_id=0; movie_id < new_centroids.numCols(); movie_id++){
        double user_rate_summary = 0.0;
        for(auto& user_id : data[cent_id])// User list of a respective centroid
          user_rate_summary += dataset.user_movie_rate(user_id,movie_id);
        double media = user_rate_summary / data[cent_id].size();
        double &d = new_centroids.at(cent_id,movie_id);
        d = media;
      }
    }
  }

/* ------------------ list management section ------------------ */
  void fill_like_list(uint cent_id,uint user_id){
    /* it will fill vector of list */
    data[cent_id].push_front(user_id);
  }

  void print_list(){
    /* This function will print vector list content */
    uint set_id = 0;
    str op;
    for(auto& similarity_set : data){
      cout << "<<<<<<< Similarity set id = " << set_id <<" >>>>>>>\n";
      for(auto& user_id : similarity_set)
        cout << user_id << " - ";
      cout << "\n\nPress enter to continue ...\n\n";
      getline(cin,op);
      set_id ++;
    }
  }

/* ------------------ dictionary management section ------------------ */

uint user_movie_rate(uint user_id, uint movie_id){
  /* This will search and return user movie rate */
  auto movie = data[user_id].find(movie_id);
  if(movie != data[user_id].end())
    return movie->second;
  return 0;
}

  void pass_data(Matrix<cont>& M){
    /* this will pass data from this matrix to M */
    uint user_id = 0;
    for(auto& user : data){
      if(user.empty()) continue;
      for(auto& movie : user)
        M.fill_like_dic(user_id,movie.first,movie.second);
      user_id++;
    }
  }

  size_t real_avail_users(){
    /* This will determine all real avail users */
    size_t real_avail_users = 0;
    for(auto& user : data)
      if(!user.empty()) real_avail_users ++;
    return real_avail_users;
  }

  void fill_like_dic(uint user_id,uint movie_id,uint rating){
    /* This will fill vector of dictionaries */
    data[user_id].insert(pair<uint,uint>(movie_id,rating));
  }

  void print_dic(){
    /* This function will print vector dic content */
    uint user_id = 0;
    string op;
    for(auto user : data){
      cout << " <<<<<<< User ID = " << user_id <<" <<<<<<< \n";
      for(auto& movie : user)
        cout << "[" << movie.first << ":" << movie.second <<"]\n";
      cout << "\nPress enter to continue ...\n\n";
      getline(cin,op);
      user_id++;
    }
  }

/* ------------------ Mumber management section ------------------ */

  double at(size_t i, size_t j) const {
    /* This will access to matrix's cell by value */
    size_t idx = cols * i + j;
    return data[idx];
  }

  double &at(size_t i, size_t j) {
    /* This will access to matrix's cell by reference */
    size_t idx = cols * i + j;
    return data[idx];
  }

  bool operator==(const Matrix &rhs) const {
    /* this will check if two matrix are equal */
    if (rows != rhs.rows)
      return false;
    if (cols != rhs.cols)
      return false;
    for (size_t i = 0; i < data.size(); i++)
      if (data[i] != rhs.data[i])
        return false;
    return true;
  }

  void identity() {
    /* this will fill matrix diagonal with 1.0 */
    for (size_t r = 0; r < rows; r++)
      for (size_t c = 0; c < cols; c++)
        if (r == c)
          at(r, c) = 1.0;
  }

  void fill_like_num() {
    /* This will fill matrix with numbers */
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(1.0, 5.0);
    for (size_t i = 0; i < data.size(); i++)
      data[i] = dis(gen);
  }

  void print_num() const {
    /* This will print matrix with numbers*/
    for (size_t r = 0; r < rows; r++) {
      for (size_t c = 0; c < cols; c++) {
        cout << " " << at(r, c);
      }
      cout << endl;
    }
    cout << endl;
  }
};
