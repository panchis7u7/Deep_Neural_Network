#pragma once
#include <vector>
#include <utility>
#include <functional>
#include "../../platform.hpp"
#include <include/Matrix.hpp>
using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

template <class T>
class NeuralNetwork
{
public:
	LIBEXP NeuralNetwork(unsigned input_nodes, std::vector<unsigned>& hidden_layer_nodes, unsigned output_nodes);
	LIBEXP virtual ~NeuralNetwork();
	LIBEXP virtual std::vector<T> *feed_forward(std::vector<T> *inputVec);
	LIBEXP virtual void train(std::vector<T> *guessesVec, std::vector<T> *answersVec);
	LIBEXP virtual inline void print_weights();
	LIBEXP void feed_forward_input_unit(std::vector<T>* inputs, std::function<T(T)> activation_function);
	LIBEXP void feed_forward_hidden_unit(std::function<T(T)> activation_function);

	static T sigmoid(T n)
	{
		return (1 / (1 + pow(2.718281828, (-n))));
	}

	static T dsigmoid(T y)
	{
		// return sigmoid(n) * (1 - sigmoid(n));
		return (y * (1 - y));
	}

protected:
	float mf_learning_rate = 0.25f;
	unsigned mu_input_layer_nodes;
	std::vector<unsigned>* mvu_hidden_layer_nodes;
	unsigned mu_output_layer_nodes;
	unsigned mu_total_layers;

	std::vector<std::pair<Matrix<T>*, Matrix<T>*>> m_vpm_weights_biases;
	std::vector<Matrix<T>*> m_vm_dot_outputs;
	std::vector<Matrix<T>*> m_vm_errors;
	std::vector<Matrix<T>*> m_vm_gradients;
	std::vector<Matrix<T>*> m_vm_deltas;

private:
	unsigned m_uHiddenLayerNodes;
	Matrix<T> *m_HiddenOutputWeights;
};

template <class T>
class DeepNeuralNetwork : public NeuralNetwork<T>
{
public:
	LIBEXP DeepNeuralNetwork(uint_fast64_t inputLayerNodes, std::vector<uint_fast64_t> &hiddenLayerNodes, uint_fast64_t outputLayerNodes);
	LIBEXP ~DeepNeuralNetwork() override;
	LIBEXP std::vector<T>* feed_forward(std::vector<T> *inputData) override;
	LIBEXP void train(std::vector<T> *guesses, std::vector<T> *answers) override;
	LIBEXP void print_weights() override;

private:
	// Number of Hidden Layers.
	unsigned int m_uHiddenLayerSize;
	// std::vector<Matrix<T> *> nth_hidden_weights;
	std::vector<Matrix<T> *> m_vHWeights;
	std::vector<Matrix<T> *> m_vBiases;
	std::vector<Matrix<T> *> m_vHiddenOutputWeights;
	std::vector<Matrix<T> *> m_vErrors;
	std::vector<Matrix<T> *> m_vGradients;
	std::vector<Matrix<T> *> m_vDeltas;
};