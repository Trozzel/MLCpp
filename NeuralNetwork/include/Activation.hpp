#ifndef __ACTIVATION_HPP__
#define __ACTIVATION_HPP__

#include <functional>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include <cmath>

enum class ActivationType {
	Sigmoid, Relu, LeakyRelu, TanH, Elu
};

struct Activator {
	virtual double fwd(double) = 0;
	virtual double bkwd(double) = 0;
};

struct Sigmoid : Activator {
	double fwd(double x) final;
	double bkwd(double x) final;
};

struct Relu : Activator {
	double fwd(double x) final;
	double bkwd(double x) final;
};

struct LeakyRelu : Activator {
	double alpha{};
	LeakyRelu(double alpha);
	double fwd(double x) final;
	double bkwd(double x) final;
};

struct TanH : Activator {
	double fwd(double x) final;
	double bkwd(double x) final;
};


//template<ActivationType>
//struct Activator ;

//template<>
//struct Activator<ActivationType::Sigmoid> {
//	double fwd(double x) { return 1 / (1 + std::exp(-x)); }
//	double bkwd(double x) { return x * (1 - x); }
//};
//
//template<>
//struct Activator<ActivationType::Relu> {
//	double fwd(double x) { return (x > 0) ? x : 0; }
//	double bkwd(double x) { 
//		if(x>0) return 1;
//		else if (x < 0) return 0;
//		else throw std::runtime_error("Can't derivative of relu(0)");
//	}
//};
//
//template<>
//struct Activator<ActivationType::LeakyRelu> {
//	double alpha{};
//	Activator(double alpha) : alpha(alpha) {}
//
//	double fwd(double  x) { return (x > 0) ? x : alpha * x; }
//	double bkwd(double x) { return (x > 0) ? x : alpha; }
//};
//
//template<>
//struct Activator<ActivationType::Elu> {
//	double alpha{};
//	Activator(double alpha) : alpha(alpha) {}
//	double fwd(double x) { return (x >=0) ? x : alpha * (std::exp(x) - 1);}
//	double bkwd(double x) { return (x>=0) ? 1 : alpha * (std::exp(x) - 1) + alpha; }
//};
//
//template<>
//struct Activator<ActivationType::TanH> {
//	double fwd(double x) { return 2 / (1 + std::exp(-2 * x)) -1; }
//	double bkwd(double x) { return 1 - std::pow((2 / (1 + std::exp(-2 * x)) -1), 2); };
//};

//template<ActivationType A>
//struct Activate;
//
//template<>
//struct Activate<ActivationType::Sigmoid> {
//	double operator()(double x) { return 1 / (1 + std::exp(-x));}
//};
//
//template<>
//struct Activate<ActivationType::Relu> {
//	double _x;
//	Activate(double x): _x(x) {};
//	double operator()() {return _x > 0 ? _x : 0; }
//};
//
//template<>
//struct Activate<ActivationType::LeakyRelu> {
//	double _x;
//	Activate(double x): _x(x) {};
//
//	template<double Alpha>
//	double operator()() {return _x > 0 ? _x : Alpha * _x; }
//};
//
//template<>
//struct Activate<ActivationType::InvSqrtRelu> {
//	double _x;
//	Activate(double x): _x(x) {};
//	double operator()() {return 1 / std::sqrt(_x); }
//};
//
//template<>
//struct Activate<ActivationType::Binary> {
//	double _x;
//	Activate(double x) : _x(x) {}
//	double operator()() {return _x > 0 ? 1 : 0; }
//};

#endif // __ACTIVATION_HPP__
