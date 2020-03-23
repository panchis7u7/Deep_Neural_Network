#include <iostream>
#include "Perceptron.h"
#include "Matrix.h"
#include "NeuralNetwork.h"
#include "olcConsoleGameEngine.h"
#include <vector>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

int main()
{
	//NeuralNetwork* nn = new NeuralNetwork(2, 2, 2);
	Matrix* a = new Matrix(3, 2);
	a->aleatorizar();
	a->print();
	delete(a);
	NeuralNetwork* nn = new NeuralNetwork(2,2,2);
	delete(nn);
	return 0;
}   