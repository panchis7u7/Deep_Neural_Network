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
	Matrix(int, int);
	~Matrix();
	float** alloc(int, int);
	void print();
	void suma(float);
	void suma(Matrix*);
	void aleatorizar();
	void productoScalar(float);
	void productoHadamard(Matrix*);
	void map(float (*func)(float));

	static Matrix* fromVector(std::vector<float>* entradas) {
		Matrix* resultado = new Matrix(entradas->size(), 1);
		for (unsigned i = 0; i < entradas->size(); i++)
		{
			resultado->datos[i][0] = entradas->at(i);
		}
		return resultado;
	}

	static std::vector<float>* toVector(Matrix* entradas) {
		std::vector<float>* resultado = new std::vector<float>();
		for (unsigned i = 0; i < entradas->filas; i++)
		{
			for (unsigned j = 0; j < entradas->columnas; j++)
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
			for (unsigned i = 0; i < A->filas; i++)
			{
				for (unsigned j = 0; j < B->columnas; j++)
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
			for (unsigned i = 0; i < A->filas; i++)
			{
				for (unsigned j = 0; j < B->columnas; j++)
				{
					resultado->datos[i][j] = A->datos[i][j] - B->datos[i][j];
				}
			}
			return resultado;
		}
	}

	static Matrix* multiplicar(Matrix* A, Matrix* B) {
		Matrix* resultado = new Matrix(A->filas, B->columnas);
		for (unsigned i = 0; i < resultado->filas; i++)
		{
			for (unsigned j = 0; j < resultado->columnas; j++)
			{
				float suma = 0;
				for (unsigned k = 0; k < A->columnas; k++)
				{
					suma += A->datos[i][k] * B->datos[k][j];
				}
				resultado->datos[i][j] = suma;
			}
		}
		return resultado;
	}

	static Matrix* transpuesta(Matrix* A) {
		Matrix* resultado = new Matrix(A->columnas, A->filas);
		for (unsigned i = 0; i < A->filas; i++)
		{
			for (unsigned j = 0; j < A->columnas; j++)
			{
				resultado->datos[j][i] = A->datos[i][j];
			}
		}
		return resultado;
	}

	static Matrix* map(Matrix* A, float (*func)(float)) {
		Matrix* resultado = new Matrix(A->filas, A->columnas);
		for (unsigned i = 0; i < A->filas; i++)
		{
			for (unsigned j = 0; j < A->columnas; j++)
			{
				resultado->datos[i][j] = func(A->datos[i][j]);
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