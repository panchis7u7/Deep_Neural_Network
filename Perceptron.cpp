#include "Perceptron.h"
#include <iostream>
#include <cstdlib>
#include <time.h>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

Perceptron::Perceptron() {
	srand(time(0));
	int wtamaño = 2;
	float w[2];
	for (int i = 0; i < wtamaño; i++) {
		w[i] = -1 + (rand() % 2);
		std::cout << w[i] << std::endl;
	}
}

int Perceptron::sign(float n) {
	return (n >= 0) ? 1 : -1;
}

int Perceptron::guess(float guess[]) {
	return 0;
}