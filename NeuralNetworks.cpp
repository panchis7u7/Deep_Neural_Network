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
	NeuralNetwork* nn = new NeuralNetwork(2, 4, 1);
	std::vector<float> entradas[] = { { 0.0f , 0.0f},
									  { 1.0f , 0.0f},
									  { 0.0f , 1.0f},
									  { 1.0f , 1.0f} };
	std::vector<float> esperado[] = { {0.0f},{1.0f},{1.0f},{0.0f} };
	for (size_t i = 0; i < 50000; i++)
	{
		int index = rand() % 4;
		nn->train(&entradas[index], &esperado[index]);
	}
	std::vector<float> entrada1 = { 0.0f, 0.0f };
	std::vector<float> entrada2 = { 1.0f, 0.0f };
	std::vector<float> entrada3 = { 0.0f, 1.0f };
	std::vector<float> entrada4 = { 1.0f, 1.0f };
	std::cout << "0,0: " << nn->feedForward(&entrada1)->at(0) << std::endl;
	std::cout << "0,1: " << nn->feedForward(&entrada2)->at(0) << std::endl;
	std::cout << "1,0: " << nn->feedForward(&entrada3)->at(0) << std::endl;
	std::cout << "1,1: " << nn->feedForward(&entrada4)->at(0) << std::endl;
	std::vector<float>* resultado = nn->feedForward(&entrada1);
	for (size_t i = 0; i < resultado->size(); i++)
	{
		std::cout << resultado->at(i) << std::endl;
	}
	delete(nn);
	return 0;
}   