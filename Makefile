# Linux build-related commands.
# ===================================================================

cmdLinux:
	cmake -S . -B out/build/Debug/Linux -DCMAKE_BUILD_TYPE=Debug
cmrLinux:
	cmake -S . -B out/build/Release/Linux -DCMAKE_BUILD_TYPE=Release
bdLinux:
	make -C out/build/Debug/Linux
brLinux:
	make -C out/build/Release/Linux
rdLinux:
	out/build/Debug/Linux/NeuralNetworkd
rdtLinux:
	out/build/Debug/Linux/NeuralNetwork_test
runLinux:
	out/build/Release/Linux/NeuralNetworkd
rrtLinux:
	out/build/Release/Linux/NeuralNetwork_test
cleanLinux:
	rm -r out/build/Release/Linux/

# MacOS build-related commands.
# ===================================================================

cmdMacU:
	cmake -S . -B out/build/Debug/MacOS -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug
cmrMacU:
	cmake -S . -B out/build/Release/MacOS -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release
cmdMac:
	cmake -S . -B out/build/Debug/MacOS -DCMAKE_BUILD_TYPE=Debug
cmrMac:
	cmake -S . -B out/build/Release/MacOS -DCMAKE_BUILD_TYPE=Release
bdMac:
	make -C out/build/Debug/MacOS
brMac:
	make -C out/build/Release/MacOS
rdMac:
	out/build/Debug/MacOS/NeuralNetworkd
rdGuiMac:
	out/build/Debug/MacOS/NeuralNetwork.app/Contents/MacOS/NeuralNetwork
rdtMac:
	out/build/Debug/MacOS/NeuralNetwork_test
runMac:
	out/build/Release/MacOS/NeuralNetworkd
rrtMac:
	out/build/Release/MacOS/NeuralNetwork_test
cleanMac:
	rm -r out/build/Release/MacOS/
	rm -r out/build/Debug/MacOS/