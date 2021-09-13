#pragma once
#include <include/Matrix.hpp>
using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

template <class T>
class NeuralNetwork {
public:
	NeuralNetwork(int inputNodes, int hiddenNodes, int outputNodes);
	virtual ~NeuralNetwork();
	virtual std::vector<T>* feedForward(std::vector<T>* inputData);
	virtual void train(std::vector<T>* guesses, std::vector<T>* answers);
	virtual inline void printWeights();
	
	static T sigmoid(T n) {
		return (1 / (1 + pow(2.718281828, (-n))));
	}

	static T dsigmoid(T y) {
		//return sigmoid(n) * (1 - sigmoid(n));
		return (y * (1 - y));
	}

protected:
	float learning_rate = 0.25f;
	int inputLayerNodes;
	int outputLayerNodes;
	Matrix<T>* pesos_ih;
	Matrix<T>* pesos_ho;
	Matrix<T>* bias_h;
	Matrix<T>* bias_o;
private:
	int hiddenLayerNodes;
	Matrix<T>* hidden_weights_output;
};

template <class T>
class DeepNeuralNetwork : public NeuralNetwork<T> {
public:
	DeepNeuralNetwork(uint_fast64_t input, std::vector<uint_fast64_t>& hidden, uint_fast64_t output);
	~DeepNeuralNetwork();
	std::vector<T>* feedForward(std::vector<T>* inputData);
	void train(std::vector<T>* guesses, std::vector<T>* answers);
	void printWeights();

private:
	//Number of Hidden Layers.
	unsigned int hiddenLayerSize;	
	std::vector<Matrix<T>*> nth_hidden_weights;
	std::vector<Matrix<T>*> pesos_hn;
	std::vector<Matrix<T>*> bias;
	std::vector<Matrix<T>*> salidas_capas_ocultas;
	std::vector<Matrix<T>*> errores;
	std::vector<Matrix<T>*> gradientes;
	std::vector<Matrix<T>*> deltas;
};