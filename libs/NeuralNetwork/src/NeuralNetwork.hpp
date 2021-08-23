#pragma once
#include <src/Matrix.hpp>
using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

class NeuralNetwork {
public:
	NeuralNetwork(int, int, int);
	NeuralNetwork(int, std::vector<int>&, int);
	~NeuralNetwork();
	std::vector<float>* feedForward(std::vector<float>*);
	std::vector<float>* feedForwardDNN(std::vector<float>*); //DNN -> Deep Neural Network
	void train(std::vector<float>*, std::vector<float>*);
	void trainDNN(std::vector<float>*, std::vector<float>*);
	static float sigmoid(float);
	static float dsigmoid(float);
private:
	float learning_rate = 0.25f;
	int inputLayerNodes;
	int hiddenLayerSize;
	int hiddenLayerNodes;
	int outputLayerNodes;
	voxel::Matrix<float>* pesos_ih;
	std::vector<Matrix<float>*> pesos_hn;
	std::vector<Matrix<float>*> bias;
	std::vector<Matrix<float>*> salidas_capas_ocultas;
	std::vector<Matrix<float>*> errores;
	std::vector<Matrix<float>*> gradientes;
	std::vector<Matrix<float>*> deltas;
	Matrix<float>* pesos_ho;
	Matrix<float>* bias_h;
	Matrix<float>* bias_o;
	Matrix<float>* salidas_capa_oculta;
protected:
};