#pragma once
#include <vector>
#include <utility>
#include "../../platform.hpp"
#include <include/Matrix.hpp>
using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

template <class T>
class NeuralNetwork {
public:
	LIBEXP NeuralNetwork(unsigned input_nodes, std::vector<unsigned>& hidden_layer_nodes, unsigned output_nodes);
	LIBEXP virtual ~NeuralNetwork();
	LIBEXP virtual std::vector<T> *feed_forward(std::vector<T> *inputVec);
	LIBEXP virtual void train(std::vector<T> *guessesVec, std::vector<T> *answersVec);
	LIBEXP virtual inline void print_weights();

	static T sigmoid(T n) {
		return (1 / (1 + pow(2.718281828, (-n))));
	}

	static T dsigmoid(T y) {
		// sigmoid(n) * (1 - sigmoid(n));
		return (y * (1 - y));
	}

protected:
	float mf_learning_rate = 0.25f;
	unsigned mu_input_layer_nodes;
	std::vector<unsigned>* mvu_hidden_layer_nodes;
	unsigned mu_output_layer_nodes;
	unsigned mu_total_layers;

	std::vector<std::pair<Matrix<T>*, Matrix<T>*>> m_vpm_weights_weights_t;
	std::vector<std::pair<Matrix<T>*, Matrix<T>*>> m_vm_product_outputs;
	std::vector<Matrix<T>*> m_vm_biases;
	std::vector<Matrix<T>*> m_vm_errors;
	std::vector<Matrix<T>*> m_vm_gradients;
	std::vector<Matrix<T>*> m_vm_deltas;
};