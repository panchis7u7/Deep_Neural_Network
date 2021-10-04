#include <iostream>
#include <include/NeuralNetwork.hpp>
#include <vector>
#include <include/rapidxml.hpp>
#include <include/SerialPortWin.hpp>
#include <list>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

int main()
{
	srand(static_cast<unsigned>(time(0)));
	NeuralNetwork<float>* nn = new NeuralNetwork<float>(2, 4, 1);
	std::vector<float> entradas[] = { { 0.0 , 0.0},
									  { 1.0 , 0.0},
									  { 0.0 , 1.0},
									  { 1.0 , 1.0} };
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
	std::vector<uint_fast64_t> f1 = { 4, 4 };
	DeepNeuralNetwork<float>* nn2 = new DeepNeuralNetwork<float>(2, f1, 1);
	for (size_t i = 0; i < 15000; i++)
	{
		int index = rand() % 4;
		nn2->train(&entradas[index], &esperado[index]);
	}
	std::cout << "0,0: " << nn2->feedForward(&entradas[0])->at(0) << std::endl;
	std::cout << "0,1: " << nn2->feedForward(&entradas[1])->at(0) << std::endl;
	std::cout << "1,0: " << nn2->feedForward(&entradas[2])->at(0) << std::endl;
	std::cout << "1,1: " << nn2->feedForward(&entradas[3])->at(0) << std::endl;

	std::vector<float> guess{ 1.0, 1.0 };
	std::cout << nn2->feedForward(&guess)->at(0) << std::endl;
	nn2->printWeights();

	/*std::list<int> ports = getAvailablePorts();
	for (std::list<int>::iterator it = ports.begin(); it != ports.end(); ++it) {
		std::cout << "COM" << *it << std::endl;
	}*/

	SerialPortWin arduino(L"COM4");
	char msgOut[] = "Hola desde Windows 11";
	arduino.outputToPort(msgOut, strlen(msgOut) + 1);
	
	std::cin.get();

	return 0;
}   