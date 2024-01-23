#include <list>
#include <vector>
#include <iostream>
#include <spdlog/spdlog.h>
#include <tabulate/table.hpp>
#include <include/rapidxml.hpp>
#include <include/SerialPort.hpp>
#include <include/NeuralNetwork.hpp>
#ifdef QT_IS_AVAILABLE
	#include <QApplication>
	#include <include/MainWindow.hpp>
#endif

int main(int argc, char* argv[])
{
	// Execute if QT is defined.
	#ifdef QT_IS_AVAILABLE
		QApplication app(argc, argv);
		MainWindow w;
		w.show();
		return app.exec();
	#endif

	// Configure the logging library.

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	spdlog::set_level(spdlog::level::debug); // Set global log level to debug

	// Create a Simple Neural Network.

	srand(static_cast<unsigned>(time(0)));
	
	NeuralNetwork<float> *nn = new NeuralNetwork<float>(2, 4, 1);
	std::vector<float> inputs[] = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};
	std::vector<float> expected_outputs[] = {{0}, {1}, {1}, {0}};
	
	for (size_t i = 0; i < 30000; i++) {
		int index = rand() % 4;
		nn->train(&inputs[index], &expected_outputs[index]);
	}

	// Create a table for outputting training results.
	tabulate::Table nn_results;

	// Center align the 'inputs' and 'outputs' column.
	nn_results.column(0).format().font_align(tabulate::FontAlign::center);
	nn_results.column(1).format().font_align(tabulate::FontAlign::center);

	// Populate the table with the calculated data.
	nn_results.add_row({"Inputs", "Outputs"});
	nn_results.add_row({"0,0", std::to_string(nn->feedForward(&inputs[0])->at(0))});
	nn_results.add_row({"0,1", std::to_string(nn->feedForward(&inputs[1])->at(0))});
	nn_results.add_row({"1,0", std::to_string(nn->feedForward(&inputs[2])->at(0))});
	nn_results.add_row({"1,1", std::to_string(nn->feedForward(&inputs[3])->at(0))});

	// Print the data.
	std::cout << "\n" << nn_results << "\n" << std::endl;

	// Create a Deep Neural Network.
	std::vector<uint_fast64_t> f1 = {4, 4};
	DeepNeuralNetwork<float> *dnn = new DeepNeuralNetwork<float>(2, f1, 1);
	
	for (size_t i = 0; i < 150000; i++) {
		int index = rand() % 4;
		dnn->train(&inputs[index], &expected_outputs[index]);
	}

	// Create a table for outputting dnn training results.
	tabulate::Table dnn_results;

	// Populate the table with the calculated data.
	dnn_results.add_row({"Inputs", "Outputs"});
	dnn_results.add_row({"0,0", std::to_string(dnn->feedForward(&inputs[0])->at(0))});
	dnn_results.add_row({"0,1", std::to_string(dnn->feedForward(&inputs[1])->at(0))});
	dnn_results.add_row({"1,0", std::to_string(dnn->feedForward(&inputs[2])->at(0))});
	dnn_results.add_row({"1,1", std::to_string(dnn->feedForward(&inputs[3])->at(0))});

	// Print the data.
	std::cout << "\n" << dnn_results << "\n" << std::endl;

	std::vector<float> guess{1.0, 1.0};
	std::cout << dnn->feedForward(&guess)->at(0) << std::endl;
	dnn->printWeights();

	/*std::list<int> ports = getAvailablePorts();
	for (std::list<int>::iterator it = ports.begin(); it != ports.end(); ++it) {
		std::cout << "COM" << *it << std::endl;
	}*/
	
    //SerialPort::getAvailablePorts();

    //SerialPort arduino("/dev/cu.usbserial-14140");
    //arduino << "encendido";

    //std::string mensaje;
    //arduino >> mensaje;

    //std::cout << "El mensaje es : "  << mensaje << std::endl;

	return 0;
}
