#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <random>
#include <list>
#include <vector>
#include <math.h>
#include <tuple>
// #include "boost/tuple/tuple.hpp"

using namespace std;
//using boost::tuple;

typedef vector< tuple<int,double> > tuple_list;
typedef vector< tuple<int,int> > int_tuple_list;

double calcInertia(int num_actual_cluster, list<double> distances_to_centroid) {
  double inertia;
  double total_distance = 0;
  for (size_t i = 0; i < num_actual_cluster; i++) {
     double subtotal_distance = 0;
     for (auto n : distances_to_centroid) {
       subtotal_distance += n;
     }
     total_distance += subtotal_distance;
  }
  inertia = total_distance;
  return inertia;
}

void addInertiaTuple(tuple_list inertia_clusters, int num_actual_cluster, double actual_inertia_cluster) {
  inertia_clusters.push_back(tuple<int, double>(num_actual_cluster, actual_inertia_cluster));
}

//Divide the range in 3 parts.
void krange(int k_rangeLow, int k_rangeHigh, int_tuple_list optRangeList) {
  int dividedRange = (k_rangeHigh/3);
  auto optRange1 = make_tuple(k_rangeLow, dividedRange);
  auto optRange2 = make_tuple((dividedRange + 1), (dividedRange*2));
  auto optRange3 = make_tuple(((dividedRange*2) + 1), k_rangeHigh);

  optRangeList[0] = optRange1;
  optRangeList[1] = optRange1;
  optRangeList[2] = optRange1;
}

int main() {
  tuple_list inertia_clusters;
  int num_cluster; //Number of clusters
  double cluster_inertia; //Inertia for the actual number of clusters
// -------------------------------------------------------------
  int_tuple_list work_range;

  //To iterate on the tuple list -->
  //   for (tuple_list::const_iterator i = tl.begin(); i != tl.end(); ++i) {
  //       cout << "Age: " << i->get<0>() << endl;
  //       cout << "Name: " << i->get<1>() << endl;
  //   }
}
