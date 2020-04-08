#pragma once
#include "Matrix.h"

// Uploaded by panchis7u7 ~ Sebastian Madrigal

class NeuralNetwork {
public:
	NeuralNetwork(int, int, int);
	NeuralNetwork(int, std::vector<int>&, int);
	~NeuralNetwork();
	std::vector<float>* feedForward(std::vector<float>*);
	void train(std::vector<float>*, std::vector<float>*);
	static float sigmoid(float);
	static float dsigmoid(float);
private:
	float learning_rate = 0.25f;
	int inputLayerNodes;
	int hiddenLayerNodes;
	int outputLayerNodes;
	Matrix* pesos_ih;
	std::vector<Matrix*> pesos_hn;
	Matrix* pesos_ho;
	Matrix* bias_h;
	Matrix* bias_o;
	Matrix* salidas_capa_oculta;
protected:
};