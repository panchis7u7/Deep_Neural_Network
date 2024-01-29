#pragma once

#include <time.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <spdlog/spdlog.h>

// Uploaded by panchis7u7 ~ Sebastian Madrigal
namespace voxel {

	template <class T>
	class Matrix {
	public:
		Matrix();
		Matrix(uint_fast64_t rows, uint_fast64_t columns);
		Matrix(Matrix<T>& copy);
		Matrix(std::vector<T>& vec);
		~Matrix();
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
		// Public static typename Methods.
		// #########################################################################################

		// ------------------------------------------------------------------------------------
		// Create a new Matrix<T> object from a Vector<T> class.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* from_vector(std::vector<T>* from) {
			Matrix<T> *result = new Matrix(from->size(), 1);
			for (uint_fast64_t i = 0; i < from->size(); i++)
				result->data[i][0] = from->at(i);
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Create a new Vector<T> object from a Matrix<T> class; it flattens out a Matrix.
		// ------------------------------------------------------------------------------------

		static std::vector<T>* to_vector(Matrix<T>* from) {
			std::vector<T>* result = new std::vector<T>();
			for (uint_fast64_t i = 0; i < from->rows; i++) {
				for (uint_fast64_t j = 0; j < from->columns; j++) {
					result->push_back(from->data[i][j]);
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Multiply by Matrix<T> corresponding cells, it must that have the same dimensions.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* hadamard_product(Matrix<T>* multiplicand, Matrix<T>* multiplier) {
			if ((multiplicand->rows != multiplier->rows) || (multiplicand->columns != multiplier->columns)) {
				spdlog::error("Trying to perform a hadmard product operation on non-matching matrices: \n 1: {} \n 2: {}", multiplicand->description, multiplier->description);
				return NULL;
			}
				
			Matrix<T> *result = new Matrix<T>(multiplicand->rows, multiplier->columns);
			for (uint_fast64_t i = 0; i < multiplicand->rows; i++) {
				for (uint_fast64_t j = 0; j < multiplier->columns; j++) {
					result->data[i][j] = multiplicand->data[i][j] * multiplier->data[i][j];
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Subtract Matrix<T> corresponding cells, it must that have the same dimensions.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* substract(Matrix<T>* minuend, Matrix<T>* subtrahend) {
			if ((minuend->rows != subtrahend->rows) || (minuend->columns != subtrahend->columns)) {
				spdlog::error("Trying to perform a substraction operation on non-matching matrices: \n 1: {} \n 2: {}", minuend->description, subtrahend->description);
				return NULL;
			}

			Matrix<T> *result = new Matrix<T>(minuend->rows, subtrahend->columns);
			for (uint_fast64_t i = 0; i < minuend->rows; i++) {
				for (uint_fast64_t j = 0; j < subtrahend->columns; j++) {
					result->data[i][j] = minuend->data[i][j] - subtrahend->data[i][j];
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Subtract to Matrix<T> corresponding cells and save it to a previously allocated 
		// memory address; it must that have the same dimensions.
		// ------------------------------------------------------------------------------------

		static void subtract(Matrix<T>* to, Matrix<T>* lhs, Matrix<T>* rhs) {
			if ((lhs->rows != rhs->rows) || (lhs->columns != rhs->columns))
				spdlog::error("Trying to perform a substraction operation on non-matching matrices: \n 1: {} \n 2: {}", lhs->description, rhs->description);
				return;

			for (uint_fast64_t i = 0; i < lhs->rows; i++) {
				for (uint_fast64_t j = 0; j < rhs->columns; j++) {
					to->data[i][j] = lhs->data[i][j] - rhs->data[i][j];
				}
			}
		}

		// ------------------------------------------------------------------------------------
		// Multiply 2 Matrix<T> objects and return a new one with new dimensions. The result of 
		// matrix multiplication is a matrix.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* multiply(Matrix<T>* multiplicand, Matrix<T>* multiplier) {
			Matrix<T> *result = new Matrix<T>(multiplicand->rows, multiplier->columns);
			for (uint_fast64_t i = 0; i < multiplicand->rows; i++) {
				for (uint_fast64_t j = 0; j < multiplier->columns; j++) {
					for (uint_fast64_t k = 0; k < multiplicand->columns; k++) {
						result->data[i][j] += multiplicand->data[i][k] * multiplier->data[k][j];
					}
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Multiply a Matrix<T> objects with a Vector<T> and return a new Matrix<T> with new 
		// dimensions. The result of matrix multiplication is another matrix.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* multiply(Matrix<T>* multiplicand, std::vector<T>* multiplier) {
			// n Column Matrix requires n elements vector in order to perform product.
			Matrix<T> *result = new Matrix<T>(multiplicand->rows, 1);
			for (uint_fast64_t i = 0; i < multiplicand->rows; i++) {
				for (uint_fast64_t j = 0; j < result->columns; j++) {
					for (uint_fast64_t k = 0; k < multiplicand->columns; k++) {
						result->data[i][j] += multiplicand->data[i][k] * multiplier->at(k);
					}
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Multiply 2 Matrix<T> objects and save it to a previously allocated 
		// memory address; The result of matrix multiplication is a matrix.
		// ------------------------------------------------------------------------------------

		static void multiply(Matrix<T>* to, Matrix* multiplicand, Matrix<T>* multiplier) {
			for (uint_fast64_t i = 0; i < multiplicand->rows; ++i) {
				for (uint_fast64_t j = 0; j < multiplier->columns; ++j) {
					for (uint_fast64_t k = 0; k < multiplicand->columns; ++k) {
						to->data[i][j] += multiplicand->data[i][k] * multiplier->data[k][j];
					}
				}
			}
		}

		// ------------------------------------------------------------------------------------
		// Transpose a Matrix<T> object and allocate new memory to this new dimension Matrix.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* transpose(Matrix<T>* from) {
			Matrix<T>* result = new Matrix<T>(from->columns, from->rows);
			for (uint_fast64_t i = 0; i < from->rows; i++) {
				for (uint_fast64_t j = 0; j < from->columns; j++) {
					result->data[j][i] = from->data[i][j];
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Transpose a Matrix<T> object and save it to a previously allocated memory address.
		// ------------------------------------------------------------------------------------

		static void transpose(Matrix<T>* to, Matrix<T>* from) {
			for (uint_fast64_t i = 0; i < from->rows; i++) {
				for (uint_fast64_t j = 0; j < from->columns; j++) {
					to->data[j][i] = from->data[i][j];
				}
			}
		}

		// ------------------------------------------------------------------------------------
		// Apply a function to all the cell data within a matrix and save the results to a new
		// memory allocated matrix.
		// ------------------------------------------------------------------------------------

		static Matrix<T> *map(Matrix<T> *A, T (*func)(T)) {
			Matrix<T> *result = new Matrix<T>(A->rows, A->columns);
			for (uint_fast64_t i = 0; i < A->rows; i++) {
				for (uint_fast64_t j = 0; j < A->columns; j++) {
					result->data[i][j] = func(A->data[i][j]);
				}
			}
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Apply a function to all the cell data within a matrix and and save it to a 
		// previously allocated memory address.
		// ------------------------------------------------------------------------------------

		static void map(Matrix<T>* to, Matrix<T>* from, T (*func)(T)) {
			for (uint_fast64_t i = 0; i < from->rows; i++) {
				for (uint_fast64_t j = 0; j < from->columns; j++) {
					to->data[i][j] = func(from->data[i][j]);
				}
			}
		}

		// ------------------------------------------------------------------------------------
		// Duplicate an allocated Matrix<T> characteristics without copying its values.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* duplicate_dimensions(Matrix<T>* from, std::string description) {
			Matrix<T>* result = new Matrix<T>(from->rows, from->columns);
			result->set_description(description);
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Duplicate an allocated Matrix<T> characteristics randomizing its contents.
		// ------------------------------------------------------------------------------------

		static Matrix<T>* duplicate_randomize(Matrix<T>* from, std::string description) {
			Matrix<T>* result = new Matrix<T>(from->rows, from->columns);
			result->set_description(description);
			result->randomize();
			return result;
		}

		// ------------------------------------------------------------------------------------
		// Duplicate an allocated Matrix<T> dimensions and transpose it (conserve its values).
		// ------------------------------------------------------------------------------------

		static Matrix<T>* duplicate_transpose(Matrix<T>* from, std::string description) {
			Matrix<T>* result = Matrix<T>::transpose(from);
			result->set_description(description);
			return result;
		}

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
