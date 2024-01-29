#include <include/Matrix.hpp>
#include <tabulate/table.hpp>

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

// ------------------------------------------------------------------------------------
// Template specialization definitions.
// ------------------------------------------------------------------------------------

template class voxel::Matrix<float>;
template class voxel::Matrix<double>;