#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <chrono>

using namespace std;
using namespace zmqpp;
typedef string str;

// these two fields will have server ip and port
str ip="localhost", port="5000";

uint make_work(uint k) {
  /* it will make some work */
  this_thread::sleep_for(chrono::seconds(2));
  return rand() % 700;
}


int main(int argc, char const *argv[]) {
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
       rpl != "Result was saved successfully!") {
      uint k = stoi(rpl);
      uint ssd = make_work(k);

      // delivering work done
      req << "deliver result" << to_string(k) << to_string(ssd);
      c.send(req);
      c.receive(reply);

      // decomposing reply
      reply >> rpl;
    }

    // showing decomposed message
    cout << rpl<<"\n";
  }

  return 0;
}
