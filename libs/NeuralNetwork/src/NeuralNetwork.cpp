#include <include/NeuralNetwork.hpp>
#include "spdlog/spdlog.h"

// Uploaded by panchis7u7 ~ Sebastian Madrigal

/*################################################################################################*/
// Simple Neural Network.
/*################################################################################################*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Constructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
NeuralNetwork<T>::NeuralNetwork(unsigned inputLayerNodes, unsigned hiddenLayerNodes, unsigned outputLayerNodes)
{
	m_uInputLayerNodes = inputLayerNodes;
	m_uHiddenLayerNodes = hiddenLayerNodes;
	m_uOutputLayerNodes = outputLayerNodes;

	// Create matrices based upon the number of nodes supplied.
	/********************************************************************************/
	m_ihWeights = new Matrix<T>(m_uHiddenLayerNodes, m_uInputLayerNodes);
	m_hoWeights = new Matrix<T>(m_uOutputLayerNodes, m_uHiddenLayerNodes);

	// Initialize random values into the weights matrices.
	/********************************************************************************/
	m_ihWeights->randomize();
	m_hoWeights->randomize();

	// Create bias matrices based upon the number of nodes supplied.
	/********************************************************************************/
	m_hBias = new Matrix<T>(m_uHiddenLayerNodes, 1);
	m_oBias = new Matrix<T>(m_uOutputLayerNodes, 1);

	// Initialize random values into the bias matrices.
	/********************************************************************************/
	m_hBias->randomize();
	m_oBias->randomize();
	m_HiddenOutputWeights = nullptr;

	spdlog::info("Created Simple Neural Network {{ Input: {0:d}, Hidden: {0:d}, Output: {0:d} }}", inputLayerNodes, hiddenLayerNodes, outputLayerNodes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Destructor.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
NeuralNetwork<T>::~NeuralNetwork()
{
	spdlog::debug("Neural Network Destroyed.");
	delete (m_ihWeights);
	delete (m_hoWeights);
	delete (m_hBias);
	delete (m_oBias);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net FeedFoward.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::vector<T> *NeuralNetwork<T>::feedForward(std::vector<T> *vInputs)
{

	////////////////////////////////////////////////
	// sig((W * i) + b) process for input - hidden.
	// W -> Weights.
	// i -> Inputs.
	// b -> Bias.
	// sig -> Sigmoid function.
	////////////////////////////////////////////////
	/********************************************************************************/

	if (m_HiddenOutputWeights)
		delete m_HiddenOutputWeights;
	m_HiddenOutputWeights = Matrix<T>::dot(m_ihWeights, vInputs);
	m_HiddenOutputWeights->add(m_hBias);
	m_HiddenOutputWeights->map(NeuralNetwork<T>::sigmoid);

	// sig((W * i) + b) process for hidden - output.
	/********************************************************************************/
	Matrix<T> *outputs = Matrix<T>::dot(m_hoWeights, m_HiddenOutputWeights);
	outputs->add(m_oBias);
	outputs->map(NeuralNetwork<T>::sigmoid);

	// Vector conversion.
	/********************************************************************************/
	std::vector<T> *vOutputs = Matrix<T>::toVector(outputs);

	// Memory deallocation.
	/********************************************************************************/
	delete outputs;

	return vOutputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Training.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void NeuralNetwork<T>::train(std::vector<T> *vInputs, std::vector<T> *vAnswers)
{

	// Convert vector to a matrix type. 		TODO: Implement native vector support.
	/********************************************************************************/
	std::vector<T> *vOutputs = this->feedForward(vInputs);

	Matrix<T> mInputs(*vInputs);
	Matrix<T> mAnswers(*vAnswers);
	Matrix<T> mOutputs(*vOutputs);

	// Error calculation.
	// 1) Output Errors -> (respuestas -  salidas).
	/********************************************************************************/
	Matrix<T> *mOutputErrors = Matrix<T>::elementWiseSubstraction(&mAnswers, &mOutputs);

	// 2) Hidden Layer Errors -> (Wh^T * (Output Erros)).
	/********************************************************************************/
	Matrix<T> *mHoErrors = Matrix<T>::transpose(m_hoWeights);
	mHoErrors->dot(*mOutputErrors);

	// Calculate output layer gradient (learning_rate * outputErrors * dsigmoid(salidas)).
	/********************************************************************************/
	Matrix<T> *mOutputGradients = Matrix<T>::map(&mOutputs, NeuralNetwork<T>::dsigmoid);
	mOutputGradients->hadamardProduct(mOutputErrors);
	mOutputGradients->scalarProduct(m_fLearningRate);

	// Calculate hidden layer gradient (learning_rate * HiddenErrors * dsigmoid(hidden_weights_output)).
	/********************************************************************************/
	Matrix<T> *mHiddenGradients = Matrix<T>::map(m_HiddenOutputWeights, NeuralNetwork<T>::dsigmoid);
	mHiddenGradients->hadamardProduct(mHoErrors);
	mHiddenGradients->scalarProduct(m_fLearningRate);

	// Calculte Hidden-Output deltas (learning_rate * errors * dsigmoid(salidas) * weights(T)).
	/********************************************************************************/
	Matrix<T> *tHiddenOutputWeights = Matrix<T>::transpose(m_HiddenOutputWeights);
	Matrix<T> *mHoDeltas = Matrix<T>::dot(mOutputGradients, tHiddenOutputWeights);
	m_hoWeights->add(mHoDeltas);
	m_oBias->add(mOutputGradients);

	// Calculate Input-Hidden deltas.
	/********************************************************************************/
	Matrix<T> *tInputs = Matrix<T>::transpose(&mInputs);
	Matrix<T> *mIhDeltas = Matrix<T>::dot(mHiddenGradients, tInputs);
	m_ihWeights->add(mIhDeltas);
	m_hBias->add(mHiddenGradients);

	// Memory deallocation.
	/********************************************************************************/

	delete vOutputs;
	delete mOutputErrors;
	delete mOutputGradients;
	delete mHiddenGradients;
	delete tHiddenOutputWeights;
	delete tInputs;
	delete mHoErrors;
	delete mIhDeltas;
	delete mHoDeltas;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Neural Net Weight Printing.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void NeuralNetwork<T>::printWeights()
{
	std::cout << "---- [Input - Hidden Layer Weights] ----"
			  << "\n\n"
			  << m_ihWeights << std::endl;
	std::cout << "---- [Hidden - Output Layer Weights] ----"
			  << "\n\n"
			  << m_hoWeights << std::endl;
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
		m_vHWeights.push_back(new Matrix<T>(hiddenLayerNodes[i + 1], hiddenLayerNodes[i]));
		m_vHWeights.at(i)->randomize();

		m_vErrors.push_back(nullptr);
		m_vHiddenOutputWeights.push_back(nullptr);
		m_vBiases.push_back(new Matrix<T>(hiddenLayerNodes[i], 1));
		m_vBiases.at(i)->randomize();
	}

	m_vHiddenOutputWeights.push_back(nullptr);
	m_vBiases.push_back(new Matrix<T>(hiddenLayerNodes[m_uHiddenLayerSize - 1], 1));
	m_vBiases[(m_uHiddenLayerSize - 1)]->randomize();
	m_vErrors.push_back(nullptr);

	// Create nth-Hidden-Output weight Matrix.
	/********************************************************************************/
	this->m_hoWeights = new Matrix<T>(outputLayerNodes, hiddenLayerNodes[m_uHiddenLayerSize - 1]);
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
	std::vector<T> *vOutputs = this->feedForward(vGuesses);

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
inline void DeepNeuralNetwork<T>::printWeights()
{
	std::cout << "---- [Input - Hidden[0] Layer Weights] ----"
			  << "\n\n"
			  << this->m_ihWeights << std::endl;
	std::cout << "---- [Hidden (n - n+1) Layer Weights] ----"
			  << "\n\n";
	int layer = 0;

	// nth Hidden wieghts matrix print.
	/********************************************************************************/
	for (auto nth_hidden_weights : this->m_vHWeights)
	{
		std::cout << "#### Layer " << layer << " ####"
				  << "\n\n"
				  << nth_hidden_weights;
		layer++;
	}
	std::cout << "---- [Hidden[n-1] - Output Layer Weights] ----"
			  << "\n\n"
			  << this->m_hoWeights << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Neural Nets Template Specialization.
////////////////////////////////////////////////////////////////////////////////////////////////////

template class NeuralNetwork<float>;
template class DeepNeuralNetwork<float>;