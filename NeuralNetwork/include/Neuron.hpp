//
// Created by George Ford on 1/20/23.
//

#ifndef NEURALNETWORK_NEURON_HPP
#define NEURALNETWORK_NEURON_HPP

#include <vector>
#include <random>


// CLASS NEURON
/******************************************************************************/
class Neuron {
private:
	double _output{};
	double _delta{};
	std::vector<double> _weights{};


public:
	// CTOR
	Neuron(int prevLayerSz);
	Neuron(const Neuron& other) = default;
	Neuron(Neuron&& other) : _output(other._output), _delta(other._output),
		_weights(std::move(other._weights))
	{}
	Neuron& operator=(const Neuron& other) {
		if(this == &other) return *this;
		_output = other._output;
		_delta = other._delta;
		_weights = other._weights;
		return *this;
	}
	Neuron& operator=(Neuron&& other) {
		if(&other == this) return *this;
		_output = other._output;
		_delta = other._delta;
		std::swap(_weights, other._weights);
		return *this;
	}

	// GET / SET
	/**************************************************************************/
	double output() const {return _output;}
	void output(double out) {_output = out;}
	double delta() const {return _delta;}
	void delta(double d) {_delta = d;}

		
	void initWeights(int prevLayerSz);
	const std::vector<double>& weights() const { return _weights;}
};


#endif //NEURALNETWORK_NEURON_HPP
