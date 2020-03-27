#include <iostream>
#include "Perceptron.h"
#include "Matrix.h"
#include "NeuralNetwork.h"
#include "olcConsoleGameEngine.h"
#include <vector>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

int main()
{
	srand(static_cast<unsigned>(time(0)));
	NeuralNetwork* nn = new NeuralNetwork(2, 2, 1);
	std::vector<float> entradas = {1, 0};
	std::vector<float> esperado = {1};
	for (size_t i = 0; i < 5; i++)
	{
		//nn->train(&entradas, &esperado);
	}
	std::vector<float>* salida = nn->feedForward(&entradas);
	for (size_t i = 0; i < salida->size(); i++)
	{
		std::cout << salida->at(i) << std::endl;
	}
	std::cout << std::endl;
	delete(nn);
	return 0;
}   