#pragma once
#include "../../platform.hpp"
#include <include/Matrix.hpp>
using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

template <class T>
class NeuralNetwork
{
public:
	LIBEXP NeuralNetwork(unsigned inputNodes, unsigned hiddenNodes, unsigned outputNodes);
	LIBEXP virtual ~NeuralNetwork();
	LIBEXP virtual std::vector<T> *feedForward(std::vector<T> *inputVec);
	LIBEXP virtual void train(std::vector<T> *guessesVec, std::vector<T> *answersVec);
	LIBEXP virtual inline void printWeights();

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
	float m_fLearningRate = 0.25f;
	unsigned m_uInputLayerNodes;
	unsigned m_uOutputLayerNodes;
	Matrix<T> *m_ihWeights;
	Matrix<T> *m_hoWeights;
	Matrix<T> *m_hBias;
	Matrix<T> *m_oBias;

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
	LIBEXP std::vector<T> *feedForward(std::vector<T> *inputData) override;
	LIBEXP void train(std::vector<T> *guesses, std::vector<T> *answers) override;
	LIBEXP void printWeights() override;

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