#pragma once
#include "Matrix.h"

// Uploaded by panchis7u7 ~ Sebastian Madrigal

class NeuralNetwork {
public:
	NeuralNetwork(int i, int h, int o);
	std::vector<float>* feedForward(std::vector<float>*);
	static float sigmoid(float n);
private:
	int inputLayerNodes;
	int hiddenLayerNodes;
	int outputLayerNodes;
	Matrix* pesos_ih;
	Matrix* pesos_ho;
	Matrix* bias_h;
	Matrix* bias_o;
protected:
};
