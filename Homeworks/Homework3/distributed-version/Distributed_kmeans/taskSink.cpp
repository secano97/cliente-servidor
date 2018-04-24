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

  //  Prepare our socket to receive the ssd
  zmqpp::socket receiver(context, zmqpp::socket_type::pull);
  receiver.bind("tcp://*:5558");

  //  Recieving the ssd value
  zmqpp::message ssd;
  receiver.receive(ssd);
  cout << "The ssd sended from worker was: " << ssd.get(0);
  cout << endl;

  //  Start our clock now
  struct timeval tstart;
  gettimeofday (&tstart, NULL);

  //  Calculate and report duration of batch
  struct timeval tend, tdiff;
  gettimeofday (&tend, NULL);

  if (tend.tv_usec < tstart.tv_usec) {
      tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
      tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
  }
  else {
      tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
      tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
  }
  double total_msec = tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;
  cout << "\nTotal elapsed time: " << total_msec << " msec\n" << endl;

}

int main(int argc, char const *argv[]) {
  taskSink();
  return 0;
}
