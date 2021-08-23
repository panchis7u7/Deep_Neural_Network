#include <include/Matrix.hpp>

using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

///////////////////////////////////////////////////////////////////////////////////////////
// Public typename Methods.
///////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Matrix<T>::Matrix() {
	this->rows = 0;
	this->columns = 0;
	this->data = alloc(0,0);
}

template <typename T>
Matrix<T>::Matrix(uint_fast64_t rows, uint_fast64_t columns) {
	this->rows = rows;
	this->columns = columns;
	this->data = alloc(rows, columns);
}

template <typename T>
Matrix<T>::Matrix(Matrix<T>& copy){
	this->rows = copy.rows;
	this->columns = copy.columns;
	this->data = alloc(copy.rows, copy.columns);
	//std::copy(std::begin(*(copy.data)), std::end(*(copy.data)), std::begin(*(this.data)));
}

template <typename T>
Matrix<T>::~Matrix() {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		delete[] data[i];
	}
	delete[] data;
}

template <typename T>
void Matrix<T>::print() {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		std::cout << "|";
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			std::cout << "  " << this->data[i][j] << "  ";
		}
		std::cout << "|";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

template <typename T>
void Matrix<T>::add(T addend) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] += addend;
		}
	}
}

template <typename T>
void Matrix<T>::add(Matrix<T>* addend) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] += addend->data[i][j];
		}
	}
}

template <typename T>
void Matrix<T>::randomize() {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			//Genera numero aleatorio entre -1 y 1
			this->data[i][j] = (-1) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - (-1)))); 
		}
	}
}

template <typename T>
void Matrix<T>::scalarProduct(T factor) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] *= factor;
		}
	}
}

template <typename T>
void Matrix<T>::hadamardProduct(Matrix<T>* factor) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] *= factor->data[i][j];
		}
	}
}

template <typename T>
void Matrix<T>::map(T (*func)(T)) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] = func(this->data[i][j]);
		}
	}
}

template <typename T>
T** Matrix<T>::alloc(uint_fast64_t rows, uint_fast64_t columns) {
	T** data = new T* [rows];
	for (uint_fast64_t i = 0; i < rows; i++)
	{
		data[i] = new T[columns];
	}
	return data;
}

template class Matrix<float>;
template class Matrix<double>;