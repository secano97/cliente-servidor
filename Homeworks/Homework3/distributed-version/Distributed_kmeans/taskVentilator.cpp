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
#include <cmath>

using namespace std;

// Initialize the 0MQ context
zmqpp::context context;

// Generate a push socket type
zmqpp::socket_type type = zmqpp::socket_type::push;
zmqpp::socket socket (context, type);

// Initialize the k vector.
vector<int> k_vector;

// Initialize the ssd vector.
vector<int> ssd_vector;

// Define the counter
int counter = 0;

class IncAngle
{
public:
	int priority;
	int k_number;

	IncAngle(int priority, int k_number)
		: priority(priority), k_number(k_number)
	{

	}
};

struct IncAngleCompare
{
	bool operator()(const IncAngle &t1, const IncAngle &t2) const
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

// Get two (2) values of k and throw the best k.
int getBestK(int prev_k, int actual_k) {
	int best_k = actual_k/2;
	cout << " Value of k: "<< best_k;
	cout << endl;
	return best_k;
}

// Takes two (points), the last and actual to calculate the slope and then calculate arctan(slope) to obtain the inclination angle. Also by this way define the priority.
int get_k_priority(int prev_k, int prev_ssd, int actual_k, int actual_ssd) {
	// slope = (y2-y1)/(x2-x1)
	double slope = 0.0;
	double inclination_angle = 0.0;

	// Calculating the slope and the inclination angle of the slope.
	slope = ((prev_ssd - actual_ssd)/(prev_k - actual_k));
	inclination_angle = atan(slope);

	// Returning the inclination angle as the priority
	return inclination_angle;
}

void taskVentilator() {

  //  Socket to send tasks to workers
  zmqpp::socket sender(context, type);
  sender.bind("tcp://*:5557");

  //  Socket to receive the result from sink
  zmqpp::socket receiver(context, zmqpp::socket_type::pull);
  receiver.bind("tcp://*:5556");

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

	// ******The following lines are going to be in a loop******
  // Sending the value of k to the worker
  char string [10];
  int work;
  work = getBestK(1, 10);
	k_vector.push_back(work); // Adding the k that will be sent to the workers into a vector.
  sprintf (string, "%d", work);
  cout << "Sending best k: " << string <<endl;
  sender.send(string);

	//  Recieving the ssd value from sink
  zmqpp::message ssd;
  receiver.receive(ssd);
  cout << "The ssd sent from sink was: " << ssd.get(0);
  cout << endl;
	ssd_vector.push_back(stoi(ssd.get(0))); // Adding the value of the ssd sent from sink into a vector.

	// If the ssd was received add 1 to the counter to keep track of the points (k, ssd).
	if (stoi(ssd.get(0)) != 0) {
		counter = counter + 1;
	}

	// If the value of counter is even(par) means that I got 2 points to use the priority function.
	if (counter % 2 != 0) {
		double actual_k_priority = 0.0;
		actual_k_priority = get_k_priority(k_vector[counter-1], ssd_vector[counter-1], k_vector[counter], ssd_vector[counter]);

		// Defining the priority queue.
		IncAngle IncAngle1(actual_k_priority, k_vector[counter]);

		// Setting up the queue.
		priority_queue<IncAngle, vector<IncAngle>, IncAngleCompare> queue;

		// Adding the prioritis with its k to the queue.
		queue.push(IncAngle1);
	}

}

int main(int argc, char const *argv[]) {
  taskVentilator();
  return 0;
}
