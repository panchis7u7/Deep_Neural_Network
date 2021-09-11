#pragma once

#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include <cstdlib>

// Uploaded by panchis7u7 ~ Sebastian Madrigal
namespace voxel {
	
	template <class T>
	class Matrix {
	public:
		Matrix();
		Matrix(uint_fast64_t rows, uint_fast64_t columns);
		Matrix(Matrix<T>& copy);
		~Matrix();
		void print();
		void add(T addend);
		void add(Matrix<T>* addend);
		void randomize();
		void transpose();
		void scalarProduct(T factor);
		void hadamardProduct(Matrix<T>* factor);
		void map(T (*func)(T));

		//Overloads.
		//friend std::ostream& operator<< <>(std::ostream& out, const Matrix<T>* mat);

		//Static methods.
		//static Matrix* fromVector(std::vector<T>* entradas);
		//static std::vector<T>* toVector(Matrix<T>* entradas);
		//static Matrix<T>* hadamardProduct(Matrix<T>* A, Matrix<T>* B);
		//static Matrix<T>* elementWiseSubstraction(Matrix<T>* A, Matrix<T>* B);
		//static Matrix<T>* dot(Matrix<T>* A, Matrix<T>* B);
		//static Matrix<T>* transpose(Matrix<T>* A);
		//static Matrix<T>* map(Matrix<T>* A, T (*func)(T));

		///////////////////////////////////////////////////////////////////////////////////////////
		// Public static typename Methods.
		///////////////////////////////////////////////////////////////////////////////////////////

	
		static Matrix<T>* fromVector(std::vector<T>* entradas) {
			Matrix<T>* result = new Matrix(entradas->size(), 1);
			for (uint_fast64_t i = 0; i < entradas->size(); i++)
			{
				result->data[i][0] = entradas->at(i);
			}
			return result;
		}

		static std::vector<T>* toVector(Matrix<T>* entradas) {
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

		static Matrix<T>* hadamardProduct(Matrix<T>* A, Matrix<T>* B) {
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

		static Matrix<T>* elementWiseSubstraction(Matrix<T>* A, Matrix<T>* B) {
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

		static Matrix<T>* dot(Matrix<T>* A, Matrix<T>* B) {
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

		static Matrix<T>* transpose(Matrix<T>* A) {
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

		static Matrix<T>* map(Matrix<T>* A, T (*func)(T)) {
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

		///////////////////////////////////////////////////////////////////////////////////////////
		// Operator Overloading.
		///////////////////////////////////////////////////////////////////////////////////////////

		friend std::ostream& operator<<(std::ostream& out, const Matrix<T>* mat){
			for (uint_fast64_t i = 0; i < mat->rows; i++)
			{
				out << "|";
				for (uint_fast64_t j = 0; j < mat->columns; j++)
				{
					out << "  " << mat->data[i][j] << "  ";
				}
				out << "|";
				out << std::endl;
			}
			out << std::endl;
			return out;
		}

	private:
		T** data;
		unsigned rows;
		unsigned columns;
		T** alloc(uint_fast64_t rows, uint_fast64_t columns);
	protected:
	};
}
