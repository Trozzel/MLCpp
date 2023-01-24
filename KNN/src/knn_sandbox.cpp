#include <future>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <numeric>

using namespace std;

//long total{0};

template<typename T>
long getSum(const vector<T>& v, size_t beg, size_t end) {
    long total{0};
    for (; beg < end; ++beg) {
        total += v[beg];
    }
    return total;
}

int main()
{
	cout << "Num cores: " << thread::hardware_concurrency() << endl;
}
