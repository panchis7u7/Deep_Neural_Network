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
	delete(this->bias_o)
}

template <typename T>
std::vector<T>* NeuralNetwork<T>::feedForward(std::vector<T>* vec_entrada) {

	//Convert vector to a matrix type. 		TODO: Implement native vector support.
	Matrix<T>* entradas = Matrix<T>::fromVector(vec_entrada);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// sig((W * i) + b) process for input - hidden.
	// W -> Weights.
	// i -> Inputs.
	// b -> Bias.
	// sig -> Sigmoid function.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Matrix<T>* entradas_capa_oculta = Matrix<T>::dot(this->pesos_ih, entradas);
	entradas_capa_oculta->add(this->bias_h);
	this->hidden_weights_output = Matrix<T>::map(entradas_capa_oculta, sigmoid);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// sig((W * i) + b) process for hidden - output.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Matrix<T>* entradas_capa_salida = Matrix<T>::dot(this->pesos_ho, this->hidden_weights_output);
	entradas_capa_salida->add(this->bias_o);
	Matrix<T>* salidas = Matrix<T>::map(entradas_capa_salida, sigmoid);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	return Matrix<T>::toVector(salidas);
}

template <typename T>
void NeuralNetwork<T>::train(std::vector<T>* vec_entradas, std::vector<T>* vec_respuestas) {

	// Convert vector to a matrix type. 		TODO: Implement native vector support.
	std::vector<T>* vec_salidas = this->feedForward(vec_entradas);
	Matrix<T>* entradas = Matrix<T>::fromVector(vec_entradas);
	Matrix<T>* respuestas = Matrix<T>::fromVector(vec_respuestas);
	Matrix<T>* salidas = Matrix<T>::fromVector(vec_salidas);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Error calculation.
	// 1) Output Errors -> (respuestas -  salidas).
	Matrix<T>* errores_salida = Matrix<T>::elementWiseSubstraction(respuestas, salidas);

	// 2) Hidden Layer Errors -> (Wh^T * (Output Erros)).
	Matrix<T>* errores_capa_oculta_salida = Matrix<T>::dot(Matrix<T>::transpose(pesos_ho), errores_salida);
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////

	//Calcular el gradiente de la capa de salida = learning_rate * errores_salida * dsigmoid(salidas)
	Matrix<T>* gradiente_salida = Matrix<T>::map(salidas, dsigmoid);
	gradiente_salida->hadamardProduct(errores_salida);
	gradiente_salida->scalarProduct(learning_rate);

	//Calcular los gradientes de la capa oculta = learning_rate * errores_capa_oculta * dsigmoid(hidden_weights_output)
	Matrix<T>* gradientes_capa_oculta = Matrix<T>::map(this->hidden_weights_output, dsigmoid);
	gradientes_capa_oculta->hadamardProduct(errores_capa_oculta_salida);
	gradientes_capa_oculta->scalarProduct(learning_rate);

	//Calcular deltas de la capa oculta-salida
	//pesos_delta = learning_rate * errores * dsigmoid(salidas) * pesos(T)
	Matrix<T>* deltas_pesos_ho = Matrix<T>::dot(gradiente_salida, Matrix<T>::transpose(this->hidden_weights_output));
	this->pesos_ho->add(deltas_pesos_ho);
	this->bias_o->add(gradiente_salida);

	//Calcular deltas de la capa de entrada-oculta
	Matrix<T>* deltas_pesos_ih = Matrix<T>::dot(gradientes_capa_oculta, Matrix<T>::transpose(entradas));
	this->pesos_ih->add(deltas_pesos_ih);
	this->bias_h->add(gradientes_capa_oculta);
}

template <typename T>
void NeuralNetwork<T>::printWeights() {
	std::cout << "---- [Input - Hidden Layer Weights] ----" << "\n\n" << this->pesos_ih << std::endl;
	std::cout << "---- [Hidden - Output Layer Weights] ----" << "\n\n" << this->pesos_ho << std::endl;
}

template <typename T>
T NeuralNetwork<T>::sigmoid(T n) {
	return (1 / (1 + pow(2.718281828, (-n))));
}

template <typename T>
T NeuralNetwork<T>::dsigmoid(T y) {
	//return sigmoid(n) * (1 - sigmoid(n));
	return (y * (1 - y));
}

template<typename T>
DeepNeuralNetwork<T>::DeepNeuralNetwork(uint_fast64_t input, std::vector<uint_fast64_t>& hidden, uint_fast64_t output){
	this->inputLayerNodes = input;
	this->hiddenLayerSize = hidden.size();
	this->outputLayerNodes = output;
	//Input-Hidden[0] layer weights with random values.
	this->pesos_ih = new Matrix<T>(hidden[0], this->inputLayerNodes);
	this->pesos_ih->randomize();
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
	this->bias.push_back(new Matrix<T>(outputLayerNodes, 1));
	this->bias.at(bias.size()- 1)->randomize();

	//Variables por eliminar
	this->bias_h = nullptr;
	this->bias_o = nullptr;
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
	Matrix<T>* entradas = Matrix<T>::fromVector(vec_entrada);
	//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas
	Matrix<T>* entradas_capa_oculta = Matrix<T>::dot(this->pesos_ih, entradas);
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_oculta->add(this->bias.at(0));
	//sig((W * i) + b) se aplica la funcion sigmoide
	////this->salidas_capas_ocultas.push_back(Matrix<float>::map(entradas_capa_oculta, sigmoid));

	this->salidas_capas_ocultas.at(0) = Matrix<T>::map(entradas_capa_oculta, sigmoid);
	//Se multiplica la matriz de pesos entre la capas ocultas y la matriz de entradas previas
	for (int i = 0; i < (hiddenLayerSize-1); i++)
	{
		//Se multiplica la matriz de pesos entre la capa oculta y la matriz de entradas de la enesima capa oculta
		////this->salidas_capas_ocultas.push_back(Matrix<float>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i)));
		this->salidas_capas_ocultas.at(i+1) = Matrix<T>::dot(this->pesos_hn.at(i), this->salidas_capas_ocultas.at(i));
		//Al resultado de la multiplicacion se le agrega el sesgo
		this->salidas_capas_ocultas.at(i+1)->add(this->bias.at(i+1));
		//sig((W * i) + b) se aplica la funcion sigmoide
		this->salidas_capas_ocultas.at(i+1)->map(sigmoid);
	}
	//----Generando las salida----
	//Se multiplica la matriz de pesos entre la capa de salida y la matriz de salidas de la capa oculta
	Matrix<T>* entradas_capa_salida = Matrix<T>::dot(this->pesos_ho, this->salidas_capas_ocultas.at(salidas_capas_ocultas.size()-1));
	//Al resultado de la multiplicacion se le agrega el sesgo
	entradas_capa_salida->add(bias.at(hiddenLayerSize));
	//sig((W * i) * b) se aplica la funcion sigmoide
	Matrix<T>* salidas = Matrix<float>::map(entradas_capa_salida, sigmoid);
	//delete entradas_capa_oculta;
	return Matrix<T>::toVector(salidas);
}

template <typename T>
void DeepNeuralNetwork<T>::train(std::vector<T>* guess, std::vector<T>* answers){
	std::vector<T>* vec_salidas = this->feedForwardDNN(vec_entradas);
	//Convertir vectores a matrices
	Matrix<T>* entradas = Matrix<T>::fromVector(vec_entradas);
	Matrix<T>* respuestas = Matrix<T>::fromVector(vec_respuestas);
	Matrix<T>* salidas = Matrix<T>::fromVector(vec_salidT
	//Calcular el error => respuestas - salidas
	Matrix<T>* errores_salida = Matrix<T>::elementWiseSubstraction(respuestas, salidas);

	//Calcular los errores de la capa oculta->salida 
	
	this->errores.at(hiddenLayerSize-1) = Matrix<T>::dot(Matrix<T>::transpose(this->pesos_ho), errores_salida);
	for (int i = hiddenLayerSize-1; i > 0; i--)
	{
		this->errores.at(i-1) = Matrix<T>::dot(Matrix<T>::transpose(this->pesos_hn.at(i - 1)), errores.at(i));
	}

	//Calcular el gradiente de la capa de salida = learning_rate * errores_salida * dsigmoid(salidas)
	Matrix<T>* gradiente_salida = Matrix<T>::map(salidas, dsigmoid);
	gradiente_salida->hadamardProduct(errores_salida);
	gradiente_salida->scalarProduct(learning_rate);
	this->bias.at(bias.size() - 1)->add(gradiente_salida);
	//Calcular el gradiente de las capas ocultas y oculta-salida = learning_rate * errores_salida * dsigmoid(salidas)
	for (int i = hiddenLayerSize-1; i > 0; i--)
	{
		this->gradientes.at(i-1) = Matrix<T>::map(this->salidas_capas_ocultas.at(hiddenLayerSize-i), dsigmoid);
		this->gradientes.at(i-1)->hadamardProduct(errores.at(hiddenLayerSize-i));
		this->gradientes.at(i-1)->scalarProduct(learning_rate);
		this->bias.at(hiddenLayerSize-i)->add(gradientes.at(i-1));
	}
	//Calcular los gradientes de la capa entada-oculta = learning_rate * errores_capa_oculta * dsigmoid(hidden_weights_output)
	Matrix<T>* gradiente_entrada_oculta = Matrix<T>::map(this->salidas_capas_ocultas.at(0), dsigmoid);
	gradiente_entrada_oculta->hadamardProduct(errores.at(0));
	gradiente_entrada_oculta->scalarProduct(learning_rate);
	this->bias.at(0)->add(gradiente_entrada_oculta);

	//Calcular deltas de la capa oculta-salida
	Matrix<T>* deltas_pesos_ho = Matrix<T>::dot(gradiente_salida, Matrix<T>::transpose(this->salidas_capas_ocultas.at(salidas_capas_ocultas.size()-1)));
	this->pesos_ho->add(deltas_pesos_ho);
	for (int i = hiddenLayerSize - 1; i > 0; i--)
	{
		//this->deltas.at(i-1) = Matrix<float>::dot(gradientes.at(i-1), Matrix<float>::transpose(this->salidas_capas_ocultas.at(i)));
		this->deltas.at(i-1) = Matrix<T>::dot(this->salidas_capas_ocultas.at(i), Matrix<T>::transpose(gradientes.at(i - 1)));
		this->pesos_hn.at(i-1)->add(deltas.at(i-1));
	}
	//Calcular deltas de la capa de entrada-oculta
	Matrix<T>* deltas_pesos_ih = Matrix<T>::dot(gradiente_entrada_oculta, Matrix<T>::transpose(entradas));
	this->pesos_ih->add(deltas_pesos_ih);
}

template <typename T>
void DeepNeuralNetwork<T>::printWeights(){
	std::cout << "---- [Input - Hidden[0] Layer Weights] ----" << "\n\n" << this->pesos_ih << std::endl;
	std::cout << "---- [Hidden (n - n+1) Layer Weights] ----" << "\n\n";
	int layer = 0;
	for(auto nth_hidden_weights : this->pesos_hn){
		std::cout <<  "#### Layer " << layer << " ####" << "\n\n" << nth_hidden_weights;
		layer++;
	}
	std::cout << "---- [Hidden[n-1] - Output Layer Weights] ----" << "\n\n" << this->pesos_ho << std::endl;
}