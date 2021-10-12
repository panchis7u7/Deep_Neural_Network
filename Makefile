build:
	make -C out/build/Linux
cmake:
	cmake -S . -B out/build/Linux
run:
	out/build/Linux/NeuralNetwork
