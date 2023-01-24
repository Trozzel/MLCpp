#ifndef __ACTIVATION_HPP__
#define __ACTIVATION_HPP__

#include <cmath>

enum class ActivationType {
	Sigmoid, Relu, LeakyRelu, InvSqrtRelu, Binary
};

template<ActivationType A>
struct Activate;

template<>
struct Activate<ActivationType::Sigmoid> {
	double _x;
	Activate(double x) : _x(x) {};
	double operator()() { return 1 / (1 + std::exp(-_x));}
};

template<>
struct Activate<ActivationType::Relu> {
	double _x;
	Activate(double x): _x(x) {};
	double operator()() {return _x > 0 ? _x : 0; }
};

template<>
struct Activate<ActivationType::LeakyRelu> {
	double _x, _alpha;
	Activate(double x, double alpha): _x(x), _alpha(alpha) {};
	double operator()() {return _x > 0 ? _x : _alpha * _x; }
};

template<>
struct Activate<ActivationType::InvSqrtRelu> {
	double _x, _alpha;
	Activate(double x): _x(x) {};
	double operator()() {return 1 / std::sqrt(_x); }
};

template<>
struct Activate<ActivationType::Binary> {
	double _x;
	Activate(double x) : _x(x) {}
	double operator()() {return _x > 0 ? 1 : 0; }
};

#endif // __ACTIVATION_HPP__
