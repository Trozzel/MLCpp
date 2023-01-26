#include "Activation.hpp"

using namespace std;


double Sigmoid::fwd(double x) {
	return 1 / (1 + std::exp(-x)); 
}
double Sigmoid::bkwd(double x) {
	return x * (1 - x);
}

double Relu::fwd(double x) {
	return (x > 0) ? x : 0; 
}
double Relu::bkwd(double x) {
	if(x>0) return 1;
	else if (x < 0) return 0;
	else throw std::runtime_error("Can't derivative of relu(0)");
}

LeakyRelu::LeakyRelu(double alpha) : alpha(alpha) {}
double LeakyRelu::fwd(double x) {
	return (x > 0) ? x : alpha * x; 
}
double LeakyRelu::bkwd(double x) {
	return (x > 0) ? x : alpha; 
}

double TanH::fwd(double x) {
	return 2 / (1 + std::exp(-2 * x)) -1; 
}
double TanH::bkwd(double x) {
	return 1 - std::pow((2 / (1 + std::exp(-2 * x)) -1), 2);
}
