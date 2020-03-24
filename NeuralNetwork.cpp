#include "NeuralNetwork.h"

// Uploaded by panchis7u7 ~ Sebastian Madrigal

NeuralNetwork::NeuralNetwork(int i, int h, int o) {
	this->inputLayerNodes = i;
	this->hiddenLayerNodes = h;
	this->outputLayerNodes = o;
	//Matriz que representa los pesos entre las capa de Entrada-Oculta
	this->pesos_ih = new Matrix(this->hiddenLayerNodes, this->inputLayerNodes);
	//Matriz que representa los pesos entre las capa Oculta-Salida
	this->pesos_ho = new Matrix(this->outputLayerNodes, this->hiddenLayerNodes);
	//Asignamos valores aleatorios a las matrices
	this->pesos_ih->aleatorizar();
	this->pesos_ho->aleatorizar();
	//Asignamos un sesgo o predisposicion a las neuronas
	this->bias_h = new Matrix(this->hiddenLayerNodes, 1);
	this->bias_o = new Matrix(this->outputLayerNodes, 1);
	this->bias_h->aleatorizar();
	this->bias_o->aleatorizar();
}

NeuralNetwork::~NeuralNetwork() {
	delete(pesos_ih);
	delete(pesos_ho);
	delete(bias_h);
	delete(bias_o);
}

std::vector<float>* NeuralNetwork::feedForward(std::vector<float>* vecEntrada) {
	/*
		Generando las salidas de la capa oculta
	*/
	//Convierte el vector de entrada a una matriz
	Matrix* entradas = Matrix::fromVector(vecEntrada);
	//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas
	Matrix* oculta = Matrix::multiplicar(this->pesos_ih, entradas);
	//Al resultado de la multiplicacion se le agrega el sesgo
	oculta->sumar(bias_h);
	//sig((W * i) * b) se aplica la funcion sigmoide
	oculta->map(sigmoid);

	/*
		Generando la salida
	*/
	//Se multiplica la matriz de pesos entre la capa de salida y la matriz de salidas de la capa oculta
	Matrix* salidas = Matrix::multiplicar(this->pesos_ho, oculta);
	//Al resultado de la multiplicacion se le agrega el sesgo
	salidas->sumar(bias_o);
	//sig((W * i) * b) se aplica la funcion sigmoide
	salidas->map(sigmoid);
	return Matrix::toVector(salidas);
}

std::vector<float>* NeuralNetwork::train(std::vector<float>* entradas, std::vector<float>* respuestas) {

	//Error = respuesta - salida

	return 0;
}

float NeuralNetwork::sigmoid(float n) {
	return (1 / (1 + pow(2.7182818284, n)));
}

float NeuralNetwork::dsigmoid(float n) {
	return 0;
}
