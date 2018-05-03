#include <zmqpp/zmqpp.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
//#include <math.h>
#include <cmath>

uint desired_work = 8; // Number of ks

using namespace std;
using namespace zmqpp;

struct Elbow{
  double priority;
  uint k;

public:
  Elbow(double _pr, uint _k){
    priority = _pr;
    k = _k;
  }
};

class cmpFunction{
public:
  double operator()(const Elbow &a, const Elbow &b){
    return a.priority < b.priority;
  }
};

typedef string str;

using pqueue = priority_queue<Elbow, vector<Elbow>, cmpFunction>;
using dvector = vector<double>; //vector of doubles
using svector = vector<str>; // vector of strings

/************GLOBALS*******************/
condition_variable cv_vec, cv_qe, cv_vec_res;
bool can_work_vec = true, can_work_qe = true, can_work_vec_res = true;
mutex fork_vec, fork_qe, fork_vec_res;
/*************************************/



// The ip field will has server ip belonging to the machine in a network
// The port field will has any desired port.
str ip = "*", port = "5000";

// Put the work quantity that you want

double modify_results(dvector& results,uint k, double ssd, uint opt){
  double result = -1.0;

  unique_lock<mutex> lock(fork_vec_res);
  cv_vec_res.wait(lock,[]{return can_work_vec_res;});
  can_work_vec_res = false;
  // ----- entering critical zone -----
  if(opt){//modify
    results[k-1] = ssd;
   }
  else{
    result = results[k-1];
  }
  can_work_vec_res = true;
  cv_vec_res.notify_one();
  // ----- ending critical zone -----

  return result;
}

str modify_avail(svector& availability, str state,uint k, uint opt) {
  str answer = "n";

  unique_lock<mutex> lock(fork_vec);
  cv_vec.wait(lock,[]{return can_work_vec;});
  can_work_vec = false;
  // ----- entering critical zone -----
  if(opt){
    availability[k-1] = state;
  }
  else{
    answer = availability[k-1];
  }
  can_work_vec = true;
  cv_vec.notify_one();
  // ----- ending critical zone -----
  return answer;
}

void modify_queue(pqueue& work,double priority,uint k, uint opt) {
  unique_lock<mutex> lock(fork_qe);
  cv_qe.wait(lock,[]{return can_work_qe;});
  can_work_qe = false;
  // ----- entering critical zone -----
  if(opt){ // push
    work.push(Elbow(priority,k));
    //cout << "size = " << work.size() << endl;
  }
  else
    work.pop();
  can_work_qe = true;
  cv_qe.notify_one();
  // ----- leaving critical zone -----
}

uint assign_work(svector& availability, pqueue& work){
  Elbow ie = work.top();
  modify_queue(work,0.0,0,0);
  str r = modify_avail(availability,"c",ie.k,1);
  return ie.k;
}

void listen(socket& s, pqueue& work, dvector& results, svector& availability) {
  /* it will put to server listen */


  message cliReq, reply;

  while(true) {

    // waiting for a client's request
    s.receive(cliReq);

    // decomposing client's request
    str req_type, part1, part2;
    cliReq >> req_type;

    // checking which request type a client made
    if(req_type == "give me work" && !work.empty()) {

      uint k = assign_work(availability, work);
      reply << to_string(k);
    }
    else if(req_type == "deliver result"){
      cliReq >> part1 >> part2;
      uint k = stoi(part1);
      double ssd = stod(part2);

      double res = modify_results(results,k,ssd,1);// control of concurrence
      str r = modify_avail(availability,"cd",k,1);//control of concurrence
      cout << "Result by worker with k = "<< k << " and ssd = " << ssd << "\n";
      reply << "Result was saved successfully!";
    }

    else if(req_type == "give me work" && work.empty()){
      reply << "wait a moment";
    }
    else
      reply << "There are no more work!";

    // replying to client's request
    s.send(reply);
  }


}

int there_are_any_behind(int i, dvector& results){
  int k_behind = -1;
  double res = 0;
  for(int b = i-1; b >= 0; b--){
    res = modify_results(results,b+1,0.0,0);
    //cout << "this is the result of  the consult behind: " << res << endl;
    if(res != -1){
      k_behind = b+1;
    //  cout << "k_behind: " << k_behind << endl;
      break;
    }
  }
  return k_behind;
}

int there_are_any_after(int i, dvector& results){
  double res = 0;
  int k_after = -1;
  for(int a = i+1; a <= results.size() - 1 ; a++){
    res = modify_results(results,a+1,0.0,0);
    //cout << "this is the result of  the consult after: " << res << endl;
    if(res != -1){
      k_after = a+1;
      //cout << "k after: " << k_after << endl;
      break;
    }
  }
  return k_after;
}

double slope(const int& k1, const int& k2, dvector& results){

  double y2 = modify_results(results,k2,0.0,0);// the -1 'cause pos[0] = ssd of  k = 1
  double y1 = modify_results(results,k1,0.0,0);
  double p = (y2-y1)/(k2-k1);
  return p;
}

double rad_to_degrees(double& a ){
  double ang = -1.0;
	double degreesPerRadian = 57.2957;

  double rad = a * degreesPerRadian;

  if(rad < 0)
		ang = 360 + rad;
	if(rad > 0)
		ang = rad;
	if(rad == 0 )
		ang = 360;

	return ang;
}

bool middle_is_correct(const int& k1, const uint& middle){
  if (middle == k1){
    return false; // it doesn't has a middle
  }
  else
    return true; // it has a middle
}

bool is_available(const uint& k, svector& availability){
  str r = modify_avail(availability,"n",k,0);
  if(r == "D")
    return true;
  else
    return false;
}

void push_in_queue(const uint& k, double& priority, pqueue& work,\
                    svector& availability){
  cout << "i'm in the push_in_queue with k: " << k <<" and priority: " \
       << priority << endl;

  str r = modify_avail(availability,"Q",k,1); // control of concurrence
  modify_queue(work,priority,k,1);
}

double get_angle_change(const int& k1, const int& k2, const int& k3,\
  pqueue& work, dvector& results, svector& availability){
    cout << "i'm inside of angle change" << endl;
    double rad1 = atan(slope(k1, k2, results));
    double rad2 = atan(slope(k2, k3, results));

    double degree1 = rad_to_degrees(rad1);
    cout << "degree1: "<< degree1 <<endl;
    double degree2 = rad_to_degrees(rad2);
    cout << "degree2: "<< degree2 <<endl;

    double angular_change = abs(degree1 - degree2); // priority quit this maybe

    uint middle1 = (floor((k2-k1)/2)) + k1; // middle of the first straight
    cout << "middle1 is: " << middle1 << endl;
    uint middle2 = (floor((k3-k2)/2)) + k2; // middle of the second straight
    cout << "middle2 is: " << middle2 << endl;

    bool correct1 = middle_is_correct(k1,middle1);
    bool correct2 = middle_is_correct(k2,middle2);

    if(correct1){
      cout <<"middle 1 is correct" << endl;
      if(is_available(middle1, availability)){
        push_in_queue(middle1, angular_change, work, availability);
      }
    }

    if(correct2){
      cout <<"middle 2 is correct" << endl;
      if(is_available(middle2, availability)){
        push_in_queue(middle2, angular_change, work, availability);
      }
    }

    if(!correct1 && !correct2){
      return -2.0; //ya encontró el k
    }
    else
    return angular_change;

  }

int main(int argc, char const *argv[]) {


  pqueue work; // making priority_queue

  // making work for clients
  dvector results;     // vector of doubles
  svector availability;// vector of strings
  results.resize(desired_work, -1.0);
  availability.resize(desired_work,"D");

  int midle = floor((desired_work - 1)/2) + 1;
  double first_priority = 30.0;

  push_in_queue(1,first_priority, work, availability);
  push_in_queue(midle,first_priority, work, availability);
  push_in_queue(desired_work,first_priority, work, availability);


  // making context and server socket
  context ctx;
  socket s(ctx,socket_type::rep); // socket_type = replayer
  // binding ip and port, whose that will be used by the server
  s.bind("tcp://"+ip+":"+port);

  // cliReq object will has client request, reply will has the server reply \
     package
  thread t(listen,ref(s),ref(work),ref(results),ref(availability));


  // From here we will execute the elbow method

  Elbow best_k = Elbow(-1.0,0); // this variable will save the best k
  bool find_k = true;
  while(find_k){
    //cout << "i'm in the while" << endl;
    size_t k1 = 0 ; //a elbow is compused for 2 rects, it's mean 3 points \
                    (k = x)
    size_t k2 = 0 ;
    size_t k3 = 0 ;
    double elbow_priority = 0.0;

    for(int i = 1; i <= results.size() - 2; i++ ){
      //cout << "i'm in the for" << endl;
      if(results[i] != -1 ){
        k2 = i+1;
        //cout << "this is the k2: " << k2 << endl;
        k1 = there_are_any_behind(i,results);
        k3 = there_are_any_after(i,results);
        if( k1 != -1 && k3 != -1){
          cout << "this is the k1: " << k1 << endl;
          cout << "this is the k2: " << k2 << endl;
          cout << "this is the k3: " << k3 << endl;
          elbow_priority = get_angle_change(k1, k2,\
                              k3,work,results,availability);
          if(elbow_priority == -2.0){// ya encontró el k
              cout << "the best k is: " << best_k.k << '\n';
              find_k = false;
              break;
          }
          else if (best_k.priority < elbow_priority){
              best_k.k = k2;
              best_k.priority = elbow_priority;
          }
        }
      }
    }
  }

  //cout << "An error has ocurred!\n";
  t.join();
  return 0;
}
