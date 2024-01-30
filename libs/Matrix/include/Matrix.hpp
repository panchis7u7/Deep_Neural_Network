#pragma once

#include <time.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <functional>

// Uploaded by panchis7u7 ~ Sebastian Madrigal
namespace voxel {

	template <class T>
	class Matrix {
	public:

		// #########################################################################################
		// Public constructors and destructors methods.
		// #########################################################################################

		Matrix();
		Matrix(uint_fast64_t rows, uint_fast64_t columns);
		Matrix(Matrix<T>& copy);
		Matrix(std::vector<T>& vec);
		~Matrix();

		// #########################################################################################
		// Public static typename methods.
		// #########################################################################################

		static Matrix<T>* from_vector(std::vector<T>* from);
		static std::vector<T>* to_vector(Matrix<T>* from);
		static Matrix<T>* hadamard_product(Matrix<T>* multiplicand, Matrix<T>* multiplier);
		static Matrix<T>* substract(Matrix<T>* minuend, Matrix<T>* subtrahend);
		static void subtract(Matrix<T>* to, Matrix<T>* lhs, Matrix<T>* rhs);
		static Matrix<T>* multiply(Matrix<T>* multiplicand, Matrix<T>* multiplier);
		static Matrix<T>* multiply(Matrix<T>* multiplicand, std::vector<T>* multiplier);
		static void multiply(Matrix<T>* to, Matrix<T>* multiplicand, Matrix<T>* multiplier);
		static void multiply(Matrix<T>* to, Matrix<T>* multiplicand, std::vector<T>* multiplier);
		static Matrix<T>* transpose(Matrix<T>* from);
		static void transpose(Matrix<T>* to, Matrix<T>* from);
		static Matrix<T>* map(Matrix<T> *A, T (*func)(T));
		static void map(Matrix<T>* to, Matrix<T>* from, T (*func)(T));
		static Matrix<T>* duplicate_dimensions(Matrix<T>* from, std::string description);
		static Matrix<T>* duplicate_randomize(Matrix<T>* from, std::string description);
		static Matrix<T>* duplicate_transpose(Matrix<T>* from, std::string description);

		// #########################################################################################
		// Public instance-based methods.
		// #########################################################################################

		void print();
		void set_description(std::string);
		void add(T addend);
		void add(Matrix<T>* addend);
		void subtract(Matrix<T>* subtrahend);
		void multiply(Matrix<T>& multiplier);
		void randomize();
		void transpose();
		void scalar_product(T multiplier);
		void hadamard_product(Matrix<T>* multiplier);
		void for_each(std::function<void(T data, unsigned row, unsigned column)> callback);
		void map(T (*func)(T));
		unsigned get_rows();
		unsigned get_columns();
		T** get_data();

		// #########################################################################################
		// Operator Overloading.
		// #########################################################################################

		// ------------------------------------------------------------------------------------
		// Overload the output stream operator so it can print the matrix contents in a pretty
		// format.
		// ------------------------------------------------------------------------------------

		friend std::ostream &operator<<(std::ostream &out, const Matrix<T> *mat) {
			for (uint_fast64_t i = 0; i < mat->rows; i++) {
				out << "|";
				for (uint_fast64_t j = 0; j < mat->columns; j++){
					out << "  " << mat->data[i][j] << "  ";
				}
				out << "|";
				out << std::endl;
			}
			out << std::endl;
			return out;
		}

	private:
		T **data;
		unsigned rows;
		unsigned columns;
		T **alloc(uint_fast64_t rows, uint_fast64_t columns);
		std::string description;

	protected:
	};
}
