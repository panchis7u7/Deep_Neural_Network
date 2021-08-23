#include "NeuralNetwork.hpp"
#include <memory>

using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

NeuralNetwork::NeuralNetwork(int i, int h, int o) {
	this->inputLayerNodes = i;
	this->hiddenLayerNodes = h;
	this->hiddenLayerSize = 1;
	this->outputLayerNodes = o;
	//Matriz que representa los pesos entre las capa de Entrada-Oculta
	this->pesos_ih = new Matrix<float>(this->hiddenLayerNodes, this->inputLayerNodes);
	//Matriz que representa los pesos entre las capa Oculta-Salida
	this->pesos_ho = new Matrix<float>(this->outputLayerNodes, this->hiddenLayerNodes);
	//Se asigna valores aleatorios a las matrices
	this->pesos_ih->randomize();
	this->pesos_ho->randomize();
	//Se asigna un sesgo o predisposicion a las neuronas
	this->bias_h = new Matrix<float>(this->hiddenLayerNodes, 1);
	this->bias_o = new Matrix<float>(this->outputLayerNodes, 1);
	//Se asigna valores aleatorios a las matrices
	this->bias_h->randomize();
	this->bias_o->randomize();
	this->salidas_capa_oculta = nullptr;
}

NeuralNetwork::NeuralNetwork(int i, std::vector<int>& h, int o) {
	this->inputLayerNodes = i;
	this->hiddenLayerSize = h.size();
	this->outputLayerNodes = o;
	//Matriz que representa los pesos entre las capa de Entrada-Oculta[0]
	this->pesos_ih = new Matrix<float>(h[0], this->inputLayerNodes);
	//Se asigna valores aleatorios a las matrices
	this->pesos_ih->randomize();
	//reserva espacio en memoria para los pesos de las n capas ocultas de la red (Optimizacion, evita la redimiension del vector cada vez que se inserta).
	this->pesos_hn.reserve(h.size() - 1);
	this->gradientes.reserve(h.size() - 1);
	this->deltas.reserve(h.size() - 1);
	this->salidas_capas_ocultas.reserve(h.size());
	this->errores.reserve(h.size());
	this->bias.reserve(h.size()+1);

	for (size_t i = 0; i < h.size(); i++)
	{
		errores.push_back(nullptr);
		salidas_capas_ocultas.push_back(nullptr);
		this->bias.push_back(new Matrix<float>(h[i], 1));
		this->bias.at(i)->randomize();
	}
	//reserva espacio en memoria para los sesgos de las n capas ocultas y de salida de la red (Optimizacion, evita la redimiension del vector cada vez que se inserta).
	
	for (size_t i = 0; i < h.size()-1; i++)
	{
		this->gradientes.push_back(nullptr);
		this->deltas.push_back(nullptr);
		this->pesos_hn.push_back(new Matrix<float>(h[i+1], h[i]));
		this->pesos_hn.at(i)->randomize();
	}
	//Matriz que representa los pesos entre las capa enesima(oculta)-Salida y se aleatoriza
	this->pesos_ho = new Matrix<float>(this->outputLayerNodes, h[h.size() - 1]);
	this->pesos_ho->randomize();
	//Se asigna un sesgo o predisposicion a la enesima capa oculta y se aleatoriza
	//this->bias.push_back(new Matrix<float>(h[h.size() - 1], 1));
	//this->bias.at(bias.size() - 1)->randomize();
	//Matriz que representa el sesgo de la capa oculta y se aleatoriza
	this->bias.push_back(new Matrix<float>(outputLayerNodes, 1));
	this->bias.at(bias.size()- 1)->randomize();

	//Variables por eliminar
	this->hiddenLayerNodes = 0;
	this->salidas_capa_oculta = nullptr;
	this->bias_h = nullptr;
	this->bias_o = nullptr;
}

NeuralNetwork::~NeuralNetwork() {
	delete(pesos_ih);
	delete(pesos_ho);
	for (auto& matrix : this->errores) {
		delete matrix;
	}
	for (auto& matrix : this->gradientes) {
		delete matrix;
	}
	for (auto& matrix : this->deltas) {
		delete matrix;
	}
	for (auto& matrix : this->salidas_capas_ocultas) {
		delete matrix;
	}
}

std::vector<float>* NeuralNetwork::feedForwardDNN(std::vector<float>* vec_entrada) {
	//----Generando las salidas de la capas ocultas----
	//Convierte el vector de entrada a una matriz
	Matrix<float>* entradas = Matrix<float>::fromVector(vec_entrada);
	//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas
	Matrix<float>* entradas_capa_oculta = Matrix<float>::dot(this->pesos_ih, entradas);
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_oculta->add(this->bias.at(0));
	//sig((W * i) + b) se aplica la funcion sigmoide
	////this->salidas_capas_ocultas.push_back(Matrix<float>::map(entradas_capa_oculta, sigmoid));

	this->salidas_capas_ocultas.at(0) = Matrix<float>::map(entradas_capa_oculta, sigmoid);
	//Se multiplica la matriz de pesos entre la capas ocultas y la matriz de entradas previas
	for (int i = 0; i < (hiddenLayerSize-1); i++)
	{
		//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas de la enesima capa oculta
		////this->salidas_capas_ocultas.push_back(Matrix<float>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i)));
		this->salidas_capas_ocultas.at(i+1) = Matrix<float>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i));
		//Al resultado de la multiplicacion se le agrega el sesgo
		this->salidas_capas_ocultas.at(i+1)->add(this->bias.at(i+1));
		//sig((W * i) + b) se aplica la funcion sigmoide
		this->salidas_capas_ocultas.at(i+1)->map(sigmoid);
	}
	//----Generando las salida----
	//Se multiplica la matriz de pesos entre la capa de salida y la matriz de salidas de la capa oculta
	Matrix<float>* entradas_capa_salida = Matrix<float>::dot(this->pesos_ho, this->salidas_capas_ocultas.at(salidas_capas_ocultas.size()-1));
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_salida->add(bias.at(hiddenLayerSize));
	//sig((W * i) * b) se aplica la funcion sigmoide
	Matrix<float>* salidas = Matrix<float>::map(entradas_capa_salida, sigmoid);
	//delete entradas_capa_oculta;
	return Matrix<float>::toVector(salidas);
}

std::vector<float>* NeuralNetwork::feedForward(std::vector<float>* vec_entrada) {
	/*
		Generando las salidas de la capa oculta
	*/
	//Convierte el vector de entrada a una matriz
	Matrix<float>* entradas = Matrix<float>::fromVector(vec_entrada);
	//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas
	Matrix<float>* entradas_capa_oculta = Matrix<float>::dot(this->pesos_ih, entradas);
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_oculta->add(this->bias_h);
	//sig((W * i) + b) se aplica la funcion sigmoide
	this->salidas_capa_oculta = Matrix<float>::map(entradas_capa_oculta, sigmoid);
	/*
		Generando la salida
	*/
	//Se multiplica la matriz de pesos entre la capa de salida y la matriz de salidas de la capa oculta
	Matrix<float>* entradas_capa_salida = Matrix<float>::dot(this->pesos_ho, this->salidas_capa_oculta);
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_salida->add(this->bias_o);
	//sig((W * i) * b) se aplica la funcion sigmoide
	Matrix<float>* salidas = Matrix<float>::map(entradas_capa_salida, sigmoid);
	return Matrix<float>::toVector(salidas);
}

void NeuralNetwork::train(std::vector<float>* vec_entradas, std::vector<float>* vec_respuestas) {
	std::vector<float>* vec_salidas = this->feedForward(vec_entradas);
	//Convertir vectores a matrices
	Matrix<float>* entradas = Matrix<float>::fromVector(vec_entradas);
	Matrix<float>* respuestas = Matrix<float>::fromVector(vec_respuestas);
	Matrix<float>* salidas = Matrix<float>::fromVector(vec_salidas);

	//Calcular el error => respuestas - salidas
	Matrix<float>* errores_salida = Matrix<float>::elementWiseSubstraction(respuestas, salidas);

	//Calcular los errores de la capa oculta->salida 
	Matrix<float>* errores_capa_oculta_salida = Matrix<float>::dot(Matrix<float>::transpose(pesos_ho), errores_salida);

	//Calcular el gradiente de la capa de salida = learning_rate * errores_salida * dsigmoid(salidas)
	Matrix<float>* gradiente_salida = Matrix<float>::map(salidas, dsigmoid);
	gradiente_salida->hadamardProduct(errores_salida);
	gradiente_salida->scalarProduct(learning_rate);

	//Calcular los gradientes de la capa oculta = learning_rate * errores_capa_oculta * dsigmoid(salidas_capa_oculta)
	Matrix<float>* gradientes_capa_oculta = Matrix<float>::map(this->salidas_capa_oculta, dsigmoid);
	gradientes_capa_oculta->hadamardProduct(errores_capa_oculta_salida);
	gradientes_capa_oculta->scalarProduct(learning_rate);

	//Calcular deltas de la capa oculta-salida
	//pesos_delta = learning_rate * errores * dsigmoid(salidas) * pesos(T)
	Matrix<float>* deltas_pesos_ho = Matrix<float>::dot(gradiente_salida, Matrix<float>::transpose(this->salidas_capa_oculta));
	this->pesos_ho->add(deltas_pesos_ho);
	this->bias_o->add(gradiente_salida);

	//Calcular deltas de la capa de entrada-oculta
	Matrix<float>* deltas_pesos_ih = Matrix<float>::dot(gradientes_capa_oculta, Matrix<float>::transpose(entradas));
	this->pesos_ih->add(deltas_pesos_ih);
	this->bias_h->add(gradientes_capa_oculta);
}

void NeuralNetwork::trainDNN(std::vector<float>* vec_entradas, std::vector<float>* vec_respuestas) {
	std::vector<float>* vec_salidas = this->feedForwardDNN(vec_entradas);
	//Convertir vectores a matrices
	Matrix<float>* entradas = Matrix<float>::fromVector(vec_entradas);
	Matrix<float>* respuestas = Matrix<float>::fromVector(vec_respuestas);
	Matrix<float>* salidas = Matrix<float>::fromVector(vec_salidas);

	//Calcular el error => respuestas - salidas
	Matrix<float>* errores_salida = Matrix<float>::elementWiseSubstraction(respuestas, salidas);

	//Calcular los errores de la capa oculta->salida 
	
	this->errores.at(hiddenLayerSize-1) = Matrix<float>::dot(Matrix<float>::transpose(this->pesos_ho), errores_salida);
	for (int i = hiddenLayerSize-1; i > 0; i--)
	{
		this->errores.at(i-1) = Matrix<float>::dot(Matrix<float>::transpose(this->pesos_hn.at(i - 1)), errores.at(i));
	}

	//Calcular el gradiente de la capa de salida = learning_rate * errores_salida * dsigmoid(salidas)
	Matrix<float>* gradiente_salida = Matrix<float>::map(salidas, dsigmoid);
	gradiente_salida->hadamardProduct(errores_salida);
	gradiente_salida->scalarProduct(learning_rate);
	this->bias.at(bias.size() - 1)->add(gradiente_salida);
	//Calcular el gradiente de las capas ocultas y oculta-salida = learning_rate * errores_salida * dsigmoid(salidas)
	for (int i = hiddenLayerSize-1; i > 0; i--)
	{
		this->gradientes.at(i-1) = Matrix<float>::map(this->salidas_capas_ocultas.at(hiddenLayerSize-i), dsigmoid);
		this->gradientes.at(i-1)->hadamardProduct(errores.at(hiddenLayerSize-i));
		this->gradientes.at(i-1)->scalarProduct(learning_rate);
		this->bias.at(hiddenLayerSize-i)->add(gradientes.at(i-1));
	}
	//Calcular los gradientes de la capa entada-oculta = learning_rate * errores_capa_oculta * dsigmoid(salidas_capa_oculta)
	Matrix<float>* gradiente_entrada_oculta = Matrix<float>::map(this->salidas_capas_ocultas.at(0), dsigmoid);
	gradiente_entrada_oculta->hadamardProduct(errores.at(0));
	gradiente_entrada_oculta->scalarProduct(learning_rate);
	this->bias.at(0)->add(gradiente_entrada_oculta);

	//Calcular deltas de la capa oculta-salida
	Matrix<float>* deltas_pesos_ho = Matrix<float>::dot(gradiente_salida, Matrix<float>::transpose(this->salidas_capas_ocultas.at(salidas_capas_ocultas.size()-1)));
	this->pesos_ho->add(deltas_pesos_ho);
	for (int i = hiddenLayerSize - 1; i > 0; i--)
	{
		//this->deltas.at(i-1) = Matrix<float>::dot(gradientes.at(i-1), Matrix<float>::transpose(this->salidas_capas_ocultas.at(i)));
		this->deltas.at(i-1) = Matrix<float>::dot(this->salidas_capas_ocultas.at(i), Matrix<float>::transpose(gradientes.at(i - 1)));
		this->pesos_hn.at(i-1)->add(deltas.at(i-1));
	}
	//Calcular deltas de la capa de entrada-oculta
	Matrix<float>* deltas_pesos_ih = Matrix<float>::dot(gradiente_entrada_oculta, Matrix<float>::transpose(entradas));
	this->pesos_ih->add(deltas_pesos_ih);
}

float NeuralNetwork::sigmoid(float n) {
	return (1 / (1 + pow(2.718281828, (-n))));
}

float NeuralNetwork::dsigmoid(float y) {
	//return sigmoid(n) * (1 - sigmoid(n));
	return (y * (1 - y));
}