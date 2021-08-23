#include <iostream>
#include <include/NeuralNetwork.hpp>
#include <vector>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

int main()
{
	srand(static_cast<unsigned>(time(0)));
	NeuralNetwork* nn = new NeuralNetwork(2, 4, 1);
	std::vector<float> entradas[] = { { 0 , 0},
									  { 1 , 0},
									  { 0 , 1},
									  { 1 , 1} };
	std::vector<float> esperado[] = { {0},{1},{1},{0} };
	for (size_t i = 0; i < 30000; i++)
	{
		int index = rand() % 4;
		nn->train(&entradas[index], &esperado[index]);
	}
	std::cout << "0,0: " << nn->feedForward(&entradas[0])->at(0) << std::endl;
	std::cout << "0,1: " << nn->feedForward(&entradas[1])->at(0) << std::endl;
	std::cout << "1,0: " << nn->feedForward(&entradas[2])->at(0) << std::endl;
	std::cout << "1,1: " << nn->feedForward(&entradas[3])->at(0) << std::endl;

	std::cout << std::endl;
	std::vector<int> f1 = { 4, 4 };
	NeuralNetwork* nn2 = new NeuralNetwork(2, f1, 1);
	for (size_t i = 0; i < 20000; i++)
	{
		int index = rand() % 4;
		nn2->trainDNN(&entradas[index], &esperado[index]);
	}
	std::cout << "0,0: " << nn2->feedForwardDNN(&entradas[0])->at(0) << std::endl;
	std::cout << "0,1: " << nn2->feedForwardDNN(&entradas[1])->at(0) << std::endl;
	std::cout << "1,0: " << nn2->feedForwardDNN(&entradas[2])->at(0) << std::endl;
	std::cout << "1,1: " << nn2->feedForwardDNN(&entradas[3])->at(0) << std::endl;
	return 0;
}   