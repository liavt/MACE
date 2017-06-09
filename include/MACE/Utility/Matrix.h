/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_MATRIX_H
#define MACE_UTILITY_MATRIX_H

#include <MACE/Utility/Vector.h>

namespace mc {
	/**
	Used in the `Matrix` class. Defined for clarity for when you iterate over a `Matrix.`
	@see Matrix
	@tparam T What data type the row is made of
	@tparam N The width of the row
	*/
	template <typename T, Size N>
	using MatrixRow = mc::Vector<T, N>;//this is for clarity

	template<typename T, Size W, Size H>
	struct Matrix;

	namespace math {
		template<typename T, Size N>
		Matrix<T, N, N> inverse(const Matrix<T, N, N>&);
	}//math

	/**
	A class representing a 2-dimensional matrix, and allows for math involving matrices. A `Matrix` can be known as a `Vector` of `Vectors`.
	<p>
	`Matrices` can be added, subtracted, and multiplyed by eachother, and by `Vectors` of equal width.
	<p>
	Examples:
	{@code
	Matrix<int,3,4> matrix = Matrix<int,3,4>()//Create a 3 by 4 Matrix of ints. By default, every value is 0

	int arr[3][4] = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
	matrix = arr;//Create a Matrix from an existing array. The array must be the same size as the Matrix.

	arr.get(x,y);//Retrieve the value at x and y
	arr[x][y];//Retrieve the value at x and y

	arr.set(x,y,value); //Set the int at x and y to a new  value
	arr[x][y]=value; //Set the int at x and y to a new value

	//Iterate over a Matrix's contents:
	for(Index x = 0;x<matrix.width();x++){
	for(Index y = 0;y<matrix.height();y++){
	int value = matrix[x][y];
	}
	}

	matrix.size();//Get how many values the Matrix is holding
	}
	<p>
	There are various type aliases in place to prevent using the template parameters. They all use the following syntax:
	`Matrix[size][prefix]` or `Matrix[width]x[height][prefix]`
	<p>
	Prefixes exist for every primitive type and are the first letter of the primitive name. For example, the prefix
	for a `float` would be `f` and the prefix for an `int` would be `i`. Primitives with modifiers simply add the
	letter. The prefixed for an `unsigned char` would be `uc` and the prefix for a `long long int` would be `lli`
	<p>
	Sizes exist for matrices up to 5x5
	<p>
	For example, to create a `Matrix` that consists of floats and is 4 by 4 in size, you would use `Matrix4f`. For a 3 by 2
	`Matrix` of unsigned ints, you would use `Matrix3x2ui`
	@see Vector
	@tparam T What the `Matrix` should consist of. Can be any type.
	@tparam W The width of the `Matrix` which must be greater than 0
	@tparam H The height of the `Matrix` which must be greater than 0
	*/
	template<typename T, Size W, Size H>
	struct Matrix: public Vector<MatrixRow<T, H>, W> {
		/**
		Default constructor. Creates a `Matrix` of the specified size where every spot is unallocated
		*/
		Matrix() : Vector<MatrixRow<T, H>, W>()//extending defautl construtor so it initializes the array
		{
			static_assert(W != 0, "A Matrix's width must be greater than 0!");
			static_assert(H != 0, "A Matrix's height must be greater than 0!");
			for( Index i = 0; i < W; i++ ) {
				content[i] = MatrixRow<T, H>();
			}
		};

		/**
		Creates a `Matrix` based on a 2-dimensional array. The array's contents will be copied into this `Matrix`
		@param arr An array of contents
		*/
		Matrix(T arr[W][H]) : Matrix() {
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					content[x][y] = arr[x][y];
				}
			}
		}

		/**
		Creates a `Matrix` from an `std::initializer_list`. Allows for an aggregate-style creation.
		<p>
		Example:
		{@code
			Matrix2i mat = {{1,2}, {3, 4}};
		}
		@param args What to create this `Matrix` with
		@throws IndexOutOfBoundsException If the amount of arguments provided is not the same size as the `Matrix`
		*/
		Matrix(const std::initializer_list<const std::initializer_list<T>> args) : Matrix()//this is for aggregate initializaition
		{
			if( args.size() != W )throw IndexOutOfBoundsError("The width of the argument must be equal to to the height of the Matrix!");
			Index counterX = 0, counterY = 0;
			for( std::initializer_list<T> elemX : args ) {
				if( elemX.size() != H )throw IndexOutOfBoundsError("The height of the argument must be equal to to the height of the Matrix!");
				counterY = 0;
				for( T elemY : elemX ) {
					content[counterX][counterY] = elemY;
					counterY++;
				}

				counterX++;
			}
		}

		/**
		Copy constructor. Clones the contents of another `Matrix` into a new `Matrix`
		@param copy What the clone
		*/
		Matrix(const Matrix& copy) : Vector<MatrixRow<T, H>, W>(copy.content) {}

		/**
		Calculates how many elements are in this `Matrix`
		@return `width()` times `height()`
		@see #height()
		@see #width()
		*/
		constexpr Size size() const {
			return W*H;
		}

		/**
		Calculates the width of this `Matrix`.
		@return The width specified by the template.
		@see #height()
		@see #size()
		*/
		constexpr Size width() const {
			return W;
		}
		/**
		Calculates the height of this `Matrix`.
		@return The height specified by the template.
		@see #width()
		@see #size()
		*/
		constexpr Size height() const {
			return H;
		}

		/**
		Retrieves the content at a certain position
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the data at `X,Y`
		@throw IndexOutOfBoundsException if `x>=width()`
		@throw IndexOutOfBoundsException if `y>=height()`
		@see #set(Index, Index, T)
		@see #operator[]
		*/
		T& get(Index x, Index y) {
			if( x >= W ) throw IndexOutOfBoundsError(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if( y >= H ) throw IndexOutOfBoundsError(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			return content[x][y];
		}

		/**
		`const` version of {@link #get(Index,Index)}
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the `const` data at `X,Y`
		@throw IndexOutOfBoundExceptions if `x>=width()`
		@throw IndexOutOfBoundsException if `y>=height()`
		@see #set(Index, Index, T)
		@see #operator[]
		*/
		const T& get(Index x, Index y) const {
			if( x >= W ) throw IndexOutOfBoundsError(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if( y >= H ) throw IndexOutOfBoundsError(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			return content[x][y];
		}

		/**
		Writes data at certain coordinates to a new value.
		@param x X-coordinate of the new data
		@param y Y-coordinate of the new data
		@param value New data to write to `X,Y`
		@throw IndexOutOfBoundsException if `x>=width()`
		@throw IndexOutOfBoundsException if `y>=height()`
		@see #operator[]
		@see #get(Index, Index)
		*/
		void set(Index x, Index y, T value) {
			if( x >= W ) throw IndexOutOfBoundsError(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if( y >= H ) throw IndexOutOfBoundsError(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			content[x][y] = value;
		}

		/**
		Creates an 1-dimensional array with the data of this `Matrix`, in O(N) time
		@return Pointer to an array of data
		*/
		const T* flatten(T arr[W*H]) const {
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					arr[y + (x*H)] = (content[x][y]);
				}
			}
			return arr;
		}

		/**
		Creates an 1-dimensional array with the transposed data of this `Matrix`, in O(N) time.
		<p>
		This method is faster than `math::transpose(matrix).flatten(array)`, as that takes O(N*2) time.
		@return Pointer to an array of data
		*/
		const T* flattenTransposed(T arr[W*H]) const {
			for( Index x = 0; x < H; x++ ) {
				for( Index y = 0; y < W; y++ ) {
					arr[y + (x*H)] = (content[y][x]);
				}
			}
			return arr;
		}

		/**
		Retrieves content at a certain `Index`, not zero indexed.
		<p>
		Equal to {@code
		matrix[x-1][y-1]
		}
		@param x Not zero-indexed X-coordinate
		@param y Not zero-indexed y-coordinate
		@return Value at `x-1` and `y-1`
		@see operator[](Index)
		@see set(Index, Index,T)
		@see get(Index,Index)
		*/
		virtual T& operator()(Index x, Index y) {
			return content[x - 1][y - 1];
		}

		/**
		`const` version of `operator()(Index,Index)`.
		@param x Not zero-indexed X-coordinate
		@param y Not zero-indexed y-coordinate
		@return Value at `x-1` and `y-1`
		@see set(Index, Index,T)
		@see get(Index,Index)
		*/
		const T& operator()(Index x, Index y) const {
			return content[x - 1][y - 1];
		}

		/**
		Adds a `Vector` and a `Matrix` together

		@param right A `Vector` of equal width
		@return A `Vector` that was created by adding a `Vector` and a `Matrix` together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		@pre The Matrix's width must not be larger than the height
		*/
		Vector<T, H> operator+(const Vector<T, H>& right) const {
			static_assert(W <= H, "When doing Matrix by Vector math, the Matrix's width must not be larger than the height.");
			T arr[H];
			for( Index x = 0; x < W; x++ ) {
				arr[x] = T();//we must initialize the value first, or else it will be undefined
				for( Index y = 0; y < H; y++ ) {
					arr[y] += static_cast<T>(content[x][y]) + static_cast<T>(right[x]);
				}
			}
			return arr;
		};

		/**
		Subtracts a `Vector` and a `Matrix` together. The Matrix's `width()` must not be larger than `height()`

		@param right A `Vector` of equal width
		@return A `Vector` that was created by subtracting a `Vector` and a `Matrix` together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		@pre The Matrix's width must not be larger than the height
		*/
		Vector<T, H> operator-(const Vector<T, H>& right) const {
			static_assert(W <= H, "When doing Matrix by Vector math, the Matrix's width must not be larger than the height.");
			T arr[H];
			for( Index x = 0; x < W; x++ ) {
				arr[x] = T();//we must initialize the value first, or else it will be undefined
				for( Index y = 0; y < H; y++ ) {
					arr[y] += static_cast<T>(content[x][y]) - static_cast<T>(right[x]);
				}
			}
			return arr;
		};

		/**
		Multiplies a `Vector` and a `Matrix` together

		@param right A `Vector` of equal width
		@return A `Vector` that was created by multiplying a `Vector` and a `Matrix` together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		@pre The Matrix's width must not be larger than the height
		*/
		Vector<T, H> operator*(const Vector<T, H>& right) const {
			static_assert(W <= H, "When doing Matrix by Vector math, the Matrix's width must not be larger than the height.");
			T arr[H];
			for( Index x = 0; x < W; x++ ) {
				arr[x] = T();//we must initialize the value first, or else it will be undefined
				for( Index y = 0; y < H; y++ ) {
					arr[y] += static_cast<T>(content[x][y]) * static_cast<T>(right[x]);
				}
			}
			return arr;
		};

		/**
		Adds 2 `Matrix` together. `width()` MUST be equal to `height()` in order to do `Matrix` math.
		@param right A `Matrix` to add
		@return A `Matrix` whose contents is 2 `Matrix` added together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		@pre Both Matrices' width and height must be equal to each other
		*/
		Matrix<T, W, H> operator+(const Matrix<T, W, H>& right) const {
			static_assert(W == H, "In order to do Matrix math, the width and height of both Matrices have to be be equal.");
			T arr[W][H];
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					arr[x][y] = content[x][y] + right[x][y];
				}
			}
			return arr;
		}

		/**
		Subtracts 2 `Matrix` together.
		@param right A `Matrix` to subtracts
		@return A `Matrix` whose contents is 2 `Matrix` subtracted together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		@pre Both Matrices' width and height must be equal to each other
		*/
		Matrix<T, W, H> operator-(const Matrix<T, W, H>& right) const {
			static_assert(W == H, "In order to do Matrix math, the width and height of both Matrices have to be be equal.");
			T arr[W][H];
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					arr[x][y] = content[x][y] - right[x][y];
				}
			}
			return arr;
		}

		/**
		Multiplies 2 `Matrix` together. `width()` must equal `height()`, or else this will error.
		@param right A `Matrix` to multiply
		@return A `Matrix` whose contents is 2 `Matrix` multiplied together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		@see operator*(const T&) const
		@pre Both Matrices' width and height must be equal to each other
		*/
		Matrix<T, W, H> operator*(const Matrix<T, W, H>& right) const {
			static_assert(W == H, "In order to multiply matrices, the width must equal to the height.");
			T arr[W][H];
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					arr[x][y] = T();
					for( Index x1 = 0; x1 < W; x1++ ) {
						arr[x][y] += content[x][x1] * right[x1][y];
					}
				}
			}
			return arr;
		}

		/**
		Scales a `Matrix` from a scalar value.
		@param scalar What to multiply each value of this `Matrix` by.
		@return A `Matrix` multiplied by the scalar
		@see operator*(const Matrix&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		@pre Both Matrices' width and height must be equal to each other
		*/
		Matrix<T, W, H> operator*(const T& scalar) const {
			T arr[W][H];
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					arr[x][y] = content[x][y] * scalar;
				}
			}
			return arr;
		}

		/**
		Divides 2 `Matrix` together. `width()` must equal `height()`, or else this will error.
		@param right A `Matrix` to divide
		@return A `Matrix` whose contents is 2 `Matrix` divide together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		@pre Both Matrices' width and height must be equal to each other
		@bug Not finished
		*/
		Matrix<T, W, H> operator/(const Matrix<T, W, H>& right) const {
			static_assert(W == H, "In order to divide matrices, the width must equal to the height.");
			return this*math::inverse(right);
		}

		/**
		Adds a `Matrix` to this one.
		@param right A `Matrix` to add
		@see operator+(const Matrix<T,W,H>&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		void operator+=(const Matrix<T, W, H>& right) {
			setContents((*this + right).getContents());
		}

		/**
		Subtracts a `Matrix` from this one.
		@param right A `Matrix` to subtracts
		@see operator-(const Matrix<T,W,H>&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		void operator-=(const Matrix<T, W, H>& right) {
			setContents((*this - right).getContents());
		}

		/**
		Multiplies `this` by a `Matrix`
		@param right A `Matrix` to multiply
		@see operator*(const Matrix<T,W,H>&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		void operator*=(Matrix<T, W, H>& right) {
			setContents((*this*right).getContents());
		}

		/**
		Multiplies `this` by a scalar
		@param scalar What to multiply each value of this `Matrix` by
		@see operator*(const T&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		void operator*=(const T& scalar) {
			setContents((*this*scalar).getContents());
		}

		/**
		Divides a `Matrix` from this one.
		@param right A `Matrix` to divide
		@see operator/(const Matrix<T,W,H>&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		void operator/=(const Matrix<T, W, H>& right) {
			setContents((*this / right).getContents());
		}

		operator Vector<MatrixRow<T, H>, W>&() {
			return content;
		}

		operator Vector<MatrixRow<T, H>, W>() {
			return content;
		}

		/**
		Operator used to output to `std::cout`.
		<p>
		The output will take up multiple lines and will show all of the `Matrix` contents
		@param output `std::ostream` the `Matrix` was inserted into
		@param m `Matrix` which will be printed
		@return `output` for chaining
		@bug the order is not retained exactly
		*/
		friend std::ostream &operator<<(std::ostream &output,
										const Matrix<T, W, H> &m) {
			for( Index x = 0; x < W; x++ ) {
				if( x == 0 )output << '[' << ' ';//why this and not "[ "? that requires #include <string> and thats more compilition time. this way doesnt have any difference in output and doesnt need to incldue that
				else output << std::endl << ' ' << ' ';
				for( Index y = 0; y < H; y++ ) {
					output << m[x][y];
					if( y < H - 1 )output << ',' << ' ';
				}
			}
			output << ' ' << ']';
			return output;
		}
	protected:
		using Vector<MatrixRow<T, H>, W>::content;//some compilers need this line even though content is protected from the Vector inheritance
	};//Matrix

	template<typename T, Size N>
	Vector<T, N> operator*=(const Vector<T, N>& v, const Matrix<T, N, N>& m) {
		return m * v;
	}

	template<typename T, Size N>
	Vector<T, N> operator*(const Vector<T, N>& v, const Matrix<T, N, N>& m) {
		return m * v;
	}

	namespace math {
		/**
		Transposes a `Matrix`. Transposing a `Matrix` creates a reflection of it, where every row is a column.
		@param matrix What to transpose
		@return A reflected `matrix`
		@tparam T Type of the `Matrix`
		@tparam W Width of the `Matrix`
		@tparam H Height of the `Matrix`
		*/
		template<typename T, Size W, Size H>
		Matrix<T, H, W> transpose(const Matrix<T, W, H>& matrix) {
			Matrix<T, H, W> out = Matrix<T, H, W>();
			for( Index x = 0; x < W; x++ ) {
				for( Index y = 0; y < H; y++ ) {
					out[y][x] = matrix[x][y];
				}
			}
			return out;
		}

		/**
		Calculates the determinate of a 2x2 `Matrix`.
		@param matrix A square 2x2 `Matrix` to find the determinate of
		@return The determinate of `matrix`
		@tparam T Type of the `Matrix`
		*/
		template<typename T>
		T det(const Matrix<T, 2, 2>& matrix) {
			return (matrix[0][0] * matrix[1][1]) - (matrix[0][1] * matrix[1][0]);
		}

		/**
		Calculates the determinate of a any sized `Matrix`.
		@param matrix A square `Matrix` to find the determinate of
		@return The determinate of `matrix`
		@bug Array subscript out of range with matrices bigger than 2x2
		@tparam T Type of the `Matrix`
		@tparam N Size of the `Matrix`
		@pre The size of the `Matrix` must be larger than 1
		*/
		template<typename T, Size N>
		T det(const Matrix<T, N, N>& matrix) {
			static_assert(N >= 2, "In order to retrieve the determinate of a Matrix, its size must be bigger than 1");
			T sum = T();
			Index counter = 0;
			for( Index i = 0; i < N; i++ ) {
				constexpr Size newMatrixSize = N - 1;
				Matrix<T, newMatrixSize, newMatrixSize> m = Matrix<T, newMatrixSize, newMatrixSize>();
				Index counterX = 0;
				Index counterY = 0;
				for( Index x = 0; x < N; x++ ) {
					for( Index y = 1; y < N; y++ ) {
						if( x != i ) {
							m[counterX][counterY] = matrix[x][y];
							counterX++;
						}
						counterY++;

					}
					counterY = 0;
				}
				T tempSum = ((counter % 2 == 0 ? -1 : 1) * matrix[i][0]);
				if( newMatrixSize == 2 ) {
					tempSum *= det<T>(matrix);
				} else {
					tempSum *= det<T, N>(matrix);
				}
				sum += tempSum;
				counter++;
			}
			return sum;
		}



		/**
		Calculates the trace of a Matrix. The trace is the sum of all of the diagonal elements of a `Matrix`.
		<p>
		The trace is related to the derivative of a {@link #det() determinate.}
		<p>
		Not to be confused with `transpose(const Matrix&))`
		@param m `Matrix` to calculate the trace of
		@return The sum of the diagonal elements of `m`
		@tparam T Type of the `Matrix`
		@tparam N Size of the `Matrix`
		@see #inverse(const Matrix<T,N,N>&)
		*/
		template<typename T, Size N>
		T tr(const Matrix<T, N, N>& m) {
			T out = T();
			//done without a trace!
			for( Index x = 0; x < N; x++ ) {
				out += m[x][x];
			}
			return out;
		}

		/**
		Creates an indentity `Matrix` of a certain size. An identity `Matrix` times another `Matrix` equals the same `Matrix`
		@return An indentity `Matrix` whose diagonal elements are `1`
		@tparam T Type of the identity `Matrix`
		@tparam N Size of the identity `Matrix`
		*/
		template<typename T, Size N>
		Matrix<T, N, N> identity() {
			Matrix<T, N, N> out = Matrix<T, N, N>();
			for( Index x = 0; x < N; x++ ) {
				for( Index y = 0; y < N; y++ ) {
					if( x == y ) {
						out[x][y] = 1;
					} else {
						out[x][y] = 0;
					}
				}
			}
			return out;
		}

		/**
		Inverses a `N` by `N` `Matrix`. An inversed `Matrix` times a normal `Matrix` equals the identity `Matrix`
		<p>
		Not to be confused with `tr()`
		<p>
		If `T` is not a floating point type, the output may not work, as it will round.
		<p>
		The output is calculated via the Caley-Hamilton theorum (https://en.wikipedia.org/wiki/Cayley%E2%80%93Hamilton_theorem)
		@param matrix The `Matrix` to invert
		@return The inverse of `matrix`
		@tparam T Type of the `Matrix`
		@tparam N Order of the `Matrix`
		@pre The size of the `Matrix` must be greater than 1
		@bug Matrices bigger then 2x2 dont work
		*/
		template<typename T, Size N>
		Matrix<T, N, N> inverse(const Matrix<T, N, N>& matrix) {
			static_assert(N >= 2, "In order to inverse a matrix, it's size must be greater than 1!");
			//honestly, this next line really seems magical to me. matrices really seem magical in general. But
			//this especialy. matrices are really something, aren't they?
			return	(((identity<T, N>()*tr(matrix)) - matrix) * (1 / det(matrix)));//calculate via the caley-hamilton method
		}
	}//math
}//mc

#endif