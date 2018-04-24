#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <utility>
#include <list>
#include <iterator>

// Para calcular la prioridad necesito la suma en el k que se está calculando, la suma del k anterior y la suma del k siguiente. Entre la suma anterior y la suma presente habría una pendiente (p1) y entre la suma presente y la suma siguiente habría una pendiente (p2). La prioridad se define por la diferencia entre p1 y p2 dependiendo del cambio de pendiente entre las 2.
// Cada prioridad va acompañado  de su k correspondiente.

using namespace std;

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
int getSSD(int k) {
	list<int> ssd_list {30, 23, 20, 18, 15};
	auto ssd_list_front = ssd_list.begin();
	advance(ssd_list_front, k);
	// cout << *ssd_list_front << '\n';
	return *ssd_list_front;
}

// Get three (3) values of k and throw the best k with its priority.
pair<int,int> getBestK(int prev_k, int actual_k, int next_k) {
	double slope1 = 0; // Pendiente 1
	double slope2 = 0; // Pendiente 2
	double slope_change = 0; //(cambio de pendiente)
	int prev_kssd = getSSD(prev_k);
	int actual_kssd = getSSD(actual_k);
	int next_kssd = getSSD(next_k);

	//slope = (y2-y1)/(x2-x1)
	slope1 = (prev_kssd - actual_kssd)/(prev_k - actual_k);
	slope2 = (next_kssd - actual_kssd)/(next_k - actual_k);
	slope_change = slope1 - slope2;

	// // Condition to set the best k with its priority.
	// if (/* condition */) {
	// 	/* code */
	// }
	// else{
	//
	// }

	pair <int,int> best_k;
	best_k.first = 5; //priority
	best_k.second = prev_k; //Value of k
	int priority = best_k.first;
	int k_value = best_k.second;
	cout << "Priority: "<< priority << " Value of k: "<< k_value;
	cout << endl;
	return {priority, k_value};
}

int main ()
{

  SSD SSD1(2, 200); //(priority, value of k)
	SSD SSD2(1, 30);
	SSD SSD3(5, 150);
	SSD SSD4(1, 10);
	SSD SSD5(3, 1);

	//priority_queue<SSD> queue;
	priority_queue<SSD, vector<SSD>, SSDCompare> queue;

	queue.push(SSD1);
	queue.push(SSD2);
	queue.push(SSD3);
	queue.push(SSD4);
	queue.push(SSD5);

	while (!queue.empty())
	{
		SSD t = queue.top();
		cout << "priority " << t.priority << " k_number " << t.k_number << endl;
		queue.pop();
	}

	getBestK(1,2,3);

	return 0;
}
