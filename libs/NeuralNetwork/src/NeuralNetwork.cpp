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
	// Reserve pre-allocations vectors sized to prevent array re-sizing.
	// ------------------------------------------------------------------------------------

	unsigned common_size = this->mu_total_layers-1;
	this->m_vm_biases->reserve(common_size);
	this->m_vm_deltas->reserve(common_size);
	this->m_vm_errors->reserve(common_size);
	this->m_vm_weights->reserve(common_size);
	this->m_vm_gradients->reserve(common_size);
	this->m_vm_product_outputs->reserve(common_size);

	// ------------------------------------------------------------------------------------
	// Create Input-Hidden weight matrices and randomize its initial values.
	// ------------------------------------------------------------------------------------

	auto input_hidden_weights = new Matrix<T>(hidden_nodes, input_nodes);
	input_hidden_weights->setDescription("Input-Hidden Weights");
	input_hidden_weights->randomize();
	this->m_vpm_weights_weights_t.push_back(std::make_pair(input_hidden_weights, Matrix<T>::duplicate_transpose(input_hidden_weights, "Transposed Input-Hidden Weights")));
	this->m_vm_deltas.push_back(Matrix<T>::duplicate_dimensions(input_hidden_weights, "Input-Hidden Deltas"));

	// ------------------------------------------------------------------------------------
	// Input-Hidden feed-forward product weights, biases errors and gradients pre-allocations.
	// ------------------------------------------------------------------------------------

	auto input_hidden_product_weights = new Matrix<T>(hidden_layer_nodes[0], 1);
	input_hidden_product_weights->setDescription("Input-Hidden (Matrix Product)");
	this->m_vm_product_outputs.push_back(std::make_pair(input_hidden_product_weights, Matrix<T>::duplicate_transpose(input_hidden_product_weights, "Transposed Input-Hidden (Matrix Product) Weights")));
	this->m_vm_biases.push_back(Matrix<T>::duplicate_randomize(input_hidden_product_weights, "Input-Hidden (Matrix Product) Biases"));
	this->m_vm_errors.push_back(Matrix<T>::duplicate_randomize(input_hidden_product_weights, "Input-Hidden Errors"));
	this->m_vm_gradients.push_back(Matrix<T>::duplicate_dimensions(input_hidden_product_weights, "Input-Hidden (Matrix Product) Gradients"));

	// ------------------------------------------------------------------------------------
	// Take advantage of the loop to also create the errors, gradients, deltas and biases pre-allocations.
	// ------------------------------------------------------------------------------------

	// For the nth hidden layers.
	for (size_t i = 0; i < m_uHiddenLayerSize - 1; i++) {
		Matrix<T>* n_hidden_layer = new Matrix<T>(hidden_layer_nodes[i + 1], hidden_layer_nodes[i]);
		n_hidden_layer->setDescription("Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weights");
		n_hidden_layer->randomize();
		this->m_vpm_weights_weights_t.push_back(std::make_pair(n_hidden_layer, Matrix<T>::duplicate_transpose(n_hidden_layer, "Transposed Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weights")));
		this->m_vm_deltas.push_back(Matrix<T>::duplicate_dimensions(n_hidden_layer, "Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weight Deltas"));

		Matrix<T>* n_hidden_layer_outputs = new Matrix<T>(hidden_layer_nodes[i+1], 1);
		n_hidden_layer->setDescription("Hidden  " + std::to_string(i) + " - " + "Hidden  " + std::to_string(i+1) + " (Matrix Product)");
		this->m_vm_product_outputs(std::make_pair(n_hidden_layer_outputs, Matrix<T>::duplicate_transpose(n_hidden_layer_outputs, "Transposed Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weight (Matrix Product)")));
		this->m_vm_errors.push_back(Matrix<T>::duplicate_randomize(n_hidden_layer_outputs, "Hidden  " + std::to_string(i) + " - " + std::to_string(i+1) + " Weight (Matrix Product) Errors"));
		this->m_vm_biases.push_back(Matrix<T>::duplicate_randomize(n_hidden_layer_outputs, "Hidden  " + std::to_string(i) cate_dimensions(n_hidden_layer_outputs, "Hidden  " + std::to_stri+ " - " + std::to_string(i+1) + " Weight (Matrix Product) Biases"));
		this->m_vm_gradients.push_back(Matrix<T>::dupling(i) + " - " + std::to_string(i+1) + " Weight (Matrix Product) Gradient"));
	}

	// ------------------------------------------------------------------------------------
	// Create Hidden-Output weight matrices and randomize its initial values.
	// ------------------------------------------------------------------------------------

	auto hidden_output_weights = new Matrix<T>(output_nodes, hidden_nodes);
	hidden_output_weights->setDescription("Hidden-Output Weights");
	hidden_output_weights->randomize();
	this->m_vpm_weights_weights_t.push_back(std::make_pair(hidden_output_weights, Matrix<T>::duplicate_transpose(hidden_output_weights, "Transposed Hidden-Output Weights")));
	this->m_vm_deltas.push_back(Matrix<T>::duplicate_dimensions(hidden_output_weights, "Hidden-Output Deltas"));

	// ------------------------------------------------------------------------------------
	// Hidden-Output product weight, biases and gradients pre-allocations.
	// ------------------------------------------------------------------------------------

	auto hidden_output_product_weights = new Matrix<T>(output_nodes, 1);
	hidden_output_product_weights->setDescription("Hidden-Output (Matrix Product)");
	this->m_vm_product_outputs.push_back(std::make_pair(hidden_output_product_weights, Matrix<T>::duplicate_transpose(hidden_output_product_weights,"Transposed Input-Hidden (Matrix Product) Weights")));
	this->m_vm_biases.push_back(Matrix<T>::duplicate_randomize(hidden_output_product_weights, "Hidden-Output (Matrix Product) Biases"));
	this->m_vm_errors.push_back(Matrix<T>::duplicate_randomize(hidden_output_product_weights, "Hidden-Output Errors"));
	this->m_vm_gradients.push_back(Matrix<T>::duplicate_dimensions(hidden_output_product_weights, "Hidden-Output (Matrix Product) Gradients"));

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
	Matrix<T>::dot(this->m_vm_product_outputs[0].first, this->m_vpm_weights_biases[0].first, inputs);
	this->m_vm_product_outputs[0].first->add(this->m_vpm_weights_biases[i].second);
	this->m_vm_product_outputs[0].first->map(activation_function);
}

// nHidden-n+1Hidden and nhidden-Output forwarding.
// ------------------------------------------------------------------------------------

void NeuralNetwork<T>::feed_forward_hidden_unit(std::function<T(T)> activation_function) {
	for (int i = 1; i < this->m_lvm_weights_biases.size(); ++i) {
		Matrix<T>::dot(this->m_vm_product_outputs[i].first, this->m_vpm_weights_biases[i].first, this->m_vm_dot_outputs[i-1]);
		this->m_vm_product_outputs[i].first->add(this->m_vpm_weights_biases[i].second);
		this->m_vm_product_outputs[i].first->map(activation_function);
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

	// ------------------------------------------------------------------------------------
	// Feed Forward to calculate the errors based on the guessed values.
	// ------------------------------------------------------------------------------------

	std::vector<T>* outputs = this->feed_forward(inputs);

	// ------------------------------------------------------------------------------------
	// Convert the vectors to matrix types. 		TODO: Implement native vector support.
	// ------------------------------------------------------------------------------------

	Matrix<T> matrix_inputs(*inputs);
	Matrix<T> matrix_answers(*answers);
	Matrix<T> matrix_outputs(*outputs);
	delete outputs;

	// ------------------------------------------------------------------------------------
	// Save calculation times by calculating the following useful variables.
	// ------------------------------------------------------------------------------------

	unsigned hidden_layers = this->mvu_hidden_layer_nodes.size();
	unsigned indexable_total_layers = this->mu_total_layers-1;
	unsigned weigth_matrices_count = indexable_total_layers-1;

	// ------------------------------------------------------------------------------------
	// Output Error calculation = (expected_output - guessed_output)
	// ------------------------------------------------------------------------------------

	Matrix<T>::element_wise_substraction(this->m_vm_errors[weigth_matrices_count], &matrix_answers, &matrix_outputs);

	// ------------------------------------------------------------------------------------
	// Hidden Layers Error Calculations.
	// ------------------------------------------------------------------------------------

	for(size_t i = weigth_matrices_count; i > 0; i--) {
		Matrix<T>::transpose(this->m_vpm_weights_weights_t[i].second, this->m_vpm_weights_weights_t[i].first);
		Matrix<T>::dot(this->m_vm_errors[i-1], this->m_vpm_weights_weights_t[i].second, this->m_vm_errors[i]);
	}

	// ------------------------------------------------------------------------------------
	// Gradient calculation. (learning_rate * output_errors * dsigmoid(outputs))
	// ------------------------------------------------------------------------------------

	for (size_t i = weigth_matrices_count; i >= 0; i--) {
		Matrix<T>::map(this->m_vm_gradients[i], this->m_vm_product_outputs[i].first, NeuralNetwork<T>::dsigmoid);
		this->m_vm_gradients[i]->hadamardProduct(this->m_vm_errors[i]);
		this->m_vm_gradients[i]->scalarProduct(this->mf_learning_rate);
		this->m_vm_biases[i]->add(this->m_vm_gradients[i]);
	}
	
	// ------------------------------------------------------------------------------------
	// Deltas calculation.
	// ------------------------------------------------------------------------------------

	for (size_t i = weigth_matrices_count; i > 0; i--) {
		Matrix<T>::transpose(this->m_vm_product_outputs[i-1].second, this->m_vm_product_outputs[i-1].first);
		Matrix<T>::dot(this->m_vm_deltas[i], this->m_vm_gradients[i], this->m_vm_product_outputs[i-1].second);
		this->m_vpm_weights_weights_t[i].first->add(this->m_vm_deltas[i]);
	}

	// Input-Hidden deltas calculation.
	matrix_inputs.transpose();
	Matrix<T>::dot(this->m_vm_deltas[0], this->m_vm_gradients[0], &matrix_inputs);
	this->m_vpm_weights_weights_t[0].first->add(this->m_vm_deltas[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Weight Printing.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void NeuralNetwork<T>::print_weights() {
	this->m_ihWeights->print();
	this->m_hoWeights->print();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Template Specialization.
////////////////////////////////////////////////////////////////////////////////////////////////////

template class NeuralNetwork<float>;