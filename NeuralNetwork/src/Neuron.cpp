#include "Neuron.hpp"

// CTOR
Neuron::Neuron(int prevLayerSz) {	
	initWeights(prevLayerSz);
}

// INITIALIZE WEIGHTS
void Neuron::initWeights(int prevLayerSz) {
	// NOTE: Should do normal distribution instead of uniform?
	auto gen = std::mt19937(std::random_device{}());
	auto dist = std::uniform_real_distribution<double>(-1., 1.);
	for(int i = 0; i < prevLayerSz; ++i) {
		_weights.push_back(dist(gen));
	}
}

