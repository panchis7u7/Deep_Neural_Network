#include <Matrix.hpp>
#include <tabulate/table.hpp>
#include <spdlog/spdlog.h>

using namespace voxel;

// Uploaded by panchis7u7 ~ Sebastian Madrigal

///////////////////////////////////////////////////////////////////////////////////////////
// Public typename Methods.
///////////////////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------------------------
// Constructor called when no rows and columns are supplied.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>::Matrix() {
	this->rows = 0;
	this->columns = 0;
	this->data = alloc(0, 0);
}

// ------------------------------------------------------------------------------------
// Constructor called when rows and columns are supplied.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>::Matrix(uint_fast64_t rows, uint_fast64_t columns) {
	this->rows = rows;
	this->columns = columns;
	this->data = alloc(rows, columns);
}

// ------------------------------------------------------------------------------------
// Copy constructor.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>::Matrix(Matrix<T> &copy) {
	this->rows = copy.rows;
	this->columns = copy.columns;
	this->data = alloc(copy.rows, copy.columns);
	// std::copy(std::begin(*(copy.data)), std::end(*(copy.data)), std::begin(*(this.data)));
}

// ------------------------------------------------------------------------------------
// Create a new Matrix from a unidimensional Vector<T>.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>::Matrix(std::vector<T> &vec) {
	std::size_t vec_size = vec.size();
	this->rows = vec_size;
	this->columns = 1;
	this->data = alloc(vec_size, 1);
	for (uint_fast64_t i = 0; i < vec_size; ++i) {
		this->data[i][0] = vec.at(i);
	}
}

// ------------------------------------------------------------------------------------
// Destructor.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>::~Matrix() {
	for (uint_fast64_t i = 0; i < this->rows; i++)
		delete[] data[i];
	delete[] data;
}

// ------------------------------------------------------------------------------------
// Pretty print method that prints a matrix contents to stdout.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::print() {
	tabulate::Table matrix_table;
	std::vector<variant<std::string, const char *, string_view, tabulate::Table>> row(this->columns);
	std::fill(row.begin(), row.end(), "");

	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			row.at(j) = std::to_string(this->data[i][j]);
		}
		matrix_table.add_row((const std::vector<variant<std::string, const char *, string_view, tabulate::Table>>)row);
	}

	if(!this->description.empty()) {
		tabulate::Table title_table;
		title_table.add_row(tabulate::Table::Row_t{this->description});
		title_table.column(0).format().font_align(tabulate::FontAlign::center);
		title_table.add_row(tabulate::Table::Row_t{matrix_table});
		std::cout << title_table << std::endl;

	} else {
		std::cout << matrix_table << std::endl;
	}
}

template <typename T>
tabulate::Table* Matrix<T>::get_pretty_table() {
	tabulate::Table* matrix_table = new tabulate::Table();
	std::vector<variant<std::string, const char *, string_view, tabulate::Table>> row(this->columns);
	std::fill(row.begin(), row.end(), "");

	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			row.at(j) = std::to_string(this->data[i][j]);
		}
		matrix_table->add_row((const std::vector<variant<std::string, const char *, string_view, tabulate::Table>>)row);
	}

	if(!this->description.empty()) {
		tabulate::Table* title_table = new tabulate::Table();
		title_table->add_row(tabulate::Table::Row_t{this->description});
		title_table->column(0).format().font_align(tabulate::FontAlign::center);
		title_table->add_row(tabulate::Table::Row_t{*matrix_table});
		std::cout << title_table << std::endl;
		return title_table;
	}
	return matrix_table;
}

// ------------------------------------------------------------------------------------
// Setter function to change a Matrix<T> internal description.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::set_description(std::string description) {
	this->description = description;
}

// ------------------------------------------------------------------------------------
// Add a scalar to a Matrix<T>.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::add(T addend) {
	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			this->data[i][j] += addend;
		}
	}
}

// ------------------------------------------------------------------------------------
// Subtract Matrix<T> corresponding cells, it must that have the same dimensions.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::subtract(Matrix<T>* subtrahend) {
	if ((this->rows != subtrahend->rows) || (this->columns != subtrahend->columns)) {
		spdlog::error("Trying to perform a substraction operation on non-matching matrices: \n 1: {} \n 2: {}", this->description, subtrahend->description);
		return;
	}

	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < subtrahend->columns; j++) {
			this->data[i][j] -= subtrahend->data[i][j];
		}
	}
}

// ------------------------------------------------------------------------------------
// Add Matrix<T> corresponding cells, it must that have the same dimensions.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::add(Matrix<T>* addend) {
	if ((this->rows != addend->rows) || (this->columns != addend->columns)) {
		spdlog::error("Trying to perform a substraction operation on non-matching matrices: \n 1: {} \n 2: {}", this->description, addend->description);
		return;
	}

	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			this->data[i][j] += addend->data[i][j];
		}
	}
}

// ------------------------------------------------------------------------------------
// Multiply a Matrix<T>, allocating a new buffer and de-allocating the old one.
// ------------------------------------------------------------------------------------

template <class T>
void Matrix<T>::multiply(Matrix<T>& multiplier) {
	T** new_data = alloc(this->rows, multiplier.columns);

	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < multiplier.columns; j++) {
			for (uint_fast64_t k = 0; k < this->columns; k++) {
				new_data[i][j] += this->data[i][k] * multiplier.data[k][j];
			}
		}
	}

	this->columns = multiplier.columns;
	delete this->data;
	this->data = new_data;
}

// ------------------------------------------------------------------------------------
// Initialize a Matrix<T> (self) cells with random values.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::randomize() {
	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			// Generate a random number between -1 and 1.
			this->data[i][j] = (-1) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1 - (-1))));
		}
	}
}

// ------------------------------------------------------------------------------------
// Transpose a Matrix<T> (self) data buffer by allocating a new buffer, and de-allocating
// the previously existing one.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::transpose() {
	T** new_data = this->alloc(this->columns, this->rows);
	
	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			new_data[j][i] = this->data[i][j];
		}
	}

	std::swap(this->rows, this->columns);
	delete this->data;
	this->data = new_data;
}

// ------------------------------------------------------------------------------------
// Perform a Matrix<T> (self) scalar product.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::scalar_product(T multiplier) {
	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			this->data[i][j] *= multiplier;
		}
	}
}

// ------------------------------------------------------------------------------------
// Multiply by a Matrix<T> corresponding cells, it must that have the same dimensions.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::hadamard_product(Matrix<T>* multiplier) {
	if ((this->rows != multiplier->rows) || (this->columns != multiplier->columns)) {
		spdlog::error("Trying to perform a hadamard product operation on non-matching matrices: \n 1: {} \n 2: {}", this->description, multiplier->description);
		return;
	}

	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			this->data[i][j] *= multiplier->data[i][j];
		}
	}
}

// ------------------------------------------------------------------------------------
// Apply a callback function (lambda) to mutate the data based on custom behavior. 
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::for_each(std::function<void(T data, unsigned row, unsigned column)> callback) {
	for (unsigned i = 0; i < this->rows; ++i) {
		for (unsigned j = 0; j < this->columns; ++j) {
			callback(this->data[i][j], i, j);
		}
	}
}

// ------------------------------------------------------------------------------------
// Apply a mutation function to all the cell data within a matrix.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::map(T (*func)(T)) {
	for (uint_fast64_t i = 0; i < this->rows; i++) {
		for (uint_fast64_t j = 0; j < this->columns; j++) {
			this->data[i][j] = func(this->data[i][j]);
		}
	}
}

// ------------------------------------------------------------------------------------
// Function to perform a cell buffer re-allocation.
// ------------------------------------------------------------------------------------

template <typename T>
T** Matrix<T>::alloc(uint_fast64_t rows, uint_fast64_t columns) {
	T** new_data = new T *[rows];
	for (uint_fast64_t i = 0; i < rows; i++) {
		new_data[i] = new T[columns];
	}
	return new_data;
}

// ------------------------------------------------------------------------------------
// Getter function to retrieve the matrices row count.
// ------------------------------------------------------------------------------------

template <typename T>
unsigned Matrix<T>::get_rows() { return this->rows; }

// ------------------------------------------------------------------------------------
// Getter function to retrieve the matrices column count.
// ------------------------------------------------------------------------------------

template <typename T>
unsigned Matrix<T>::get_columns() { return this->columns; }

// ------------------------------------------------------------------------------------
// Getter function to retrieve the matrices cell data buffer.
// ------------------------------------------------------------------------------------

template <typename T>
T **Matrix<T>::get_data() { return this->data; }

// #########################################################################################
// Public static typename Methods.
// #########################################################################################

// ------------------------------------------------------------------------------------
// Create a new Matrix<T> object from a Vector<T> class.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>* Matrix<T>::from_vector(std::vector<T>* from) {
	Matrix<T> *result = new Matrix(from->size(), 1);
	for (uint_fast64_t i = 0; i < from->size(); i++)
		result->data[i][0] = from->at(i);
	return result;
}

// ------------------------------------------------------------------------------------
// Create a new Vector<T> object from a Matrix<T> class; it flattens out a Matrix.
// ------------------------------------------------------------------------------------

template <typename T>
std::vector<T>* Matrix<T>::to_vector(Matrix<T>* from) {
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

template <typename T>
Matrix<T>* Matrix<T>::hadamard_product(Matrix<T>* multiplicand, Matrix<T>* multiplier) {
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

template <typename T>
Matrix<T>* Matrix<T>::substract(Matrix<T>* minuend, Matrix<T>* subtrahend) {
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

template <typename T>
void Matrix<T>::subtract(Matrix<T>* to, Matrix<T>* lhs, Matrix<T>* rhs) {
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

template <typename T>
Matrix<T>* Matrix<T>::multiply(Matrix<T>* multiplicand, Matrix<T>* multiplier) {
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

template <typename T>
Matrix<T>* Matrix<T>::multiply(Matrix<T>* multiplicand, std::vector<T>* multiplier) {
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

template <typename T>
void Matrix<T>::multiply(Matrix<T>* to, Matrix<T>* multiplicand, Matrix<T>* multiplier) {
	for (uint_fast64_t i = 0; i < multiplicand->rows; ++i) {
		for (uint_fast64_t j = 0; j < multiplier->columns; ++j) {
			for (uint_fast64_t k = 0; k < multiplicand->columns; ++k) {
				to->data[i][j] += multiplicand->data[i][k] * multiplier->data[k][j];
			}
		}
	}
}

// ------------------------------------------------------------------------------------
// Multiply a Matrix<T> objects with a Vector<T> and save it to a previously allocated 
// memory address The result of matrix multiplication is another matrix.
// ------------------------------------------------------------------------------------

template <typename T>
void Matrix<T>::multiply(Matrix<T>* to, Matrix<T>* multiplicand, std::vector<T>* multiplier) {
	// n Column Matrix requires n elements vector in order to perform product.
	for (uint_fast64_t i = 0; i < multiplicand->rows; i++) {
		for (uint_fast64_t j = 0; j < to->columns; j++) {
			for (uint_fast64_t k = 0; k < multiplicand->columns; k++) {
				to->data[i][j] += multiplicand->data[i][k] * multiplier->at(k);
			}
		}
	}
}

// ------------------------------------------------------------------------------------
// Transpose a Matrix<T> object and allocate new memory to this new dimension Matrix.
// ------------------------------------------------------------------------------------

template <typename T>
static Matrix<T>* Matrix<T>::transpose(Matrix<T>* from) {
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

template <typename T>
void Matrix<T>::transpose(Matrix<T>* to, Matrix<T>* from) {
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

template <typename T>
Matrix<T>* Matrix<T>::map(Matrix<T> *A, T (*func)(T)) {
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

template <typename T>
void Matrix<T>::map(Matrix<T>* to, Matrix<T>* from, T (*func)(T)) {
	for (uint_fast64_t i = 0; i < from->rows; i++) {
		for (uint_fast64_t j = 0; j < from->columns; j++) {
			to->data[i][j] = func(from->data[i][j]);
		}
	}
}

// ------------------------------------------------------------------------------------
// Duplicate an allocated Matrix<T> characteristics without copying its values.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>* Matrix<T>::duplicate_dimensions(Matrix<T>* from, std::string description) {
	Matrix<T>* result = new Matrix<T>(from->rows, from->columns);
	result->set_description(description);
	return result;
}

// ------------------------------------------------------------------------------------
// Duplicate an allocated Matrix<T> characteristics randomizing its contents.
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>* Matrix<T>::duplicate_randomize(Matrix<T>* from, std::string description) {
	Matrix<T>* result = new Matrix<T>(from->rows, from->columns);
	result->set_description(description);
	result->randomize();
	return result;
}

// ------------------------------------------------------------------------------------
// Duplicate an allocated Matrix<T> dimensions and transpose it (conserve its values).
// ------------------------------------------------------------------------------------

template <typename T>
Matrix<T>* Matrix<T>::duplicate_transpose(Matrix<T>* from, std::string description) {
	Matrix<T>* result = Matrix<T>::transpose(from);
	result->set_description(description);
	return result;
}

// ------------------------------------------------------------------------------------
// Template specialization definitions.
// ------------------------------------------------------------------------------------

template class voxel::Matrix<float>;
template class voxel::Matrix<double>;