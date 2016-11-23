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
	@see Vector
	@tparam T What the `Matrix` should consist of
	@tparam W The width of the `Matrix`
	@tparam H The height of the `Matrix`
	*/
	template<typename T, Size W, Size H>
	struct Matrix: public Vector<MatrixRow<T, H>, W> {
		using Vector<MatrixRow<T, H>, W>::content;//some compilers need this line even though content is protected

		/**
		Default constructor. Creates a `Matrix` of the specified size where every spot is unallocated
		*/
		Matrix() : Vector<MatrixRow<T, H>, W>()//extending defautl construtor so it initializes the array
		{
			static_assert(W != 0, "A Matrix's width must be greater than 0!");
			static_assert(H != 0, "A Matrix's height must be greater than 0!");
			for( Index i = 0; i < content.size(); i++ ) {
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


		Matrix(const std::initializer_list<const std::initializer_list<T>> args) : Matrix()//this is for aggregate initializaition
		{
			if( args.size() != W )throw IndexOutOfBoundsException("The width of the argument must be equal to to the height of the Matrix!");
			Index counterX = 0, counterY = 0;
			for( std::initializer_list<T> elemX : args ) {
				if( elemX.size() != H )throw IndexOutOfBoundsException("The height of the argument must be equal to to the height of the Matrix!");
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
		@throw IndexOutOfBounds if `x>=width()`
		@throw IndexOutOfBounds if `y>=height()`
		@see #set(Index, Index, T)
		@see #operator[]
		*/
		T& get(Index x, Index y) {
			if( x >= W ) throw IndexOutOfBoundsException(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if( y >= H ) throw IndexOutOfBoundsException(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			return content[x][y];
		}

		/**
		`const` version of {@link #get(Index,Index)}
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the `const` data at `X,Y`
		@throw IndexOutOfBounds if `x>=width()`
		@throw IndexOutOfBounds if `y>=height()`
		@see #set(Index, Index, T)
		@see #operator[]
		*/
		const T& get(Index x, Index y) const {
			if( x >= W ) throw IndexOutOfBoundsException(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if( y >= H ) throw IndexOutOfBoundsException(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			return content[x][y];
		}

		/**
		Writes data at certain coordinates to a new value.
		@param x X-coordinate of the new data
		@param y Y-coordinate of the new data
		@param value New datat to write to `X,Y`
		@throw IndexOutOfBounds if `x>=width()`
		@throw IndexOutOfBounds if `y>=height()`
		@see #operator[]
		@see #get(Index, Index)
		*/
		void set(Index x, Index y, T value) {
			if( x >= W ) throw IndexOutOfBoundsException(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if( y >= H ) throw IndexOutOfBoundsException(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
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
		@throw ArithmeticError If `width()` is greater than `height()`
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
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
		@throw ArithmeticError If `width()` is greater than `height()`
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
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

	};//Matrix

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
		*/
		template<typename T, Size N>
		T det(const Matrix<T, N, N>& matrix) {
			static_assert(N >= 2, "In order to retrieve the determinate of a Matrix, its size must be bigger than 1");
			T sum = T();
			Index counter = 0;
			for( Index i = 0; i < N; i++ ) {
				const Size newMatrixSize = N - 1;
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
				if( N - 1 == 2 ) {
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

	/*Autogenerated aliases for every single combination of matrix up to 5x5 and using every primitive type*/
	using Matrix1i = Matrix< int, 1, 1 >; using Matrix2i = Matrix< int, 2, 2 >; using Matrix3i = Matrix< int, 3, 3 >; using Matrix4i = Matrix< int, 4, 4 >; using Matrix5i = Matrix< int, 5, 5 >; using Matrix2x3i = Matrix< int, 2, 3 >; using Matrix3x2i = Matrix< int, 3, 2 >; using Matrix2x4i = Matrix< int, 2, 4 >; using Matrix4x2i = Matrix< int, 4, 2 >; using Matrix2x5i = Matrix< int, 2, 5 >; using Matrix5x2i = Matrix< int, 5, 2 >; using Matrix3x4i = Matrix< int, 3, 4 >; using Matrix4x3i = Matrix< int, 4, 3 >; using Matrix3x5i = Matrix< int, 3, 5 >; using Matrix5x3i = Matrix< int, 5, 3 >; using Matrix4x5i = Matrix< int, 4, 5 >; using Matrix5x4i = Matrix< int, 5, 4 >; using Matrix1ui = Matrix< unsigned int, 1, 1 >; using Matrix2ui = Matrix< unsigned int, 2, 2 >; using Matrix3ui = Matrix< unsigned int, 3, 3 >; using Matrix4ui = Matrix< unsigned int, 4, 4 >; using Matrix5ui = Matrix< unsigned int, 5, 5 >; using Matrix2x3ui = Matrix< unsigned int, 2, 3 >; using Matrix3x2ui = Matrix< unsigned int, 3, 2 >; using Matrix2x4ui = Matrix< unsigned int, 2, 4 >; using Matrix4x2ui = Matrix< unsigned int, 4, 2 >; using Matrix2x5ui = Matrix< unsigned int, 2, 5 >; using Matrix5x2ui = Matrix< unsigned int, 5, 2 >; using Matrix3x4ui = Matrix< unsigned int, 3, 4 >; using Matrix4x3ui = Matrix< unsigned int, 4, 3 >; using Matrix3x5ui = Matrix< unsigned int, 3, 5 >; using Matrix5x3ui = Matrix< unsigned int, 5, 3 >; using Matrix4x5ui = Matrix< unsigned int, 4, 5 >; using Matrix5x4ui = Matrix< unsigned int, 5, 4 >; using Matrix1si = Matrix< signed int, 1, 1 >; using Matrix2si = Matrix< signed int, 2, 2 >; using Matrix3si = Matrix< signed int, 3, 3 >; using Matrix4si = Matrix< signed int, 4, 4 >; using Matrix5si = Matrix< signed int, 5, 5 >; using Matrix2x3si = Matrix< signed int, 2, 3 >; using Matrix3x2si = Matrix< signed int, 3, 2 >; using Matrix2x4si = Matrix< signed int, 2, 4 >; using Matrix4x2si = Matrix< signed int, 4, 2 >; using Matrix2x5si = Matrix< signed int, 2, 5 >; using Matrix5x2si = Matrix< signed int, 5, 2 >; using Matrix3x4si = Matrix< signed int, 3, 4 >; using Matrix4x3si = Matrix< signed int, 4, 3 >; using Matrix3x5si = Matrix< signed int, 3, 5 >; using Matrix5x3si = Matrix< signed int, 5, 3 >; using Matrix4x5si = Matrix< signed int, 4, 5 >; using Matrix5x4si = Matrix< signed int, 5, 4 >; using Matrix1f = Matrix< float, 1, 1 >; using Matrix2f = Matrix< float, 2, 2 >; using Matrix3f = Matrix< float, 3, 3 >; using Matrix4f = Matrix< float, 4, 4 >; using Matrix5f = Matrix< float, 5, 5 >; using Matrix2x3f = Matrix< float, 2, 3 >; using Matrix3x2f = Matrix< float, 3, 2 >; using Matrix2x4f = Matrix< float, 2, 4 >; using Matrix4x2f = Matrix< float, 4, 2 >; using Matrix2x5f = Matrix< float, 2, 5 >; using Matrix5x2f = Matrix< float, 5, 2 >; using Matrix3x4f = Matrix< float, 3, 4 >; using Matrix4x3f = Matrix< float, 4, 3 >; using Matrix3x5f = Matrix< float, 3, 5 >; using Matrix5x3f = Matrix< float, 5, 3 >; using Matrix4x5f = Matrix< float, 4, 5 >; using Matrix5x4f = Matrix< float, 5, 4 >; using Matrix1d = Matrix< double, 1, 1 >; using Matrix2d = Matrix< double, 2, 2 >; using Matrix3d = Matrix< double, 3, 3 >; using Matrix4d = Matrix< double, 4, 4 >; using Matrix5d = Matrix< double, 5, 5 >; using Matrix2x3d = Matrix< double, 2, 3 >; using Matrix3x2d = Matrix< double, 3, 2 >; using Matrix2x4d = Matrix< double, 2, 4 >; using Matrix4x2d = Matrix< double, 4, 2 >; using Matrix2x5d = Matrix< double, 2, 5 >; using Matrix5x2d = Matrix< double, 5, 2 >; using Matrix3x4d = Matrix< double, 3, 4 >; using Matrix4x3d = Matrix< double, 4, 3 >; using Matrix3x5d = Matrix< double, 3, 5 >; using Matrix5x3d = Matrix< double, 5, 3 >; using Matrix4x5d = Matrix< double, 4, 5 >; using Matrix5x4d = Matrix< double, 5, 4 >; using Matrix1ld = Matrix< long double, 1, 1 >; using Matrix2ld = Matrix< long double, 2, 2 >; using Matrix3ld = Matrix< long double, 3, 3 >; using Matrix4ld = Matrix< long double, 4, 4 >; using Matrix5ld = Matrix< long double, 5, 5 >; using Matrix2x3ld = Matrix< long double, 2, 3 >; using Matrix3x2ld = Matrix< long double, 3, 2 >; using Matrix2x4ld = Matrix< long double, 2, 4 >; using Matrix4x2ld = Matrix< long double, 4, 2 >; using Matrix2x5ld = Matrix< long double, 2, 5 >; using Matrix5x2ld = Matrix< long double, 5, 2 >; using Matrix3x4ld = Matrix< long double, 3, 4 >; using Matrix4x3ld = Matrix< long double, 4, 3 >; using Matrix3x5ld = Matrix< long double, 3, 5 >; using Matrix5x3ld = Matrix< long double, 5, 3 >; using Matrix4x5ld = Matrix< long double, 4, 5 >; using Matrix5x4ld = Matrix< long double, 5, 4 >; using Matrix1s = Matrix< short int, 1, 1 >; using Matrix2s = Matrix< short int, 2, 2 >; using Matrix3s = Matrix< short int, 3, 3 >; using Matrix4s = Matrix< short int, 4, 4 >; using Matrix5s = Matrix< short int, 5, 5 >; using Matrix2x3s = Matrix< short int, 2, 3 >; using Matrix3x2s = Matrix< short int, 3, 2 >; using Matrix2x4s = Matrix< short int, 2, 4 >; using Matrix4x2s = Matrix< short int, 4, 2 >; using Matrix2x5s = Matrix< short int, 2, 5 >; using Matrix5x2s = Matrix< short int, 5, 2 >; using Matrix3x4s = Matrix< short int, 3, 4 >; using Matrix4x3s = Matrix< short int, 4, 3 >; using Matrix3x5s = Matrix< short int, 3, 5 >; using Matrix5x3s = Matrix< short int, 5, 3 >; using Matrix4x5s = Matrix< short int, 4, 5 >; using Matrix5x4s = Matrix< short int, 5, 4 >; using Matrix1us = Matrix< unsigned short int, 1, 1 >; using Matrix2us = Matrix< unsigned short int, 2, 2 >; using Matrix3us = Matrix< unsigned short int, 3, 3 >; using Matrix4us = Matrix< unsigned short int, 4, 4 >; using Matrix5us = Matrix< unsigned short int, 5, 5 >; using Matrix2x3us = Matrix< unsigned short int, 2, 3 >; using Matrix3x2us = Matrix< unsigned short int, 3, 2 >; using Matrix2x4us = Matrix< unsigned short int, 2, 4 >; using Matrix4x2us = Matrix< unsigned short int, 4, 2 >; using Matrix2x5us = Matrix< unsigned short int, 2, 5 >; using Matrix5x2us = Matrix< unsigned short int, 5, 2 >; using Matrix3x4us = Matrix< unsigned short int, 3, 4 >; using Matrix4x3us = Matrix< unsigned short int, 4, 3 >; using Matrix3x5us = Matrix< unsigned short int, 3, 5 >; using Matrix5x3us = Matrix< unsigned short int, 5, 3 >; using Matrix4x5us = Matrix< unsigned short int, 4, 5 >; using Matrix5x4us = Matrix< unsigned short int, 5, 4 >; using Matrix1ss = Matrix< signed short int, 1, 1 >; using Matrix2ss = Matrix< signed short int, 2, 2 >; using Matrix3ss = Matrix< signed short int, 3, 3 >; using Matrix4ss = Matrix< signed short int, 4, 4 >; using Matrix5ss = Matrix< signed short int, 5, 5 >; using Matrix2x3ss = Matrix< signed short int, 2, 3 >; using Matrix3x2ss = Matrix< signed short int, 3, 2 >; using Matrix2x4ss = Matrix< signed short int, 2, 4 >; using Matrix4x2ss = Matrix< signed short int, 4, 2 >; using Matrix2x5ss = Matrix< signed short int, 2, 5 >; using Matrix5x2ss = Matrix< signed short int, 5, 2 >; using Matrix3x4ss = Matrix< signed short int, 3, 4 >; using Matrix4x3ss = Matrix< signed short int, 4, 3 >; using Matrix3x5ss = Matrix< signed short int, 3, 5 >; using Matrix5x3ss = Matrix< signed short int, 5, 3 >; using Matrix4x5ss = Matrix< signed short int, 4, 5 >; using Matrix5x4ss = Matrix< signed short int, 5, 4 >; using Matrix1l = Matrix< long int, 1, 1 >; using Matrix2l = Matrix< long int, 2, 2 >; using Matrix3l = Matrix< long int, 3, 3 >; using Matrix4l = Matrix< long int, 4, 4 >; using Matrix5l = Matrix< long int, 5, 5 >; using Matrix2x3l = Matrix< long int, 2, 3 >; using Matrix3x2l = Matrix< long int, 3, 2 >; using Matrix2x4l = Matrix< long int, 2, 4 >; using Matrix4x2l = Matrix< long int, 4, 2 >; using Matrix2x5l = Matrix< long int, 2, 5 >; using Matrix5x2l = Matrix< long int, 5, 2 >; using Matrix3x4l = Matrix< long int, 3, 4 >; using Matrix4x3l = Matrix< long int, 4, 3 >; using Matrix3x5l = Matrix< long int, 3, 5 >; using Matrix5x3l = Matrix< long int, 5, 3 >; using Matrix4x5l = Matrix< long int, 4, 5 >; using Matrix5x4l = Matrix< long int, 5, 4 >; using Matrix1ul = Matrix< unsigned long int, 1, 1 >; using Matrix2ul = Matrix< unsigned long int, 2, 2 >; using Matrix3ul = Matrix< unsigned long int, 3, 3 >; using Matrix4ul = Matrix< unsigned long int, 4, 4 >; using Matrix5ul = Matrix< unsigned long int, 5, 5 >; using Matrix2x3ul = Matrix< unsigned long int, 2, 3 >; using Matrix3x2ul = Matrix< unsigned long int, 3, 2 >; using Matrix2x4ul = Matrix< unsigned long int, 2, 4 >; using Matrix4x2ul = Matrix< unsigned long int, 4, 2 >; using Matrix2x5ul = Matrix< unsigned long int, 2, 5 >; using Matrix5x2ul = Matrix< unsigned long int, 5, 2 >; using Matrix3x4ul = Matrix< unsigned long int, 3, 4 >; using Matrix4x3ul = Matrix< unsigned long int, 4, 3 >; using Matrix3x5ul = Matrix< unsigned long int, 3, 5 >; using Matrix5x3ul = Matrix< unsigned long int, 5, 3 >; using Matrix4x5ul = Matrix< unsigned long int, 4, 5 >; using Matrix5x4ul = Matrix< unsigned long int, 5, 4 >; using Matrix1sl = Matrix< signed long int, 1, 1 >; using Matrix2sl = Matrix< signed long int, 2, 2 >; using Matrix3sl = Matrix< signed long int, 3, 3 >; using Matrix4sl = Matrix< signed long int, 4, 4 >; using Matrix5sl = Matrix< signed long int, 5, 5 >; using Matrix2x3sl = Matrix< signed long int, 2, 3 >; using Matrix3x2sl = Matrix< signed long int, 3, 2 >; using Matrix2x4sl = Matrix< signed long int, 2, 4 >; using Matrix4x2sl = Matrix< signed long int, 4, 2 >; using Matrix2x5sl = Matrix< signed long int, 2, 5 >; using Matrix5x2sl = Matrix< signed long int, 5, 2 >; using Matrix3x4sl = Matrix< signed long int, 3, 4 >; using Matrix4x3sl = Matrix< signed long int, 4, 3 >; using Matrix3x5sl = Matrix< signed long int, 3, 5 >; using Matrix5x3sl = Matrix< signed long int, 5, 3 >; using Matrix4x5sl = Matrix< signed long int, 4, 5 >; using Matrix5x4sl = Matrix< signed long int, 5, 4 >; using Matrix1ll = Matrix< long long int, 1, 1 >; using Matrix2ll = Matrix< long long int, 2, 2 >; using Matrix3ll = Matrix< long long int, 3, 3 >; using Matrix4ll = Matrix< long long int, 4, 4 >; using Matrix5ll = Matrix< long long int, 5, 5 >; using Matrix2x3ll = Matrix< long long int, 2, 3 >; using Matrix3x2ll = Matrix< long long int, 3, 2 >; using Matrix2x4ll = Matrix< long long int, 2, 4 >; using Matrix4x2ll = Matrix< long long int, 4, 2 >; using Matrix2x5ll = Matrix< long long int, 2, 5 >; using Matrix5x2ll = Matrix< long long int, 5, 2 >; using Matrix3x4ll = Matrix< long long int, 3, 4 >; using Matrix4x3ll = Matrix< long long int, 4, 3 >; using Matrix3x5ll = Matrix< long long int, 3, 5 >; using Matrix5x3ll = Matrix< long long int, 5, 3 >; using Matrix4x5ll = Matrix< long long int, 4, 5 >; using Matrix5x4ll = Matrix< long long int, 5, 4 >; using Matrix1ull = Matrix< unsigned long long int, 1, 1 >; using Matrix2ull = Matrix< unsigned long long int, 2, 2 >; using Matrix3ull = Matrix< unsigned long long int, 3, 3 >; using Matrix4ull = Matrix< unsigned long long int, 4, 4 >; using Matrix5ull = Matrix< unsigned long long int, 5, 5 >; using Matrix2x3ull = Matrix< unsigned long long int, 2, 3 >; using Matrix3x2ull = Matrix< unsigned long long int, 3, 2 >; using Matrix2x4ull = Matrix< unsigned long long int, 2, 4 >; using Matrix4x2ull = Matrix< unsigned long long int, 4, 2 >; using Matrix2x5ull = Matrix< unsigned long long int, 2, 5 >; using Matrix5x2ull = Matrix< unsigned long long int, 5, 2 >; using Matrix3x4ull = Matrix< unsigned long long int, 3, 4 >; using Matrix4x3ull = Matrix< unsigned long long int, 4, 3 >; using Matrix3x5ull = Matrix< unsigned long long int, 3, 5 >; using Matrix5x3ull = Matrix< unsigned long long int, 5, 3 >; using Matrix4x5ull = Matrix< unsigned long long int, 4, 5 >; using Matrix5x4ull = Matrix< unsigned long long int, 5, 4 >; using Matrix1sll = Matrix< signed long long int, 1, 1 >; using Matrix2sll = Matrix< signed long long int, 2, 2 >; using Matrix3sll = Matrix< signed long long int, 3, 3 >; using Matrix4sll = Matrix< signed long long int, 4, 4 >; using Matrix5sll = Matrix< signed long long int, 5, 5 >; using Matrix2x3sll = Matrix< signed long long int, 2, 3 >; using Matrix3x2sll = Matrix< signed long long int, 3, 2 >; using Matrix2x4sll = Matrix< signed long long int, 2, 4 >; using Matrix4x2sll = Matrix< signed long long int, 4, 2 >; using Matrix2x5sll = Matrix< signed long long int, 2, 5 >; using Matrix5x2sll = Matrix< signed long long int, 5, 2 >; using Matrix3x4sll = Matrix< signed long long int, 3, 4 >; using Matrix4x3sll = Matrix< signed long long int, 4, 3 >; using Matrix3x5sll = Matrix< signed long long int, 3, 5 >; using Matrix5x3sll = Matrix< signed long long int, 5, 3 >; using Matrix4x5sll = Matrix< signed long long int, 4, 5 >; using Matrix5x4sll = Matrix< signed long long int, 5, 4 >; using Matrix1c = Matrix< char, 1, 1 >; using Matrix2c = Matrix< char, 2, 2 >; using Matrix3c = Matrix< char, 3, 3 >; using Matrix4c = Matrix< char, 4, 4 >; using Matrix5c = Matrix< char, 5, 5 >; using Matrix2x3c = Matrix< char, 2, 3 >; using Matrix3x2c = Matrix< char, 3, 2 >; using Matrix2x4c = Matrix< char, 2, 4 >; using Matrix4x2c = Matrix< char, 4, 2 >; using Matrix2x5c = Matrix< char, 2, 5 >; using Matrix5x2c = Matrix< char, 5, 2 >; using Matrix3x4c = Matrix< char, 3, 4 >; using Matrix4x3c = Matrix< char, 4, 3 >; using Matrix3x5c = Matrix< char, 3, 5 >; using Matrix5x3c = Matrix< char, 5, 3 >; using Matrix4x5c = Matrix< char, 4, 5 >; using Matrix5x4c = Matrix< char, 5, 4 >; using Matrix1uc = Matrix< unsigned char, 1, 1 >; using Matrix2uc = Matrix< unsigned char, 2, 2 >; using Matrix3uc = Matrix< unsigned char, 3, 3 >; using Matrix4uc = Matrix< unsigned char, 4, 4 >; using Matrix5uc = Matrix< unsigned char, 5, 5 >; using Matrix2x3uc = Matrix< unsigned char, 2, 3 >; using Matrix3x2uc = Matrix< unsigned char, 3, 2 >; using Matrix2x4uc = Matrix< unsigned char, 2, 4 >; using Matrix4x2uc = Matrix< unsigned char, 4, 2 >; using Matrix2x5uc = Matrix< unsigned char, 2, 5 >; using Matrix5x2uc = Matrix< unsigned char, 5, 2 >; using Matrix3x4uc = Matrix< unsigned char, 3, 4 >; using Matrix4x3uc = Matrix< unsigned char, 4, 3 >; using Matrix3x5uc = Matrix< unsigned char, 3, 5 >; using Matrix5x3uc = Matrix< unsigned char, 5, 3 >; using Matrix4x5uc = Matrix< unsigned char, 4, 5 >; using Matrix5x4uc = Matrix< unsigned char, 5, 4 >; using Matrix1sc = Matrix< signed char, 1, 1 >; using Matrix2sc = Matrix< signed char, 2, 2 >; using Matrix3sc = Matrix< signed char, 3, 3 >; using Matrix4sc = Matrix< signed char, 4, 4 >; using Matrix5sc = Matrix< signed char, 5, 5 >; using Matrix2x3sc = Matrix< signed char, 2, 3 >; using Matrix3x2sc = Matrix< signed char, 3, 2 >; using Matrix2x4sc = Matrix< signed char, 2, 4 >; using Matrix4x2sc = Matrix< signed char, 4, 2 >; using Matrix2x5sc = Matrix< signed char, 2, 5 >; using Matrix5x2sc = Matrix< signed char, 5, 2 >; using Matrix3x4sc = Matrix< signed char, 3, 4 >; using Matrix4x3sc = Matrix< signed char, 4, 3 >; using Matrix3x5sc = Matrix< signed char, 3, 5 >; using Matrix5x3sc = Matrix< signed char, 5, 3 >; using Matrix4x5sc = Matrix< signed char, 4, 5 >; using Matrix5x4sc = Matrix< signed char, 5, 4 >;

}//mc

#endif