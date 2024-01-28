#include <include/NeuralNetwork.hpp>
#include <spdlog/spdlog.h>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

// #################################################################################################
// Neural Network Class.
// #################################################################################################

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Net Constructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
NeuralNetwork<T>::NeuralNetwork(unsigned input_nodes, std::vector<unsigned>& hidden_layer_nodes, unsigned output_nodes) {
	this->mu_input_layer_nodes = input_nodes;
	this->mvu_hidden_layer_nodes = hidden_layer_nodes;
	this->mu_output_layer_nodes = output_nodes;
	this->mu_total_layers = input_nodes + hidden_layer_nodes.size() + output_nodes;

	// ------------------------------------------------------------------------------------
	// Create weight matrices and randomize its initial values.
	// We can take advantage of the loop to also create the deltas pre-allocations.
	// ------------------------------------------------------------------------------------

	auto input_hidden_weights = new Matrix<T>(hidden_nodes, input_nodes);
	input_hidden_weights->setDescription("Input-Hidden Weights");
	input_hidden_weights->randomize();
	this->m_vm_weights.push_back(input_hidden_weights);
	this->m_vm_deltas.push_back(Matrix<T>::duplicate_dimensions(input_hidden_weights, "Input-Hidden Deltas"));

	// For the nth hidden layers.
	for (size_t i = 0; i < m_uHiddenLayerSize - 1; i++) {
		Matrix<T>* n_hidden_layer = new Matrix<T>(hidden_layer_nodes[i + 1], hidden_layer_nodes[i]);
		n_hidden_layer->setDescription("Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weights");
		n_hidden_layer->randomize();
		this->m_vm_weights.push_back(n_hidden_layer);
		this->m_vm_deltas.push_back(Matrix<T>::duplicate_dimensions(n_hidden_layer, "Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weight Deltas"));
	}

	auto hidden_output_weights = new Matrix<T>(output_nodes, hidden_nodes);
	hidden_output_weights->setDescription("Hidden-Output Weights");
	hidden_output_weights->randomize();
	this->m_vm_weights.push_back(hidden_output_weights);
	this->m_vm_deltas.push_back(Matrix<T>::duplicate_dimensions(hidden_output_weights, "Hidden-Output Deltas"));

	// ------------------------------------------------------------------------------------
	// Product weight pre-allocations.
	// We can take advantage of the loop to also create the gradients and biases pre-allocations.
	// ------------------------------------------------------------------------------------

	auto input_hidden_product_weights = new Matrix<T>(hidden_layer_nodes[0], 1);
	input_hidden_product_weights->setDescription("Input-Hidden (Matrix Product)");
	this->m_vm_product_outputs.push_back(input_hidden_product_weights);
	this->m_vm_biases.push_back(Matrix<T>::duplicate_randomize(input_hidden_product_weights, "Input-Hidden (Matrix Product) Biases"));
	this->m_vm_gradients.push_back(Matrix<T>::duplicate_dimensions(input_hidden_product_weights, "Input-Hidden (Matrix Product) Gradients"));

	for (size_t i = 0; i < m_uHiddenLayerSize - 1; i++) {
		// Create the matrix for the nth-nth+1 layer and its corresponding biases.
		Matrix<T>* n_hidden_layer_outputs = new Matrix<T>(hidden_layer_nodes[i+1], 1);
		n_hidden_layer->setDescription("Hidden  " + std::to_string(i) + " - " + "Hidden  " + std::to_string(i+1) + " (Matrix Product)");
		this->m_vm_product_outputs.push_back(n_hidden_layer_outputs);
		this->m_vm_biases.push_back(Matrix<T>::duplicate_randomize(n_hidden_layer_outputs, "Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weight (Matrix Product) Biases"));
		this->m_vm_gradients.push_back(Matrix<T>::duplicate_dimensions(n_hidden_layer_outputs, "Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weight (Matrix Product) Gradient"));
	}

	auto hidden_output_product_weights = new Matrix<T>(output_nodes, 1);
	hidden_output_product_weights->setDescription("Hidden-Output (Matrix Product)");
	this->m_vm_dot_outputs.push_back(hidden_output_product_weights);
	this->m_vm_biases.push_back(Matrix<T>::duplicate_randomize(hidden_output_product_weights, "Hidden-Output (Matrix Product) Biases"));
	this->m_vm_gradients.push_back(Matrix<T>::duplicate_dimensions(hidden_output_product_weights, "Hidden-Output (Matrix Product) Gradients"));

	// ------------------------------------------------------------------------------------
	// Error vector matrices initialization (see README for more explanation on the pre-allocs)
	// ------------------------------------------------------------------------------------
	
	// Output layer errors.
	output_error = new Matrix<T>(1, output_nodes);
	output_error->setDescription("Output Layer Errors");
	this->m_vm_errors.push(output_error);

	// Hidden layer errors.
	for (size_t i = hidden_layer_nodes-1; i >= 0; i--) {
		// Create the matrix with the transposed row value and error matrix column value.
		nth_hidden_error = new Matrix<T>(hidden_layer_nodes[i], this->m_vm_errors[hidden_layer_nodes.size()+i]);
		nth_hidden_error->setDescription("Hidden  " + std::to_string(i) + " - " + "Hidden  " + std::to_string(i+1) + " Errors");
		this->m_vm_errors.push(nth_hidden_error);
	}

	// Input layer errors.
	input_error = new Matrix<T>(input_nodes, this->m_vm_errors[1]->getColumns());
	output_error->setDescription("Input-Hidden Layer Errors");
	this->m_vm_errors.push(input_error);

	// ------------------------------------------------------------------------------------
	// Log the event.
	// ------------------------------------------------------------------------------------

	spdlog::debug("Created Simple Neural Network {{ Input: {0:d}, Hidden: {0:d}, Output: {0:d} }}", input_nodes, hidden_nodes, output_nodes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Net Destructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
NeuralNetwork<T>::~NeuralNetwork() {
	spdlog::debug("Neural Network Destroyed.");
	for (auto &matrix_pair : this->m_lpm_weights_biases) {
		delete matrix_pair.first;
		delete matrix_pair.second;
		// delete matrix_pair;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Feed-Foward calculation units.
////////////////////////////////////////////////////////////////////////////////////////////////////

// Input-Hidden forwarding.
// ------------------------------------------------------------------------------------

void NeuralNetwork<T>::feed_forward_input_unit(std::vector<T>* inputs, std::function<T(T)> activation_function) {
	Matrix<T>::dot(this->m_vm_dot_outputs[0], this->m_vpm_weights_biases[0].first, inputs);
	this->m_vm_dot_outputs[0]->add(this->m_vpm_weights_biases[i].second);
	this->m_vm_dot_outputs[0]->map(activation_function);
}

// nHidden-n+1Hidden and nhidden-Output forwarding.
// ------------------------------------------------------------------------------------

void NeuralNetwork<T>::feed_forward_hidden_unit(std::function<T(T)> activation_function) {
	for (int i = 1; i < this->m_lvm_weights_biases.size(); ++i) {
		Matrix<T>::dot(this->m_vm_dot_outputs[i], this->m_vpm_weights_biases[i].first, this->m_vm_dot_outputs[i-1]);
		this->m_vm_dot_outputs[i]->add(this->m_vpm_weights_biases[i].second);
		this->m_vm_dot_outputs[i]->map(activation_function);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Net Feed-Foward.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::vector<T> *NeuralNetwork<T>::feed_forward(std::vector<T>* inputs) {

	// ---------------------------------------------
	// sig((w * i) + b) process for input - hidden.
	// w -> Weights.
	// i -> Inputs.
	// b -> Bias.
	// sig() -> Sigmoid function.
	// ---------------------------------------------

	// ---------------------------------------------
	// 		[ 1,1,1 ] x [ 2, 5 ] = [ 4, 7 ]
	// 		            [ 1, 1 ]
	// 		            [ 1, 1 ]
	//
	//  Result Dimensions = LHs Rows x RHs Columns
	// Condition, same rows in LHs as Columns in RHs
	// ---------------------------------------------

	// Push the input vector throuh all the neural layers with a matrix multiplacation.
	this->feed_forward_input_unit(inputs, NeuralNetwork<T>::sigmoid);

	// Iterate through all the weight matrices and push the input vector forward through
	// matrix multiplacations.
	// sig((Wi * Iv) + b)
	// ------------------------------------------------------------------------------------
	this->feed_forward_hidden_unit(NeuralNetwork<T>::sigmoid);

	// Return the outputs in a vector form (must deallocate).
	// ------------------------------------------------------------------------------------
	return Matrix<T>::toVector(outputs);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Net Training.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void NeuralNetwork<T>::train(std::vector<T>* inputs, std::vector<T>* answers) {

	// Feed Forward to calculate the errors based on the guessed values.
	// ------------------------------------------------------------------------------------
	std::vector<T>* outputs = this->feed_forward(inputs);

	// Convert the vectors to matrix types. 		TODO: Implement native vector support.
	// ------------------------------------------------------------------------------------

	Matrix<T> matrix_inputs(*inputs);
	Matrix<T> matrix_answers(*answers);
	Matrix<T> matrix_outputs(*outputs);
	delete outputs;

	// 1) Error calculation = (expected_output - guessed_output)
	// ------------------------------------------------------------------------------------
	Matrix<T>::element_wise_substraction(this->m_errors, &matrix_answers, &matrix_outputs);

	// 2) Hidden Layer Errors -> (Wh^T * (Output Erros)).
	// ------------------------------------------------------------------------------------
	Matrix<T>* ho_errors = Matrix<T>::transpose(m_hoWeights);
	ho_errors->dot(*output_errors);

	// Calculate output layer gradient (learning_rate * output_errors * dsigmoid(outputs)).
	// ------------------------------------------------------------------------------------
	matrix_outputs.map(NeuralNetwork<T>::dsigmoid);
	matrix_outputs->hadamardProduct(output_errors);
	matrix_outputs->scalarProduct(m_fLearningRate);

	// Calculate hidden layer gradient (learning_rate * hidden_errors * dsigmoid(hidden_weights_output)).
	// ------------------------------------------------------------------------------------
	Matrix<T> *hidden_gradients = Matrix<T>::map(m_HiddenOutputWeights, NeuralNetwork<T>::dsigmoid);
	hidden_gradients->hadamardProduct(ho_errors);
	hidden_gradients->scalarProduct(m_fLearningRate);
	delete ho_errors;

	// Calculte Hidden-Output deltas (learning_rate * errors * dsigmoid(outputs) * weights(T)).
	// ------------------------------------------------------------------------------------
	Matrix<T> *transposed_ho_weights = Matrix<T>::transpose(m_HiddenOutputWeights);
	Matrix<T> *ho_deltas = Matrix<T>::dot(&matrix_outputs, transposed_ho_weights);
	m_hoWeights->add(ho_deltas);
	m_oBias->add(&matrix_outputs);
	delete ho_deltas;
	delete transposed_ho_weights;

	// Calculate Input-Hidden deltas.
	// ------------------------------------------------------------------------------------
	Matrix<T> *transposed_inputs = Matrix<T>::transpose(&matrix_inputs);
	Matrix<T> *ih_deltas = Matrix<T>::dot(hidden_gradients, transposed_inputs);
	m_ihWeights->add(ih_deltas);
	m_hBias->add(hidden_gradients);
	delete hidden_gradients;
	delete transposed_inputs;
	delete ih_deltas;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Weight Printing.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void NeuralNetwork<T>::print_weights() {
	this->m_ihWeights->print();
	this->m_hoWeights->print();
}

/*################################################################################################*/
// Deep Neural Network.
/*################################################################################################*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Constructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
DeepNeuralNetwork<T>::DeepNeuralNetwork(uint_fast64_t inputLayerNodes, std::vector<uint_fast64_t> &hiddenLayerNodes, uint_fast64_t outputLayerNodes)
	: NeuralNetwork<T>::NeuralNetwork(inputLayerNodes, hiddenLayerNodes[0], outputLayerNodes)
{
	// Specify number of hidden layer present on the net.
	/********************************************************************************/
	m_uHiddenLayerSize = hiddenLayerNodes.size();

	// Reserve vector space to avoid reallocation for the nth hidden layers.
	/********************************************************************************/
	m_vErrors.reserve(m_uHiddenLayerSize);
	m_vHiddenOutputWeights.reserve(m_uHiddenLayerSize);
	m_vHWeights.reserve(m_uHiddenLayerSize - 1);
	m_vGradients.reserve(m_uHiddenLayerSize - 1);
	m_vDeltas.reserve(m_uHiddenLayerSize - 1);
	m_vBiases.reserve(m_uHiddenLayerSize + 1);

	// Fill previously reserved vectors with null pointers and weights with random values.
	/********************************************************************************/
	for (size_t i = 0; i < m_uHiddenLayerSize - 1; i++)
	{
		m_vGradients.push_back(nullptr);
		m_vDeltas.push_back(nullptr);
		Matrix<T>* nMatrix = new Matrix<T>(hiddenLayerNodes[i + 1], hiddenLayerNodes[i]);
		nMatrix->setDescription("Hidden Layers " + std::to_string(i) + " to " + std::to_string(i+1));
		m_vHWeights.push_back(nMatrix);
		m_vHWeights.at(i)->randomize();

		m_vErrors.push_back(nullptr);
		m_vHiddenOutputWeights.push_back(nullptr);
		m_vBiases.push_back(new Matrix<T>(hiddenLayerNodes[i], 1));
		m_vBiases.at(i)->randomize();
	}

	m_vHiddenOutputWeights.push_back(nullptr);
	Matrix<T>* hoMatrix = new Matrix<T>(hiddenLayerNodes[m_uHiddenLayerSize - 1], 1);
	hoMatrix->setDescription("Hidden Layer " + std::to_string(m_uHiddenLayerSize - 1) + " to output Layer");
	m_vBiases.push_back(hoMatrix);
	m_vBiases[(m_uHiddenLayerSize - 1)]->randomize();
	m_vErrors.push_back(nullptr);

	// Create nth-Hidden-Output weight Matrix.
	/********************************************************************************/
	this->m_hoWeights = new Matrix<T>(outputLayerNodes, hiddenLayerNodes[m_uHiddenLayerSize - 1]);
	this->m_hoWeights->setDescription("Last Hidden layer - Output Layer");
	this->m_hoWeights->randomize();

	// Create nth-Hidden-Output bias Matrix.
	/********************************************************************************/
	m_vBiases.push_back(new Matrix<T>(outputLayerNodes, 1));
	m_vBiases.at(m_vBiases.size() - 1)->randomize();

	spdlog::info("Created Deep Neural Network {{ Input Nodes: {}, Hidden Layers: {}, Output Nodes: {} }}", inputLayerNodes, hiddenLayerNodes.size(), outputLayerNodes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Destructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
DeepNeuralNetwork<T>::~DeepNeuralNetwork()
{
	for (auto &matrix : m_vErrors)
		delete matrix;
	for (auto &matrix : m_vGradients)
		delete matrix;
	for (auto &matrix : m_vDeltas)
		delete matrix;
	for (auto &matrix : m_vHiddenOutputWeights)
		delete matrix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net FeedForward.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::vector<T> *DeepNeuralNetwork<T>::feed_forward(std::vector<T> *inputVec)
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
	if (m_vHiddenOutputWeights[0] != nullptr)
		delete m_vHiddenOutputWeights[0];
	m_vHiddenOutputWeights[0] = Matrix<T>::dot(this->m_ihWeights, inputVec);
	m_vHiddenOutputWeights[0]->add(m_vBiases[0]);
	m_vHiddenOutputWeights[0]->map(NeuralNetwork<T>::sigmoid);

	// sig((W * i) + b) process for nth-hidden layers.
	/********************************************************************************/
	for (size_t i = 0; i < (m_uHiddenLayerSize - 1); i++)
	{
		if (m_vHiddenOutputWeights[i + 1] != nullptr)
			delete m_vHiddenOutputWeights[i + 1];
		m_vHiddenOutputWeights.at(i + 1) = Matrix<T>::dot(m_vHWeights.at(i), m_vHiddenOutputWeights.at(i));
		m_vHiddenOutputWeights.at(i + 1)->add(m_vBiases.at(i + 1));
		m_vHiddenOutputWeights.at(i + 1)->map(NeuralNetwork<T>::sigmoid);
	}

	// sig((W * i) + b) process for nth-hidden and output layer.
	/********************************************************************************/
	Matrix<T> *outputLayerInputs = Matrix<T>::dot(this->m_hoWeights, m_vHiddenOutputWeights.at(m_vHiddenOutputWeights.size() - 1));
	outputLayerInputs->add(m_vBiases.at(m_uHiddenLayerSize));
	outputLayerInputs->map(NeuralNetwork<T>::sigmoid);

	// Matrix to Vector conversion for return value.
	/********************************************************************************/
	std::vector<T> *outputVec = Matrix<T>::toVector(outputLayerInputs);

	// Memory deallocation.
	/********************************************************************************/
	delete outputLayerInputs;

	return outputVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Training.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void DeepNeuralNetwork<T>::train(std::vector<T> *vGuesses, std::vector<T> *vAnswers)
{

	// Obtain Neural Nets outputs, given a guess vector.
	/********************************************************************************/
	std::vector<T> *vOutputs = this->feed_forward(vGuesses);

	// Vector to Matrix conversion for Matrix operations.
	/********************************************************************************/

	Matrix<T> inputs(*vGuesses);
	Matrix<T> answers(*vAnswers);
	Matrix<T> outputs(*vOutputs);

	// Error calculation. (Answers - Outputs)
	/********************************************************************************/
	Matrix<T> *outputErrors = Matrix<T>::elementWiseSubstraction(&answers, &outputs);

	// Hidden-Output error calculation. (Backpropagation)
	/********************************************************************************/
	this->m_vpm_weights_biases[this->mu_total_layers-2];

	Matrix<T> *tHOWeights = Matrix<T>::transpose(this->m_hoWeights);
	if (m_vErrors[m_uHiddenLayerSize - 1] != nullptr)
		delete m_vErrors[m_uHiddenLayerSize - 1];
	m_vErrors.at(m_uHiddenLayerSize - 1) = Matrix<T>::dot(tHOWeights, outputErrors);

	for (size_t i = m_uHiddenLayerSize - 1; i > 0; i--)
	{
		if (m_vErrors[i - 1] != nullptr)
			delete m_vErrors[i - 1];
		Matrix<T> *tHiddenWHN = Matrix<T>::transpose(m_vHWeights.at(i - 1));
		m_vErrors.at(i - 1) = Matrix<T>::dot(tHiddenWHN, m_vErrors.at(i));
		delete tHiddenWHN;
	}

	// Output layer gradient calculation. (learning_rate * outputErrors * dsigmoid(outputs))
	/********************************************************************************/
	Matrix<T> *outputGradient = &outputs;
	outputGradient->map(NeuralNetwork<T>::dsigmoid);
	outputGradient->hadamardProduct(outputErrors);
	outputGradient->scalarProduct(this->m_fLearningRate);
	m_vBiases.at(m_vBiases.size() - 1)->add(outputGradient);

	// Hidden-(Hidden-Output) gradient calculation. (learning_rate * outputErrors * dsigmoid(salidas))
	/********************************************************************************/
	for (size_t i = m_uHiddenLayerSize - 1; i > 0; i--)
	{
		if (m_vGradients[i - 1] != nullptr)
			delete m_vGradients[i - 1];
		m_vGradients.at(i - 1) = Matrix<T>::map(m_vHiddenOutputWeights.at(m_uHiddenLayerSize - i), NeuralNetwork<T>::dsigmoid);
		m_vGradients.at(i - 1)->hadamardProduct(m_vErrors.at(m_uHiddenLayerSize - i));
		m_vGradients.at(i - 1)->scalarProduct(this->m_fLearningRate);
		m_vBiases.at(m_uHiddenLayerSize - i)->add(m_vGradients.at(i - 1));
	}

	// Input-Hidden gradient calculation. (learning_rate * outputErrors * dsigmoid(hidden_weights_output))
	/********************************************************************************/
	Matrix<T> *ihGradient = Matrix<T>::map(this->m_vHiddenOutputWeights.at(0), NeuralNetwork<T>::dsigmoid);
	ihGradient->hadamardProduct(m_vErrors.at(0));
	ihGradient->scalarProduct(this->m_fLearningRate);
	m_vBiases.at(0)->add(ihGradient);

	// Hidden-Output deltas calculation.
	/********************************************************************************/
	Matrix<T> *tHiddenOutputWeights = Matrix<T>::transpose(m_vHiddenOutputWeights.at(m_vHiddenOutputWeights.size() - 1));
	Matrix<T> *hoDeltaWeights = Matrix<T>::dot(outputGradient, tHiddenOutputWeights);
	this->m_hoWeights->add(hoDeltaWeights);

	// Hidden-(Hidden-Output) deltas calculation.
	/********************************************************************************/
	for (size_t i = m_uHiddenLayerSize - 1; i > 0; i--)
	{
		Matrix<T> *tGradient = Matrix<T>::transpose(m_vGradients.at(i - 1));
		if (m_vDeltas[i - 1] != nullptr)
			delete m_vDeltas[i - 1];
		m_vDeltas.at(i - 1) = Matrix<T>::dot(m_vHiddenOutputWeights.at(i), tGradient);
		this->m_vHWeights.at(i - 1)->add(m_vDeltas.at(i - 1));
		delete tGradient;
	}

	// Input-Hidden deltas calculation.
	/********************************************************************************/
	inputs.transpose();
	Matrix<T> *ihDeltaWeights = Matrix<T>::dot(ihGradient, &inputs);
	this->m_ihWeights->add(ihDeltaWeights);

	// Memory deallocation.
	/********************************************************************************/
	delete vOutputs;
	delete outputErrors;
	delete tHOWeights;
	delete tHiddenOutputWeights;
	delete ihGradient;
	delete ihDeltaWeights;
	delete hoDeltaWeights;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Neural Net Weight Printer.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void DeepNeuralNetwork<T>::print_weights() {
	this->m_ihWeights->print();
	for(auto weight : this->m_vHWeights) {
		weight->print();
	}
	this->m_hoWeights->print();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Nets Template Specialization.
////////////////////////////////////////////////////////////////////////////////////////////////////

template class NeuralNetwork<float>;
template class DeepNeuralNetwork<float>;