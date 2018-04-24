#include <zmqpp/zmqpp.hpp>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <queue>
#include <vector>
#include <functional>
#include <utility>
#include <list>
#include <iterator>

using namespace std;

// Initialize the 0MQ context
zmqpp::context context;

// Generate a push socket type
zmqpp::socket_type type = zmqpp::socket_type::push;
zmqpp::socket socket (context, type);

// Initialize a counter
int cont = 0;

class SSD
{
public:
	int priority;
	int k_number;

	SSD(int priority, int k_number)
		: priority(priority), k_number(k_number)
	{

	}
};

struct SSDCompare
{
	bool operator()(const SSD &t1, const SSD &t2) const
	{
		int t1value = t1.priority * 1000 + t1.k_number;
		int t2value = t2.priority * 1000 + t2.k_number;
		return t1value < t2value;
	}
};

// Get a k_ssd
// int getSSD(int k) {
// 	list<int> ssd_list {30, 23, 20, 18, 15};
// 	auto ssd_list_front = ssd_list.begin();
// 	advance(ssd_list_front, k);
// 	// cout << *ssd_list_front << '\n';
// 	return *ssd_list_front;
// }

// Get three (2) values of k and throw the best k.
int getBestK(int prev_k, int next_k) {
	int best_k = next_k/2;
	cout << " Value of k: "<< best_k;
	cout << endl;
	return best_k;
}

int get_k_priority(int k, double ssd) {
  // double slope1 = 0; // Pendiente 1
	// double slope2 = 0; // Pendiente 2
	// double slope_change = 0; //(cambio de pendiente)

	//slope = (y2-y1)/(x2-x1)
	// slope1 = (prev_kssd - actual_kssd)/(prev_k - actual_k);
	// slope2 = (next_kssd - actual_kssd)/(next_k - actual_k);
	// slope_change = slope1 - slope2;

	// // Condition to set the best k with its priority.
	// if (/* condition */) {
	// 	/* code */
	// }
	// else{
	//
	// }
}

void taskVentilator() {

  //  Socket to send messages on
  zmqpp::socket sender(context, type);
  sender.bind("tcp://*:5557");

  cout << "Press Enter when the workers are ready: " << endl;
  getchar ();
  cout << "Sending tasks to workers…\n" << endl;

  // //  The first message is "0" and signals start of batch
  // zmqpp::socket sink(context, type);
  // sink.connect("tcp://localhost:5558");
  // zmqpp::message message;
  // //The next 2 lines shows the way to send a message.
  // message << "0";
  // sink.send(message);

  //  Initialize random number generator
  srandom ((unsigned) time (NULL));

  // Sending the value of k to the workers
  char string [10];
  int work;
  work = getBestK(1, 10);
  sprintf (string, "%d", work);
  cout << "Sending best k: " << string <<endl;
  sender.send(string);

  }
  // cout << "Total expected cost: " << total_msec << " msec" << endl;
  // sleep (1);              //  Give 0MQ time to deliver

int main(int argc, char const *argv[]) {
  taskVentilator();
  return 0;
}
