cmake:
	cmake -S . -B out/build/

build:
	make -C out/build/

run:
	out/build/DeepNeuralNetwork