#include <array>
#include <fstream>
#include <iostream>
#include <cctype>
#include <iomanip>
#include <bitset>
#include <vector>
#include <numeric>
#include <iterator>

using namespace std;

constexpr uint32_t intFromBytes(uint8_t* buf) {
	return (int32_t)(
			(buf[0] << 24) |
			(buf[1] << 16) |
			(buf[2] <<  8) |
			(buf[3])
	);
}

template<T>
void doSomething

int main() {
    vector<int*> ints{5, nullptr};
	cout << "Size of ints: " << ints.size() << endl;
	decltype(ints.begin())::difference_type;
}
