#ifndef __NETWORK_HPP__
#define  __NETWORK_HPP__

#include <functional>
#include <numeric>
#include <type_traits>

#include "Features.hpp"
#include "Neuron.hpp"
#include "Layer.hpp"
#include "CommonData.hpp"
#include "DataHandler.hpp"
#include "Activation.hpp"


template<typename T, typename LabelType>
class Network : public CommonData {
using Features_t = Features<T, LabelType>;
using FeaturesNorm_t = Features<float, LabelType>;
private:
	std::unique_ptr<std::vector<Layer>> _layers{new std::vector<Layer>};
	unsigned int _numClasses{};
	double       _learningRate {};
	double       _testPerformance {};

public:
	// CTOR
	/// @descrip Construct a network with given learning rate
	/// @param hiddenLayerSzs sizes of internal (hidden) layers
	/// @param actTypes activation type for each layer
	/// @param inLayerSz size of input layer to the network
	/// @param numClasses amount of categories (i.e. possible classes)
	/// @param lr learning rate
	/**************************************************************************/
	Network(const std::vector<int>& hiddenLayerSzs, 
			const std::vector<ActivationType>& actTypes,
		   	size_t inLayerSz,
		   	size_t numClasses,
		   	double lr) :
		_numClasses(numClasses), _learningRate(lr)
	{
		for (int i = 0; int layerSize : hiddenLayerSzs) { 
			if(i == 0) 
				// Layer(prev size, cur size, Activation type)
				_layers->emplace_back(inLayerSz, layerSize, actTypes.at(i));
			else 
				_layers->emplace_back(_layers->at(i-1).size(), layerSize, actTypes.at(i));
			++i;
		}
		// Create final output layer
		_layers->emplace_back(_layers->back().size(), _numClasses, actTypes.back());
	}

	/// CTOR
	/// @descrip Pass vector of Layer as const l-ref to be passed to _layers
	/// @param All layers of the network, including 
	/**************************************************************************/
	Network(const std::vector<Layer>& layers, double lr) : 
		_learningRate(lr) 
	{
		*_layers = layers;
		_numClasses = _layers->back().size();
	}

	/// CTOR
	/// @descrip Pass vector of Layer for move semantics on the vector
	/// @param All layers of the network, including 
	/**************************************************************************/
	Network(std::vector<Layer>&& layers, double lr) : 
		_learningRate(lr) 
	{
		std::swap(layers, *_layers);
		_numClasses = _layers->back().size();
	}
	
	/// COPY CTOR
	/// @descrip manually copies, layer by layer, the layers into *this
	/**************************************************************************/
	Network(const Network& other) :
	   	_numClasses(other._numClasses), 
		_learningRate(other._learningRate),
		_testPerformance(other._testPerformance)
   	{
		std::copy(other._layers->cbegin(), other._layers->cend(), std::back_inserter(*_layers));
		std::copy(other._layers->cbegin(), other._layers->cend(), std::back_inserter(*_layers));
	}

	/// MOVE CTOR
	/**************************************************************************/
	Network(Network && other) noexcept :
	   	_numClasses(other._numClasses),
	   	_learningRate(other._learningRate),
	   	_testPerformance(other._testPerformance) 
	{
		std::swap(_layers, other._layers);
	}

	/// DTOR
	~Network() = default;

	/// FORWARD PROPOGATE
	/// @descrip Go through every layer, applying weights to the incoming augmented features
	/// returning the final layer outputs
	/// @param features original normalized feature vector
	/// @return final output of network with size of amount of categories (i.e. classes)
	/**************************************************************************/
	std::vector<double> fwdProp(const FeaturesNorm_t& features) {
		std::vector<double> prevFeatures = *features.features();
		std::vector<double> layerOutputs{};
		for(auto & layer : *_layers) {
			layerOutputs.resize(layer.size());
			for(int i = 0; auto & neuron : layer.neurons()) {
				double dot = dotProd(neuron.weights(), prevFeatures);
				neuron.output(activate<layer.activator()>(dot)());
				layerOutputs.at(i++) = neuron.output();
			}
			if(&layer != &_layers->back()) { // Don't copy if last layer
				prevFeatures = std::move(layerOutputs);
			}
		}
		return layerOutputs;
	}

	/// BACK PROPAGAION
	/// @descrip Performs back propagation across all layers. Size of entry layer is size of 
	/// feature vector. Final layer is size of num classes.
	/// Creates a vector of errors that corresponds to the error at each neuron
	/// @param features 
	/**************************************************************************/
	void backProp(const FeaturesNorm_t& features) {
		// *** LAYER LEVEL
		// Go through every layer, starting from class outut estimation layer to input layer
		for(auto i = _layers->size() - 1; i >= 0; --i) {
			const Layer& layer = _layers->at(i);
			std::vector<double> errors;
			// Apply to all layers except for last layer (class output estimation layer)
			if(i != _layers->size() - 1) {
				// *** NEURON LEVEL
				// j corresponds to current neuron index as well as the next layer's
				// weight index
				for(int j = 0; const auto & neuron : layer.neurons()) {
					double error = 0.0;
					// *** NEXT LAYER'S WEIGHTS LEVEL
					for(const auto & fwdNeuron : _layers->at(i+1).neurons()) {
						error += fwdNeuron.weights().at(j) * fwdNeuron.delta();
					}
					errors.push_back(error);
					++j;
				}
			}
			// Apply to class output estimation layer
			else {
				for(const auto & neuron : _layers->at(i).neurons()) {
					// NOTE: I believe there should be an error function applied here
					errors.push_back(static_cast<double>(features.label()) - neuron.output()); 
				}
			}
			// *** NEURON LEVEL
			// Calculate deltas for each neuron
			for(int j = 0; const auto& neuron : layer.neurons()) {
				neuron.delta(errors.at(j++) * transferDerivative(neuron.output()));
			}
		}
	}

	/// DOT PRODUCT
	/// @descrip Return the dot products of two given vector<double>
	/// @param weights the weights to be multiplied with 
	/// @param features incoming augmented feature
	/// @return dot product of weights against features with offset of weights.back()
	/**************************************************************************/
	double dotProd(const std::vector<double>& weights, const std::vector<double>& features) {
		return std::inner_product(weights.cbegin(), weights.cend(), features.cbegin(), 
				weights.back(), std::multiplies<double>(), std::plus<double>());
	}

	/// ACTIVATION
	/// @descrip applies given Activation Function Object to the passed value
	/// @param dotVal the resultant dot product between weights on incoming outputs
	/// @alpha applies to activation types that handle two values
	/// @return activated value
	/**************************************************************************/
	template<ActivationType A>
		double activate(double dotVal, double alpha) {
			return Activate<A>(dotVal, alpha)();
		}
	template<ActivationType A>
		double activate(double dotVal) {
			return Activate<A>(dotVal)();
		}

	// TRANSFER DERIVATIVE
	/**************************************************************************/
	double transferDerivative(double output) { // Used for back propagation
		return output * (1 - output);
	}

	void updateWeights(const Features_t& features);
	int predict(const Features_t& features); // return index of maximum value in the output array
											 
	double test();
	void validate();
};

#endif // __NETWORK_HPP__
