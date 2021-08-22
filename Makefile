cmake:
	cmake -S . -B out/build/Linux

build:
	make -C out/build/Linux

run:
	out/build/Linux/DeepNeuralNetwork