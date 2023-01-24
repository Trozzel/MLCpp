#ifndef __LAYER_HPP__
#define __LAYER_HPP__

#include <memory>
#include <vector>

#include "Neuron.hpp"
#include "Activation.hpp"

static int layerId {0};


// CLASS LAYER
/******************************************************************************/
class  Layer {
private:
	ActivationType _activator = ActivationType::Relu;
	std::unique_ptr<std::vector<Neuron>> _neurons{new std::vector<Neuron>};

public:
	/// CTOR
	/// @descrip create layer based upon previous layer size for determining the
	/// amount of weights per neuron and the layer size for this layer
	/// @param prevLayerSz the size of preceding layer. If initial layer, this will
	/// be the amount of features
	/// @param curLayerSz the size of *this layer
	/// @param activationType activate to be applied upon this layer
	/**************************************************************************/
	Layer(int prevLayerSz, int curLayerSz, ActivationType activationType = ActivationType::Relu);
	
	/// COPY CTOR
	/// @note For COPY CTOR, must create a new unique pointer to copied neurons
	/**************************************************************************/
	Layer(const Layer& other) ;

	/// MOVE CTOR
	/**************************************************************************/
	Layer(Layer&& other) ;

	~Layer() = default;

	/// COPY ASSIGNMENT OPERTATOR
	/**************************************************************************/
	Layer& operator=(const Layer& other) ;
	
	/// MOVE ASSIGNMENT OPERTATOR
	/**************************************************************************/
	Layer& operator=(Layer&& other) ;

	// GET ACTIVATOR
	/**************************************************************************/
	ActivationType activator() const { return _activator;}
	// SET ACTIVAION TYPE
	/**************************************************************************/
	void activationType(ActivationType activationType) {
		_activator = activationType;
	}
	// GET CONST NEURONS
	/**************************************************************************/
	std::vector<Neuron>& neurons() const      {return *_neurons;}

	// GET NON-CONST NEURONS
	std::vector<Neuron>& neurons() 					{return *_neurons;}
	
	// GET SIZE OF LAYER (i.e. num neurons)
	/**************************************************************************/
	inline size_t size() const 						{return _neurons->size();}
};

#endif // __LAYER_HPP__
