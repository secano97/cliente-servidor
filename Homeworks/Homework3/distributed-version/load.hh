#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "parallel-mat.hh"

using namespace std;
using mat = Matrix<cont>;        // setting an alias mat (matrix)

void pass_data(mat& M1, mat& M2){
  /* it will pass data between M1 and M2 */
    uint user_id = 0;
    const vector<cont>& unnormalized_data = M1.get_cont();
    for(auto& user : unnormalized_data){
      if(user.empty()) continue;
      for(auto& movie : user)
        M2.fill_like_dic(user_id,movie.first,movie.second);
      user_id++;
    }
}

void load_data(str fileName,const size_t& avail_users,mat& dataset){
  /* this function will load dataset into memory */
  ifstream file(fileName);        // open file
  mat temp(avail_users); // building dataset of type container
  str line;                       // one line will be loaded into line object
  uint movie_id = 0;              // movie id

  while(getline(file,line)){      // getting line per line from file.txt
    istringstream iss(line);      // making possible put the line into a container
    uint last_pos = line.size()-1;// determining last_pos of line (last character)
    if(line[last_pos] == ':'){    // if we find ':' this means movie id
      iss >> movie_id;            // get the movie id
      continue;                   // continue with the next line
    }
    uint user_id, rating;         // user id and rating
    char comma;                   // comma character
    iss >> user_id >> comma >> rating;// withdrawing relevant items from iss container
    temp.fill_like_dic(user_id,movie_id,rating);
    // dataset is a vector that contains dictionaries with movie id and rating
    // vector position is the user id, movie id is linked with rating number
  }

  file.close();                   // closing file
  size_t real_avail_users = temp.real_avail_users();
  cout << "Users that at least rated a movie = " << real_avail_users << endl;
  dataset.resize(real_avail_users);
  pass_data(temp,dataset);
}
