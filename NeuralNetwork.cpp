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
	this->oculta = nullptr;
	this->entradas = nullptr;
	this->salidas = nullptr;
}

NeuralNetwork::~NeuralNetwork() {
	delete(pesos_ih);
	delete(pesos_ho);
	delete(bias_h);
	delete(bias_o);
}

void NeuralNetwork::print() {
	std::cout << "Entradas: " << std::endl;
	entradas->print();
	std::cout << "Pesos capa oculta: " << std::endl;
	oculta->print();
	std::cout << "Salidas: " << std::endl;
	salidas->print();
}

std::vector<float>* NeuralNetwork::feedForward(std::vector<float>* vec_entrada) {
	/*
		Generando las salidas de la capa oculta
	*/
	//Convierte el vector de entrada a una matriz
	this->entradas = Matrix::fromVector(vec_entrada);
	//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas
	oculta = Matrix::multiplicar(this->pesos_ih, this->entradas);
	//Al resultado de la multiplicacion se le agrega el sesgo
	oculta->suma(this->bias_h);
	//sig((W * i) * b) se aplica la funcion sigmoide
	oculta->map(sigmoid);

	/*
		Generando la salida
	*/
	//Se multiplica la matriz de pesos entre la capa de salida y la matriz de salidas de la capa oculta
	salidas = Matrix::multiplicar(this->pesos_ho, oculta);
	//Al resultado de la multiplicacion se le agrega el sesgo
	salidas->suma(this->bias_o);
	//sig((W * i) * b) se aplica la funcion sigmoide
	salidas->map(sigmoid);
	return Matrix::toVector(salidas);
}

void NeuralNetwork::train(std::vector<float>* vec_entradas, std::vector<float>* vec_respuestas) {
	std::vector<float>* vec_salidas = this->feedForward(vec_entradas);
	//Errores de la Salida = respuesta - salida
	//Convertir vectores a matrices
	Matrix* respuestas = Matrix::fromVector(vec_respuestas);
	Matrix* salidas = Matrix::fromVector(vec_salidas);

	//Calcular el error => respuestas - salidas
	Matrix* errores_salida = Matrix::restaElementWise(respuestas, salidas);

	//Calcular el gradiente = learning_rate * output_errors * dsigmoid(outputs)
	Matrix* gradientes = Matrix::map(salidas, dsigmoid);
	gradientes->productoHadamard(errores_salida);
	gradientes->productoScalar(learning_rate);

	//Calcular deltas
	//pesos_delta = learning_rate * errores * dsigmoid(salidas) * pesos(T)
	Matrix* deltas_pesos_ho = Matrix::multiplicar(gradientes, Matrix::transpuesta(this->oculta));
	this->pesos_ho->suma(deltas_pesos_ho);
	this->bias_o->suma(gradientes);

	//Calcular los errores de la capa oculta->salida 
	Matrix* errores_capa_oculta = Matrix::multiplicar(Matrix::transpuesta(pesos_ho), errores_salida);

	//Calcular los gradientes de la capa oculta
	Matrix* gradientes_capa_oculta = Matrix::map(oculta, dsigmoid);
	gradientes_capa_oculta->productoHadamard(errores_capa_oculta);
	gradientes_capa_oculta->productoScalar(learning_rate);

	//Calcular deltas de la capa de entrada-oculta
	Matrix* deltas_pesos_ih = Matrix::multiplicar(gradientes_capa_oculta, Matrix::transpuesta(this->entradas));
	this->pesos_ih->suma(deltas_pesos_ih);
	this->bias_h->suma(gradientes_capa_oculta);
}

float NeuralNetwork::sigmoid(float n) {
	return (1 / (1 + pow(2.7182818284, n)));
}

float NeuralNetwork::dsigmoid(float y) {
	//return sigmoid(n) * (1 - sigmoid(n));
	return (y * (1 - y));
}