#include <zmqpp/zmqpp.hpp>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

using namespace std;

// initialize the 0MQ context
zmqpp::context context;

void taskSink() {

  //  Prepare our socket to receive the value of ssd
  zmqpp::socket receiver(context, zmqpp::socket_type::pull);
  receiver.bind("tcp://*:5558");

  //  Prepare our socket to send the value of ssd to the ventilator
  zmqpp::socket sender(context, zmqpp::socket_type::push);
  sender.connect("tcp://localhost:5556");

  //  Recieving the ssd value from workers
  zmqpp::message ssd;
  receiver.receive(ssd);
  cout << "The ssd sent from worker was: " << ssd.get(0);
  cout << endl;

  // Sending the ssd value to ventilator
  char k_ssd [10];
  sprintf (k_ssd, "%d", stoi(ssd.get(0)));
  cout << "Sending the ssd value: " << k_ssd << endl;
  sender.send(k_ssd);
}

int main(int argc, char const *argv[]) {
  taskSink();
  return 0;
}
