#pragma once
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include <cstdlib>

// Uploaded by panchis7u7 ~ Sebastian Madrigal

class Matrix {
public:
	Matrix();
	Matrix(int filas, int columnas);
	~Matrix();
	float** alloc(int filas, int columnas);
	void print();
	void suma(int n);
	void sumar(Matrix*);
	void aleatorizar();
	float normalRandom();
	void map(float (*func)(float));
	Matrix* transpuesta();

	static Matrix* fromVector(std::vector<float>* entradas) {
		Matrix* resultado = new Matrix(entradas->size(), 1);
		for (size_t i = 0; i < entradas->size(); i++)
		{
			resultado->datos[i][0] = entradas->at(i);
		}
		return resultado;
	}

	static std::vector<float>* toVector(Matrix* entradas) {
		std::vector<float>* resultado = new std::vector<float>();
		for (size_t i = 0; i < entradas->filas; i++)
		{
			for (size_t j = 0; j < entradas->columnas; j++)
			{
				resultado->push_back(entradas->datos[i][j]);
			}
		}
		return resultado;
	}

	static Matrix* productoHadamard(Matrix* A, Matrix* B) {
		if ((A->filas != B->filas) || (A->columnas != B->columnas)){
			return NULL;
		} else {
			Matrix* resultado = new Matrix(A->filas, B->columnas);
			for (size_t i = 0; i < A->filas; i++)
			{
				for (size_t j = 0; j < B->columnas; j++)
				{
					resultado->datos[i][j] = A->datos[i][j] * B->datos[i][j];
				}
			}
			return resultado;
		}
	}

	static Matrix* restaElementWise(Matrix* A, Matrix* B) {
		if ((A->filas != B->filas) || (A->columnas != B->columnas)) {
			return NULL;
		}
		else {
			Matrix* resultado = new Matrix(A->filas, B->columnas);
			for (size_t i = 0; i < A->filas; i++)
			{
				for (size_t j = 0; j < B->columnas; j++)
				{
					resultado->datos[i][j] = A->datos[i][j] - B->datos[i][j];
				}
			}
			return resultado;
		}
	}

	static Matrix* multiplicar(Matrix* A, Matrix* B) {
		Matrix* resultado = new Matrix(A->filas, B->columnas);
		for (short i = 0; i < resultado->filas; i++)
		{
			for (short j = 0; j < resultado->columnas; j++)
			{
				float suma = 0;
				for (short k = 0; k < A->columnas; k++)
				{
					suma += A->datos[i][k] * B->datos[k][j];
				}
				resultado->datos[i][j] = suma;
			}
		}
		return resultado;
	}

private:
	float** datos;
	unsigned filas;
	unsigned columnas;
protected:
};