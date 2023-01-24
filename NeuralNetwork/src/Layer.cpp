#include "Layer.hpp"

// CTOR
/**************************************************************************/
Layer::Layer(int prevLayerSz, int curLayerSz, ActivationType activationType) : 
	_activator(activationType) 
{
	for (int i = 0; i < curLayerSz ; ++i) {
		_neurons->emplace_back(prevLayerSz);
	}
}
// COPY CTOR
/**************************************************************************/
Layer::Layer(const Layer& other) : _activator(other._activator){
		std::vector<Neuron>* neurons = new std::vector(*other._neurons);
		_neurons.reset(neurons); // deletes pervious vector<Neuron> and manages new vector<Neuron>
}

// MOVE CTOR
/**************************************************************************/
Layer::Layer(Layer&& other) : _activator(other._activator){
		// NOTE: unique_ptr::release sets pointer to nullptr and calls to destructor of managed object
		_neurons.reset(other._neurons.release());
	}

// COPY ASSIGNMENT
/**************************************************************************/
Layer& Layer::operator=(const Layer& other) {
	if(this == &other) return *this;
	_activator = other._activator;
	std::vector<Neuron>* neurons = new std::vector(*other._neurons);
	_neurons.reset(neurons);

	return *this;
}

// MOVE ASSIGNMENT
/**************************************************************************/
Layer& Layer::operator=(Layer&& other) {
	if(this == &other) return *this;
	std::swap(_activator, other._activator);
	_neurons.reset(other._neurons.release());
	return *this;
}


