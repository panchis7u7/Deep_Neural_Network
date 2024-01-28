#pragma once

#include <time.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <functional>

// Uploaded by panchis7u7 ~ Sebastian Madrigal
namespace voxel
{

	template <class T>
	class Matrix
	{
	public:
		Matrix();
		Matrix(uint_fast64_t rows, uint_fast64_t columns);
		Matrix(Matrix<T> &copy);
		Matrix(std::vector<T> &vec);
		~Matrix();
		void print();
		void setDescription(std::string);
		void add(T addend);
		void add(Matrix<T> *addend);
		void subtract(Matrix<T> *minuend);
		void subtract(std::vector<T> *minuend);
		void dot(Matrix<T> &multiplicand);
		void randomize();
		void transpose();
		void scalarProduct(T factor);
		void hadamardProduct(Matrix<T> *factor);
		void forEach(std::function<void(T data, unsigned row, unsigned column)> callback);
		void map(T (*func)(T));
		unsigned getRows();
		unsigned getColumns();
		T **getData();

		// Overloads.
		// friend std::ostream& operator<< <>(std::ostream& out, const Matrix<T>* mat);

		// Static methods.
		// static Matrix* fromVector(std::vector<T>* entradas);
		// static std::vector<T>* toVector(Matrix<T>* entradas);
		// static Matrix<T>* hadamardProduct(Matrix<T>* A, Matrix<T>* B);
		// static Matrix<T>* elementWiseSubstraction(Matrix<T>* A, Matrix<T>* B);
		// static Matrix<T>* dot(Matrix<T>* A, Matrix<T>* B);
		// static Matrix<T>* transpose(Matrix<T>* A);
		// static Matrix<T>* map(Matrix<T>* A, T (*func)(T));

		///////////////////////////////////////////////////////////////////////////////////////////
		// Public static typename Methods.
		///////////////////////////////////////////////////////////////////////////////////////////

		static Matrix<T> *fromVector(std::vector<T> *entradas)
		{
			Matrix<T> *result = new Matrix(entradas->size(), 1);
			for (uint_fast64_t i = 0; i < entradas->size(); i++)
			{
				result->data[i][0] = entradas->at(i);
			}
			return result;
		}

		static std::vector<T> *toVector(Matrix<T> *entradas)
		{
			std::vector<T> *result = new std::vector<T>();
			for (uint_fast64_t i = 0; i < entradas->rows; i++)
			{
				for (uint_fast64_t j = 0; j < entradas->columns; j++)
				{
					result->push_back(entradas->data[i][j]);
				}
			}
			return result;
		}

		static Matrix<T> *hadamardProduct(Matrix<T> *A, Matrix<T> *B)
		{
			if ((A->rows != B->rows) || (A->columns != B->columns))
			{
				return NULL;
			}
			else
			{
				Matrix<T> *result = new Matrix<T>(A->rows, B->columns);
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

		static Matrix<T> *elementWiseSubstraction(Matrix<T> *A, Matrix<T> *B)
		{
			if ((A->rows != B->rows) || (A->columns != B->columns))
			{
				return NULL;
			}
			else
			{
				Matrix<T> *result = new Matrix<T>(A->rows, B->columns);
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

		static void element_wise_substraction(Matrix<T>* to, Matrix<T>* lhs, Matrix<T>* rhs) {
			if ((lhs->rows != rhs->rows) || (lhs->columns != rhs->columns))
				return;

			for (uint_fast64_t i = 0; i < lhs->rows; i++) {
				for (uint_fast64_t j = 0; j < rhs->columns; j++) {
					to->data[i][j] = lhs->data[i][j] - rhs->data[i][j];
				}
			}
		}

		static Matrix<T> *dot(Matrix<T> *A, Matrix<T> *B)
		{
			Matrix<T> *result = new Matrix<T>(A->rows, B->columns);
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

		static Matrix<T> *dot(Matrix<T> *A, std::vector<T> *B)
		{
			// n Column Matrix requires n elements vector in order to perform product.
			Matrix<T> *result = new Matrix<T>(A->rows, 1);
			for (uint_fast64_t i = 0; i < result->rows; i++)
			{
				for (uint_fast64_t j = 0; j < result->columns; j++)
				{
					T sum = 0;
					for (uint_fast64_t k = 0; k < A->columns; k++)
					{
						sum += A->data[i][k] * B->at(k);
					}
					result->data[i][j] = sum;
				}
			}
			return result;
		}

		static void dot(Matrix<T> *to, Matrix *aOperand, Matrix<T> *bOperand)
		{
			for (uint_fast64_t i = 0; i < aOperand->rows; ++i)
			{
				for (uint_fast64_t j = 0; j < bOperand->columns; ++j)
				{
					for (uint_fast64_t k = 0; k < aOperand->columns; ++k)
					{
						to->data[i][j] += aOperand->data[i][k] * bOperand->data[k][j];
					}
				}
			}
		}

		static Matrix<T> *transpose(Matrix<T> *A)
		{
			Matrix<T> *result = new Matrix<T>(A->columns, A->rows);
			for (uint_fast64_t i = 0; i < A->rows; i++)
			{
				for (uint_fast64_t j = 0; j < A->columns; j++)
				{
					result->data[j][i] = A->data[i][j];
				}
			}
			return result;
		}

		static Matrix<T> *map(Matrix<T> *A, T (*func)(T))
		{
			Matrix<T> *result = new Matrix<T>(A->rows, A->columns);
			for (uint_fast64_t i = 0; i < A->rows; i++)
			{
				for (uint_fast64_t j = 0; j < A->columns; j++)
				{
					result->data[i][j] = func(A->data[i][j]);
				}
			}
			return result;
		}

		static Matrix<T>* duplicate_dimensions(Matrix<T>* from, std::string description) {
			Matrix<T>* result = new Matrix<T>(from->rows, from->columns);
			result->setDescription(description);
			return result;
		}

		static Matrix<T>* duplicate_randomize(Matrix<T>* from, std::string description) {
			Matrix<T>* result = new Matrix<T>(from->rows, from->columns);
			result->setDescription(description);
			result->randomize();
			return result;
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Operator Overloading.
		///////////////////////////////////////////////////////////////////////////////////////////

		friend std::ostream &operator<<(std::ostream &out, const Matrix<T> *mat)
		{
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
		T **data;
		unsigned rows;
		unsigned columns;
		T **alloc(uint_fast64_t rows, uint_fast64_t columns);
		std::string description;

	protected:
	};
}
