buildLinux:
	make -C out/build/Linux
buildMac:
	make -C out/build/MacOS
cmakeLinux:
	cmake -S . -B out/build/Linux
cmakeMac:
	cmake -S . -B out/build/MacOS -G 'Unix Makefiles'
runLinux:
	out/build/Linux/NeuralNetwork
runMac:
	out/build/Debug/MacOS/NeuralNetworkd
testMac:
	out/build/MacOS/NeuralNetwork_test