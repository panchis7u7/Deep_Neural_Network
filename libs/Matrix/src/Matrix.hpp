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
		void scalarProduct(T factor);
		void hadamardProduct(Matrix<T>* factor);
		void map(T (*func)(T));

		//Static methods.
		static Matrix* fromVector(std::vector<T>* entradas);
		static std::vector<T>* toVector(Matrix<T>* entradas);
		static Matrix<T>* hadamardProduct(Matrix<T>* A, Matrix<T>* B);
		static Matrix<T>* elementWiseSubstraction(Matrix<T>* A, Matrix<T>* B);
		static Matrix<T>* dot(Matrix<T>* A, Matrix<T>* B);
		static Matrix<T>* transpose(Matrix<T>* A);
		static Matrix<T>* map(Matrix<T>* A, T (*func)(T));

	private:
		T** data;
		unsigned rows;
		unsigned columns;
		T** alloc(uint_fast64_t rows, uint_fast64_t columns);

	protected:
	};
}
