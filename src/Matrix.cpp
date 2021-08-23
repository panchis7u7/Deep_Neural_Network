#include "Matrix.hpp"

using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

///////////////////////////////////////////////////////////////////////////////////////////
// Public class Methods.
///////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Matrix<T>::Matrix() {
	this->rows = 0;
	this->columns = 0;
	this->data = alloc(0,0);
}

template <class T>
Matrix<T>::Matrix(uint_fast64_t rows, uint_fast64_t columns) {
	this->rows = rows;
	this->columns = columns;
	this->data = alloc(rows, columns);
}

template <class T>
Matrix<T>::Matrix(Matrix<T>& copy){
	this->rows = copy->rows;
	this->columns = copy->columns;
	this->data = alloc(copy->rows, copy->columns);
	std::copy(std::begin(copy->data), std::end(copy->data), std::begin(this->data));
}

template <class T>
Matrix<T>::~Matrix() {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		delete[] data[i];
	}
	delete[] data;
}

template <class T>
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

template <class T>
void Matrix<T>::add(T addend) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] += addend;
		}
	}
}

template <class T>
void Matrix<T>::add(Matrix<T>* addend) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] += addend->data[i][j];
		}
	}
}

template <class T>
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

template <class T>
void Matrix<T>::scalarProduct(T factor) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] *= factor;
		}
	}
}

template <class T>
void Matrix<T>::hadamardProduct(Matrix<T>* factor) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] *= factor->data[i][j];
		}
	}
}

template <class T>
void Matrix<T>::map(T (*func)(T)) {
	for (uint_fast64_t i = 0; i < this->rows; i++)
	{
		for (uint_fast64_t j = 0; j < this->columns; j++)
		{
			this->data[i][j] = func(this->data[i][j]);
		}
	}
}

template <class T>
T** alloc(uint_fast64_t rows, uint_fast64_t columns) {
	T** data = new T* [rows];
	for (uint_fast64_t i = 0; i < rows; i++)
	{
		data[i] = new T[columns];
	}
	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Public static class Methods.
///////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Matrix<T>* fromVector(std::vector<T>* entradas) {
	Matrix<T>* result = new Matrix(entradas->size(), 1);
	for (uint_fast64_t i = 0; i < entradas->size(); i++)
	{
		result->data[i][0] = entradas->at(i);
	}
	return result;
}

template <class T>
std::vector<T>* toVector(Matrix<T>* entradas) {
	std::vector<T>* result = new std::vector<T>();
	for (uint_fast64_t i = 0; i < entradas->rows; i++)
	{
		for (uint_fast64_t j = 0; j < entradas->columns; j++)
		{
			result->push_back(entradas->data[i][j]);
		}
	}
	return result;
}

template <class T>
Matrix<T>* hadamardProduct(Matrix<T>* A, Matrix<T>* B) {
	if ((A->rows != B->rows) || (A->columns != B->columns)){
		return NULL;
	} else {
		Matrix<T>* result = new Matrix<T>(A->rows, B->columns);
		for (uint_fast64_t i = 0; i < A->rows; i++)
		{
			for (uint_fast64_t j = 0; j < B->columns; j++)
			{
				result->data[i][j] = A->data[i][j] * B->data[i][j];
			}
		}
		return result;
	}
}

template <class T>
Matrix<T>* elementWiseSubstraction(Matrix<T>* A, Matrix<T>* B) {
	if ((A->rows != B->rows) || (A->columns != B->columns)) {
		return NULL;
	}
	else {
		Matrix<T>* result = new Matrix<T>(A->rows, B->columns);
		for (uint_fast64_t i = 0; i < A->rows; i++)
		{
			for (uint_fast64_t j = 0; j < B->columns; j++)
			{
				result->data[i][j] = A->data[i][j] - B->data[i][j];
			}
		}
		return result;
	}
}

template <class T>
Matrix<T>* dot(Matrix<T>* A, Matrix<T>* B) {
	Matrix<T>* result = new Matrix<T>(A->rows, B->columns);
	for (uint_fast64_t i = 0; i < result->rows; i++)
	{
		for (uint_fast64_t j = 0; j < result->columns; j++)
		{
			T sum = 0;
			for (uint_fast64_t k = 0; k < A->columns; k++)
			{
				sum += A->data[i][k] * B->data[k][j];
			}
			result->data[i][j] = sum;
		}
	}
	return result;
}

template <class T>
Matrix<T>* transpose(Matrix<T>* A) {
	Matrix<T>* result = new Matrix<T>(A->columns, A->rows);
	for (uint_fast64_t i = 0; i < A->rows; i++)
	{
		for (uint_fast64_t j = 0; j < A->columns; j++)
		{
			result->data[j][i] = A->data[i][j];
		}
	}
	return result;
}

template <class T>
Matrix<T>* map(Matrix<T>* A, T (*func)(T)) {
	Matrix<T>* result = new Matrix<T>(A->rows, A->columns);
	for (uint_fast64_t i = 0; i < A->rows; i++)
	{
		for (uint_fast64_t j = 0; j < A->columns; j++)
		{
			result->data[i][j] = func(A->data[i][j]);
		}
	}
	return result;
}