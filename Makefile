build:
	make -C out/build/Linux
cmake:
	cmake -S . -B out/build/Linux
test:
	out/build/Linux/NeuralNetwork_test
run:
	out/build/Linux/NeuralNetwork
