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
    cols = c-1;
    data.resize(r*c,0.0);
  }

  Matrix(const size_t& total){
    rows = total;
    data.resize(total);
  }

  Matrix(){
    rows = 0;
    cols = 0;
  }

  size_t numRows() const { return rows; }
  size_t numCols() const { return cols; }

  size_t get_set_size(uint set_id) const {
    return data[set_id].size();
  }

  const vector<T>& get_cont() const {
    return data;
  }

  void resize(const size_t& new_size){
    rows = new_size;
    data.resize(new_size);
  }

  void clear(){
    data.clear();
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

uint user_movie_rate(uint user_id, uint movie_id) const {
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
