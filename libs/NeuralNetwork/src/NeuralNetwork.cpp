#include <include/NeuralNetwork.hpp>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

/*################################################################################################*/
// Simple Neural Network.
/*################################################################################################*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Constructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
NeuralNetwork<T>::NeuralNetwork(int inputNodes, int hiddenNodes, int outputNodes)
{

	this->inputLayerNodes = inputNodes;
	this->hiddenLayerNodes = hiddenNodes;
	this->outputLayerNodes = outputNodes;

	// Create matrices based upon the number of nodes supplied.
	/********************************************************************************/
	this->pesos_ih = new Matrix<T>(this->hiddenLayerNodes, this->inputLayerNodes);
	this->pesos_ho = new Matrix<T>(this->outputLayerNodes, this->hiddenLayerNodes);

	// Initialize random values into the weights matrices.
	/********************************************************************************/
	this->pesos_ih->randomize();
	this->pesos_ho->randomize();

	// Create bias matrices based upon the number of nodes supplied.
	/********************************************************************************/
	this->bias_h = new Matrix<T>(this->hiddenLayerNodes, 1);
	this->bias_o = new Matrix<T>(this->outputLayerNodes, 1);

	// Initialize random values into the bias matrices.
	/********************************************************************************/
	this->bias_h->randomize();
	this->bias_o->randomize();
	this->hidden_weights_output = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Destructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
NeuralNetwork<T>::~NeuralNetwork()
{
	delete (this->pesos_ih);
	delete (this->pesos_ho);
	delete (this->bias_h);
	delete (this->bias_o);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net FeedFoward.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::vector<T> *NeuralNetwork<T>::feedForward(std::vector<T> *inputVec)
{

	////////////////////////////////////////////////
	// sig((W * i) + b) process for input - hidden.
	// W -> Weights.
	// i -> Inputs.
	// b -> Bias.
	// sig -> Sigmoid function.
	////////////////////////////////////////////////
	/********************************************************************************/

	if (this->hidden_weights_output)
		delete this->hidden_weights_output;
	this->hidden_weights_output = Matrix<T>::dot(this->pesos_ih, inputVec);
	this->hidden_weights_output->add(this->bias_h);
	this->hidden_weights_output->map(NeuralNetwork<T>::sigmoid);

	// sig((W * i) + b) process for hidden - output.
	/********************************************************************************/
	Matrix<T> *outputs = Matrix<T>::dot(this->pesos_ho, this->hidden_weights_output);
	outputs->add(this->bias_o);
	outputs->map(NeuralNetwork<T>::sigmoid);

	// Vector conversion.
	/********************************************************************************/
	std::vector<T> *outputVec = Matrix<T>::toVector(outputs);

	// Memory deallocation.
	/********************************************************************************/
	delete outputs;

	return outputVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Training.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void NeuralNetwork<T>::train(std::vector<T> *vec_entradas, std::vector<T> *vec_respuestas)
{

	// Convert vector to a matrix type. 		TODO: Implement native vector support.
	/********************************************************************************/
	std::vector<T> *outputsVec = this->feedForward(vec_entradas);
	Matrix<T> *inputs = Matrix<T>::fromVector(vec_entradas);
	Matrix<T> *answers = Matrix<T>::fromVector(vec_respuestas);
	Matrix<T> *outputs = Matrix<T>::fromVector(outputsVec);

	// Error calculation.
	// 1) Output Errors -> (respuestas -  salidas).
	/********************************************************************************/
	Matrix<T> *outputErrors = Matrix<T>::elementWiseSubstraction(answers, outputs);

	// 2) Hidden Layer Errors -> (Wh^T * (Output Erros)).
	/********************************************************************************/
	Matrix<T> *ho_errors = Matrix<T>::transpose(pesos_ho);
	ho_errors->dot(*outputErrors);

	// Calculate output layer gradient (learning_rate * outputErrors * dsigmoid(salidas)).
	/********************************************************************************/
	Matrix<T> *output_gradients = Matrix<T>::map(outputs, NeuralNetwork<T>::dsigmoid);
	output_gradients->hadamardProduct(outputErrors);
	output_gradients->scalarProduct(this->learning_rate);

	// Calculate hidden layer gradient (learning_rate * HiddenErrors * dsigmoid(hidden_weights_output)).
	/********************************************************************************/
	Matrix<T> *hidden_gradients = Matrix<T>::map(this->hidden_weights_output, NeuralNetwork<T>::dsigmoid);
	hidden_gradients->hadamardProduct(ho_errors);
	hidden_gradients->scalarProduct(this->learning_rate);

	// Calculte Hidden-Output deltas (learning_rate * errors * dsigmoid(salidas) * weights(T)).
	/********************************************************************************/
	Matrix<T> *hidden_weights_output_T = Matrix<T>::transpose(this->hidden_weights_output);
	Matrix<T> *ho_deltas = Matrix<T>::dot(output_gradients, hidden_weights_output_T);
	this->pesos_ho->add(ho_deltas);
	this->bias_o->add(output_gradients);

	// Calculate Input-Hidden deltas.
	/********************************************************************************/
	Matrix<T> *inputs_T = Matrix<T>::transpose(inputs);
	Matrix<T> *ih_deltas = Matrix<T>::dot(hidden_gradients, inputs_T);
	this->pesos_ih->add(ih_deltas);
	this->bias_h->add(hidden_gradients);

	// Memory deallocation.
	/********************************************************************************/
	delete inputs;
	delete answers;
	delete outputs;
	delete outputsVec;
	delete outputErrors;
	delete output_gradients;
	delete hidden_gradients;
	delete hidden_weights_output_T;
	delete inputs_T;
	delete ho_deltas;
	delete ih_deltas;
	delete ho_errors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Weight Printing.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void NeuralNetwork<T>::printWeights()
{
	std::cout << "---- [Input - Hidden Layer Weights] ----"
			  << "\n\n"
			  << this->pesos_ih << std::endl;
	std::cout << "---- [Hidden - Output Layer Weights] ----"
			  << "\n\n"
			  << this->pesos_ho << std::endl;
}

/*################################################################################################*/
// Deep Neural Network.
/*################################################################################################*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Constructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
DeepNeuralNetwork<T>::DeepNeuralNetwork(uint_fast64_t input, std::vector<uint_fast64_t> &hidden, uint_fast64_t output) : NeuralNetwork<T>::NeuralNetwork(input, hidden[0], output)
{

	// Specify number of hidden layer present on the net.
	/********************************************************************************/
	this->hiddenLayerSize = hidden.size();

	// Reserve vector space to avoid reallocation for the nth hidden layers.
	/********************************************************************************/
	this->errores.reserve(this->hiddenLayerSize);
	this->salidas_capas_ocultas.reserve(this->hiddenLayerSize);
	this->pesos_hn.reserve(this->hiddenLayerSize - 1);
	this->gradientes.reserve(this->hiddenLayerSize - 1);
	this->deltas.reserve(this->hiddenLayerSize - 1);
	this->bias.reserve(this->hiddenLayerSize + 1);

	// Fill previously reserved vectors with null pointers and weights with random values.
	/********************************************************************************/
	for (size_t i = 0; i < this->hiddenLayerSize - 1; i++)
	{
		this->gradientes.push_back(nullptr);
		this->deltas.push_back(nullptr);
		this->pesos_hn.push_back(new Matrix<T>(hidden[i + 1], hidden[i]));
		this->pesos_hn.at(i)->randomize();

		this->errores.push_back(nullptr);
		this->salidas_capas_ocultas.push_back(nullptr);
		this->bias.push_back(new Matrix<T>(hidden[i], 1));
		this->bias.at(i)->randomize();
	}

	this->errores.push_back(nullptr);
	this->salidas_capas_ocultas.push_back(nullptr);
	this->bias.push_back(new Matrix<T>(hidden[this->hiddenLayerSize - 1], 1));
	this->bias[(this->hiddenLayerSize - 1)]->randomize();

	// Create nth-Hidden-Output weight Matrix.
	/********************************************************************************/
	this->pesos_ho = new Matrix<T>(this->outputLayerNodes, hidden[this->hiddenLayerSize - 1]);
	this->pesos_ho->randomize();

	// Create nth-Hidden-Output bias Matrix.
	/********************************************************************************/
	this->bias.push_back(new Matrix<T>(this->outputLayerNodes, 1));
	this->bias.at(bias.size() - 1)->randomize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Destructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
DeepNeuralNetwork<T>::~DeepNeuralNetwork()
{
	for (auto &matrix : this->errores)
		delete matrix;
	for (auto &matrix : this->gradientes)
		delete matrix;
	for (auto &matrix : this->deltas)
		delete matrix;
	for (auto &matrix : this->salidas_capas_ocultas)
		delete matrix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net FeedForward.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::vector<T> *DeepNeuralNetwork<T>::feedForward(std::vector<T> *inputVec)
{
	// Convierte el vector de entrada a una matriz
	// Matrix<T>* entradas = Matrix<T>::fromVector(inputData);

	////////////////////////////////////////////////
	// sig((W * i) + b) process for input - hidden.
	// W -> Weights.
	// i -> Inputs.
	// b -> Bias.
	// sig -> Sigmoid function.
	////////////////////////////////////////////////
	/********************************************************************************/
	if (this->salidas_capas_ocultas[0] != nullptr)
		delete this->salidas_capas_ocultas[0];
	this->salidas_capas_ocultas[0] = Matrix<T>::dot(this->pesos_ih, inputVec);
	this->salidas_capas_ocultas[0]->add(this->bias[0]);
	this->salidas_capas_ocultas[0]->map(NeuralNetwork<T>::sigmoid);

	// sig((W * i) + b) process for nth-hidden layers.
	/********************************************************************************/
	for (size_t i = 0; i < (this->hiddenLayerSize - 1); i++)
	{
		if (salidas_capas_ocultas[i + 1] != nullptr)
			delete salidas_capas_ocultas[i + 1];
		this->salidas_capas_ocultas.at(i + 1) = Matrix<T>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i));
		this->salidas_capas_ocultas.at(i + 1)->add(this->bias.at(i + 1));
		this->salidas_capas_ocultas.at(i + 1)->map(NeuralNetwork<T>::sigmoid);
	}

	// sig((W * i) + b) process for nth-hidden and output layer.
	/********************************************************************************/
	Matrix<T> *entradas_capa_salida = Matrix<T>::dot(this->pesos_ho, this->salidas_capas_ocultas.at(salidas_capas_ocultas.size() - 1));
	entradas_capa_salida->add(bias.at(hiddenLayerSize));
	entradas_capa_salida->map(NeuralNetwork<T>::sigmoid);

	// Matrix to Vector conversion for return value.
	/********************************************************************************/
	std::vector<T> *outputVec = Matrix<T>::toVector(entradas_capa_salida);

	// Memory deallocation.
	/********************************************************************************/
	delete entradas_capa_salida;

	return outputVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Training.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void DeepNeuralNetwork<T>::train(std::vector<T> *guessVec, std::vector<T> *answersVec)
{

	// Obtain Neural Nets outputs, given a guess vector.
	/********************************************************************************/
	std::vector<T> *outputsVec = this->feedForward(guessVec);

	// Vector to Matrix conversion for Matrix operations.
	/********************************************************************************/
	Matrix<T> *inputs = Matrix<T>::fromVector(guessVec);
	Matrix<T> *answers = Matrix<T>::fromVector(answersVec);
	Matrix<T> *outputs = Matrix<T>::fromVector(outputsVec);

	// Error calculation. (Answers - Outputs)
	/********************************************************************************/
	Matrix<T> *outputErrors = Matrix<T>::elementWiseSubstraction(answers, outputs);

	// Hidden-Output error calculation. (Backpropagation)
	/********************************************************************************/
	Matrix<T> *ho_weights = Matrix<T>::transpose(this->pesos_ho);
	if (this->errores[this->hiddenLayerSize - 1] != nullptr)
		delete this->errores[this->hiddenLayerSize - 1];
	this->errores.at(this->hiddenLayerSize - 1) = Matrix<T>::dot(ho_weights, outputErrors);

	for (size_t i = hiddenLayerSize - 1; i > 0; i--)
	{
		if (this->errores[i - 1] != nullptr)
			delete this->errores[i - 1];
		this->errores.at(i - 1) = Matrix<T>::dot(Matrix<T>::transpose(this->pesos_hn.at(i - 1)), errores.at(i));
	}

	// Output layer gradient calculation. (learning_rate * outputErrors * dsigmoid(outputs))
	/********************************************************************************/
	Matrix<T> *outputGradient = outputs;
	outputGradient->map(NeuralNetwork<T>::dsigmoid);
	outputGradient->hadamardProduct(outputErrors);
	outputGradient->scalarProduct(this->learning_rate);
	this->bias.at(bias.size() - 1)->add(outputGradient);

	// Hidden-(Hidden-Output) gradient calculation. (learning_rate * outputErrors * dsigmoid(salidas))
	/********************************************************************************/
	for (size_t i = hiddenLayerSize - 1; i > 0; i--)
	{
		if (this->gradientes[i - 1] != nullptr)
			delete this->gradientes[i - 1];
		this->gradientes.at(i - 1) = Matrix<T>::map(this->salidas_capas_ocultas.at(hiddenLayerSize - i), NeuralNetwork<T>::dsigmoid);
		this->gradientes.at(i - 1)->hadamardProduct(errores.at(hiddenLayerSize - i));
		this->gradientes.at(i - 1)->scalarProduct(this->learning_rate);
		this->bias.at(hiddenLayerSize - i)->add(gradientes.at(i - 1));
	}

	// Input-Hidden gradient calculation. (learning_rate * outputErrors * dsigmoid(hidden_weights_output))
	/********************************************************************************/
	Matrix<T> *gradiente_entrada_oculta = Matrix<T>::map(this->salidas_capas_ocultas.at(0), NeuralNetwork<T>::dsigmoid);
	gradiente_entrada_oculta->hadamardProduct(errores.at(0));
	gradiente_entrada_oculta->scalarProduct(this->learning_rate);
	this->bias.at(0)->add(gradiente_entrada_oculta);

	// Hidden-Output deltas calculation.
	/********************************************************************************/
	Matrix<T> *salidas_capas_ocultas_T = Matrix<T>::transpose(this->salidas_capas_ocultas.at(salidas_capas_ocultas.size() - 1));
	Matrix<T> *deltas_pesos_ho = Matrix<T>::dot(outputGradient, salidas_capas_ocultas_T);
	this->pesos_ho->add(deltas_pesos_ho);

	// Hidden-(Hidden-Output) deltas calculation.
	/********************************************************************************/
	for (size_t i = hiddenLayerSize - 1; i > 0; i--)
	{
		Matrix<T> *gradientes_T = Matrix<T>::transpose(this->gradientes.at(i - 1));
		if (this->deltas[i - 1] != nullptr)
			delete this->deltas[i - 1];
		this->deltas.at(i - 1) = Matrix<T>::dot(this->salidas_capas_ocultas.at(i), gradientes_T);
		this->pesos_hn.at(i - 1)->add(deltas.at(i - 1));
		delete gradientes_T;
	}

	// Input-Hidden deltas calculation.
	/********************************************************************************/
	inputs->transpose();
	Matrix<T> *deltas_pesos_ih = Matrix<T>::dot(gradiente_entrada_oculta, inputs);
	this->pesos_ih->add(deltas_pesos_ih);

	// Memory deallocation.
	/********************************************************************************/
	delete outputsVec;
	delete inputs;
	delete outputErrors;
	delete answers;
	delete ho_weights;
	delete salidas_capas_ocultas_T;
	delete gradiente_entrada_oculta;
	delete outputGradient;
	delete deltas_pesos_ih;
	delete deltas_pesos_ho;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Weight Printer.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void DeepNeuralNetwork<T>::printWeights()
{
	std::cout << "---- [Input - Hidden[0] Layer Weights] ----"
			  << "\n\n"
			  << this->pesos_ih << std::endl;
	std::cout << "---- [Hidden (n - n+1) Layer Weights] ----"
			  << "\n\n";
	int layer = 0;

	// nth Hidden wieghts matrix print.
	/********************************************************************************/
	for (auto nth_hidden_weights : this->pesos_hn)
	{
		std::cout << "#### Layer " << layer << " ####"
				  << "\n\n"
				  << nth_hidden_weights;
		layer++;
	}
	std::cout << "---- [Hidden[n-1] - Output Layer Weights] ----"
			  << "\n\n"
			  << this->pesos_ho << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Nets Template Specialization.
////////////////////////////////////////////////////////////////////////////////////////////////////

template class NeuralNetwork<float>;
template class DeepNeuralNetwork<float>;