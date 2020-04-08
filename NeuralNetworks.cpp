#include <iostream>
#include "Matrix.h"
#include "NeuralNetwork.h"
#include "olcConsoleGameEngine.h"
#include <vector>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

int main()
{
	srand(static_cast<unsigned>(time(0)));
	NeuralNetwork* nn = new NeuralNetwork(2, 2, 1);
	std::vector<float> entradas[] = { { 0 , 0},
									  { 1 , 0},
									  { 0 , 1},
									  { 1 , 1} };
	std::vector<float> esperado[] = { {0},{1},{1},{0} };
	for (size_t i = 0; i < 50000; i++)
	{
		int index = rand() % 4;
		nn->train(&entradas[index], &esperado[index]);
	}
	std::cout << "0,0: " << nn->feedForward(&entradas[0])->at(0) << std::endl;
	std::cout << "0,1: " << nn->feedForward(&entradas[1])->at(0) << std::endl;
	std::cout << "1,0: " << nn->feedForward(&entradas[2])->at(0) << std::endl;
	std::cout << "1,1: " << nn->feedForward(&entradas[3])->at(0) << std::endl;
	
	std::vector<int> f1 = {3,2,3,4};
	NeuralNetwork* nn2 = new NeuralNetwork(2, f1, 2);
	delete(nn);
	return 0;
}   