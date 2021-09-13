#include <include/NeuralNetwork.hpp>
#include <memory>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

template <typename T>
NeuralNetwork<T>::NeuralNetwork(int inputNodes, int hiddenNodes, int outputNodes) {
	this->inputLayerNodes = inputNodes;
	this->hiddenLayerNodes = hiddenNodes;
	this->outputLayerNodes = outputNodes;
	// Create matrices based upon the number of nodes supplied..
	this->pesos_ih = new Matrix<T>(this->hiddenLayerNodes, this->inputLayerNodes);
	this->pesos_ho = new Matrix<T>(this->outputLayerNodes, this->hiddenLayerNodes);
	// Initialize random values into the weights matrices.
	this->pesos_ih->randomize();
	this->pesos_ho->randomize();
	// Create bias matrices based upon the number of nodes supplied.
	this->bias_h = new Matrix<T>(this->hiddenLayerNodes, 1);
	this->bias_o = new Matrix<T>(this->outputLayerNodes, 1);
	// Initialize random values into the bias matrices.
	this->bias_h->randomize();
	this->bias_o->randomize();
	this->hidden_weights_output = nullptr;
}

template <typename T>
NeuralNetwork<T>::~NeuralNetwork() {
	delete(this->pesos_ih);
	delete(this->pesos_ho);
	delete(this->bias_h);
	delete(this->bias_o);
}

template <typename T>
std::vector<T>* NeuralNetwork<T>::feedForward(std::vector<T>* inputVec) {

	//Convert vector to a matrix type. 		TODO: Implement native vector support.
	//Matrix<T>* inputs = Matrix<T>::fromVector(inputVec);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// sig((W * i) + b) process for input - hidden.
	// W -> Weights.
	// i -> Inputs.
	// b -> Bias.
	// sig -> Sigmoid function.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	this->hidden_weights_output = Matrix<T>::dot(this->pesos_ih, inputVec);
	this->hidden_weights_output->add(this->bias_h);
	this->hidden_weights_output->map(NeuralNetwork<T>::sigmoid);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// sig((W * i) + b) process for hidden - output.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Matrix<T>* outputs = Matrix<T>::dot(this->pesos_ho, this->hidden_weights_output);
	outputs->add(this->bias_o);
	outputs->map(NeuralNetwork<T>::sigmoid);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vector conversion.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<T>* outputVec = Matrix<T>::toVector(outputs);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Memory deallocation.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	delete outputs;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	return outputVec;
}

template <typename T>
void NeuralNetwork<T>::train(std::vector<T>* vec_entradas, std::vector<T>* vec_respuestas) {

	// Convert vector to a matrix type. 		TODO: Implement native vector support.
	std::vector<T>* outputsVec = this->feedForward(vec_entradas);
	Matrix<T>* inputs = Matrix<T>::fromVector(vec_entradas);
	Matrix<T>* answers = Matrix<T>::fromVector(vec_respuestas);
	Matrix<T>* outputs = Matrix<T>::fromVector(outputsVec);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Error calculation.
	// 1) Output Errors -> (respuestas -  salidas).
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Matrix<T>* outputErrors = Matrix<T>::elementWiseSubstraction(answers, outputs);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2) Hidden Layer Errors -> (Wh^T * (Output Erros)).	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Matrix<T>* ho_errors = Matrix<T>::transpose(pesos_ho);
	ho_errors->dot(*outputErrors);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Calcular el gradiente de la capa de salida = learning_rate * errores_salida * dsigmoid(salidas)
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Matrix<T>* output_gradients = Matrix<T>::map(outputs, NeuralNetwork<T>::dsigmoid);
	output_gradients->hadamardProduct(outputErrors);
	output_gradients->scalarProduct(this->learning_rate);

	//Calcular los gradientes de la capa oculta = learning_rate * errores_capa_oculta * dsigmoid(hidden_weights_output)
	Matrix<T>* hidden_gradients = Matrix<T>::map(this->hidden_weights_output, NeuralNetwork<T>::dsigmoid);
	hidden_gradients->hadamardProduct(ho_errors);
	hidden_gradients->scalarProduct(this->learning_rate);

	//Calcular deltas de la capa oculta-salida
	//pesos_delta = learning_rate * errores * dsigmoid(salidas) * pesos(T)
	Matrix<T>* hidden_weights_output_T = Matrix<T>::transpose(this->hidden_weights_output);
	Matrix<T>* deltas_pesos_ho = Matrix<T>::dot(gradiente_salida, hidden_weights_output_T);
	this->pesos_ho->add(deltas_pesos_ho);
	this->bias_o->add(gradiente_salida);

	//Calcular deltas de la capa de entrada-oculta
	Matrix<T>* inputs_T = Matrix<T>::transpose(inputs);
	Matrix<T>* deltas_pesos_ih = Matrix<T>::dot(gradientes_capa_oculta, inputs_T);
	this->pesos_ih->add(deltas_pesos_ih);
	this->bias_h->add(gradientes_capa_oculta);

	delete inputs;
	delete answers;
	delete outputs;
	delete outputsVec;
	delete outputErrors;
	delete gradiente_salida;
	delete gradientes_capa_oculta;
	delete hidden_weights_output_T;
	delete inputs_T;
	delete deltas_pesos_ih;
	delete deltas_pesos_ho;

}

template <typename T>
inline void NeuralNetwork<T>::printWeights() {
	std::cout << "---- [Input - Hidden Layer Weights] ----" << "\n\n" << this->pesos_ih << std::endl;
	std::cout << "---- [Hidden - Output Layer Weights] ----" << "\n\n" << this->pesos_ho << std::endl;
}

template<typename T>
DeepNeuralNetwork<T>::DeepNeuralNetwork(uint_fast64_t input, std::vector<uint_fast64_t>& hidden, uint_fast64_t output): 
	NeuralNetwork<T>::NeuralNetwork(input, hidden[0], output){

	this->hiddenLayerSize = hidden.size();

	//Reserve vector space to avoid reallocating for the nth hidden layers.
	this->pesos_hn.reserve(this->hiddenLayerSize - 1);
	this->gradientes.reserve(this->hiddenLayerSize - 1);
	this->deltas.reserve(this->hiddenLayerSize - 1);
	this->salidas_capas_ocultas.reserve(this->hiddenLayerSize);
	this->errores.reserve(this->hiddenLayerSize);
	this->bias.reserve(this->hiddenLayerSize+1);

	for (size_t i = 0; i < this->hiddenLayerSize; i++)
	{
		this->errores.push_back(nullptr);
		this->salidas_capas_ocultas.push_back(nullptr);
		this->bias.push_back(new Matrix<T>(hidden[i], 1));
		this->bias.at(i)->randomize();
	}

	//reserva espacio en memoria para los sesgos de las n capas ocultas y de salida de la red (Optimizacion, evita la redimiension del vector cada vez que se inserta).
	
	for (size_t i = 0; i < this->hiddenLayerSize-1; i++)
	{
		this->gradientes.push_back(nullptr);
		this->deltas.push_back(nullptr);
		this->pesos_hn.push_back(new Matrix<T>(hidden[i+1], hidden[i]));
		this->pesos_hn.at(i)->randomize();
	}

	//Matriz que representa los pesos entre las capa enesima(oculta)-Salida y se aleatoriza
	this->pesos_ho = new Matrix<T>(this->outputLayerNodes, hidden[this->hiddenLayerSize - 1]);
	this->pesos_ho->randomize();
	//Se asigna un sesgo o predisposicion a la enesima capa oculta y se aleatoriza
	//this->bias.push_back(new Matrix<float>(h[h.size() - 1], 1));
	//this->bias.at(bias.size() - 1)->randomize();
	//Matriz que representa el sesgo de la capa oculta y se aleatoriza
	this->bias.push_back(new Matrix<T>(this->outputLayerNodes, 1));
	this->bias.at(bias.size()- 1)->randomize();
}

template <typename T>
DeepNeuralNetwork<T>::~DeepNeuralNetwork(){
	for (auto& matrix : this->errores) {
		delete matrix;
	}
	for (auto& matrix : this->gradientes) {
		delete matrix;
	}
	for (auto& matrix : this->deltas) {
		delete matrix;
	}
	for (auto& matrix : this->salidas_capas_ocultas) {
		delete matrix;
	}
}

template <typename T>
std::vector<T>* DeepNeuralNetwork<T>::feedForward(std::vector<T>* inputData) {
//----Generando las salidas de la capas ocultas----
	//Convierte el vector de entrada a una matriz
	Matrix<T>* entradas = Matrix<T>::fromVector(inputData);
	//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas
	Matrix<T>* entradas_capa_oculta = Matrix<T>::dot(this->pesos_ih, entradas);
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_oculta->add(this->bias.at(0));
	//sig((W * i) + b) se aplica la funcion sigmoide
	////this->salidas_capas_ocultas.push_back(Matrix<float>::map(entradas_capa_oculta, sigmoid));

	this->salidas_capas_ocultas.at(0) = Matrix<T>::map(entradas_capa_oculta, NeuralNetwork<T>::sigmoid);
	//Se multiplica la matriz de pesos entre la capas ocultas y la matriz de entradas previas
	for (size_t i = 0; i < (hiddenLayerSize-1); i++)
	{
		//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas de la enesima capa oculta
		////this->salidas_capas_ocultas.push_back(Matrix<float>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i)));
		this->salidas_capas_ocultas.at(i+1) = Matrix<T>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i));
		//Al resultado de la multiplicacion se le agrega el sesgo
		this->salidas_capas_ocultas.at(i+1)->add(this->bias.at(i+1));
		//sig((W * i) + b) se aplica la funcion sigmoide
		this->salidas_capas_ocultas.at(i+1)->map(NeuralNetwork<T>::sigmoid);
	}
	//----Generando las salida----
	//Se multiplica la matriz de pesos entre la capa de salida y la matriz de salidas de la capa oculta
	Matrix<T>* entradas_capa_salida = Matrix<T>::dot(this->pesos_ho, this->salidas_capas_ocultas.at(salidas_capas_ocultas.size()-1));
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_salida->add(bias.at(hiddenLayerSize));
	//sig((W * i) * b) se aplica la funcion sigmoide
	Matrix<T>* salidas = Matrix<T>::map(entradas_capa_salida, NeuralNetwork<T>::sigmoid);
	
	std::vector<T>* outputVec = Matrix<T>::toVector(salidas);

	delete entradas;
	delete entradas_capa_oculta;
	delete entradas_capa_salida;
	delete salidas;

	return outputVec;
}

template <typename T>
void DeepNeuralNetwork<T>::train(std::vector<T>* guess, std::vector<T>* answers){
	std::vector<T>* vec_salidas = this->feedForward(guess);
	//Convertir vectores a matrices
	Matrix<T>* entradas = Matrix<T>::fromVector(guess);
	Matrix<T>* respuestas = Matrix<T>::fromVector(answers);
	Matrix<T>* salidas = Matrix<T>::fromVector(vec_salidas);
	//Calcular el error => respuestas - salidas
	Matrix<T>* errores_salida = Matrix<T>::elementWiseSubstraction(respuestas, salidas);

	//Calcular los errores de la capa oculta->salida 
	
	this->errores.at(hiddenLayerSize-1) = Matrix<T>::dot(Matrix<T>::transpose(this->pesos_ho), errores_salida);
	for (size_t i = hiddenLayerSize-1; i > 0; i--)
	{
		this->errores.at(i-1) = Matrix<T>::dot(Matrix<T>::transpose(this->pesos_hn.at(i - 1)), errores.at(i));
	}

	//Calcular el gradiente de la capa de salida = learning_rate * errores_salida * dsigmoid(salidas)
	//Matrix<T>* gradiente_salida = Matrix<T>::map(salidas, NeuralNetwork<T>::dsigmoid);
	Matrix<T>* gradiente_salida = salidas;
	gradiente_salida->map(NeuralNetwork<T>::dsigmoid);
	gradiente_salida->hadamardProduct(errores_salida);
	gradiente_salida->scalarProduct(this->learning_rate);
	this->bias.at(bias.size() - 1)->add(gradiente_salida);

	//Calcular el gradiente de las capas ocultas y oculta-salida = learning_rate * errores_salida * dsigmoid(salidas)
	for (size_t i = hiddenLayerSize-1; i > 0; i--)
	{
		this->gradientes.at(i-1) = Matrix<T>::map(this->salidas_capas_ocultas.at(hiddenLayerSize-i), NeuralNetwork<T>::dsigmoid);
		this->gradientes.at(i-1)->hadamardProduct(errores.at(hiddenLayerSize-i));
		this->gradientes.at(i-1)->scalarProduct(this->learning_rate);
		this->bias.at(hiddenLayerSize-i)->add(gradientes.at(i-1));
	}

	//Calcular los gradientes de la capa entada-oculta = learning_rate * errores_capa_oculta * dsigmoid(hidden_weights_output)
	Matrix<T>* gradiente_entrada_oculta = Matrix<T>::map(this->salidas_capas_ocultas.at(0), NeuralNetwork<T>::dsigmoid);
	gradiente_entrada_oculta->hadamardProduct(errores.at(0));
	gradiente_entrada_oculta->scalarProduct(this->learning_rate);
	this->bias.at(0)->add(gradiente_entrada_oculta);

	//Calcular deltas de la capa oculta-salida
	Matrix<T>* salidas_capas_ocultas_T = Matrix<T>::transpose(this->salidas_capas_ocultas.at(salidas_capas_ocultas.size()-1));
	Matrix<T>* deltas_pesos_ho = Matrix<T>::dot(gradiente_salida, salidas_capas_ocultas_T);
	this->pesos_ho->add(deltas_pesos_ho);
	
	for (size_t i = hiddenLayerSize - 1; i > 0; i--)
	{
		Matrix<T>* gradientes_T = Matrix<T>::transpose(gradientes.at(i - 1));
		//this->deltas.at(i-1) = Matrix<float>::dot(gradientes.at(i-1), Matrix<float>::transpose(this->salidas_capas_ocultas.at(i)));
		this->deltas.at(i-1) = Matrix<T>::dot(this->salidas_capas_ocultas.at(i), gradientes_T);
		this->pesos_hn.at(i-1)->add(deltas.at(i-1));
		delete gradientes_T;
	}

	//Calcular deltas de la capa de entrada-oculta
	entradas->transpose();
	Matrix<T>* deltas_pesos_ih = Matrix<T>::dot(gradiente_entrada_oculta, entradas);
	this->pesos_ih->add(deltas_pesos_ih);

	//Free space.
	delete vec_salidas;
	delete entradas;
	delete respuestas;
	delete salidas_capas_ocultas_T;
	delete errores_salida;
	delete gradiente_entrada_oculta;
	delete gradiente_salida;
	delete deltas_pesos_ih;
	delete deltas_pesos_ho;
}

template <typename T>
inline void DeepNeuralNetwork<T>::printWeights(){
	std::cout << "---- [Input - Hidden[0] Layer Weights] ----" << "\n\n" << this->pesos_ih << std::endl;
	std::cout << "---- [Hidden (n - n+1) Layer Weights] ----" << "\n\n";
	int layer = 0;
	for(auto nth_hidden_weights : this->pesos_hn){
		std::cout <<  "#### Layer " << layer << " ####" << "\n\n" << nth_hidden_weights;
		layer++;
	}
	std::cout << "---- [Hidden[n-1] - Output Layer Weights] ----" << "\n\n" << this->pesos_ho << std::endl;
}

template class NeuralNetwork<float>;
template class DeepNeuralNetwork<float>;