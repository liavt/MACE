/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <array>
#include <MC-System/Exceptions.h>
#include <MC-System/Constants.h>
#include <iosfwd>

namespace mc {
	//forward defining Matrix here for friend declaration in Vector
	template<typename T, Size W, Size H>
	struct Matrix;

	/**
	1-dimensional vector class that supports mathmatical operations.
	<p>
	`Vectors` can be added, subtracted, multiplied, and divided by other `Vectors` of equal width. Additionally, they can also be operated by a `Matrix` of equal width.
	<p>
	`Vector` math is done by adding the adjacent values of both vectors together.
	For example, we want to add these 2 `Vectors` together:
	{@code
	left = [55,42,-12,23]

	right = [3,7,5,9]

	result = left + right
	}
	The result would be every value across from eachother added together, as so:
	{@code
	result = [left[1]+right[1],left[2]+right[2],left[3]+right[3],left[4]+right[4]]
	}
	or
	{@code
	result = [58,49,-7,32]
	}
	Multiplication, division, and subtraction are the same concept. To do math with MACE, all you need to do is to use the mathmatical operators.
	<p>
	Examples:
	{@code
		Vector<int,3> vector = Vector<int,3>();//Create a Vector of 3 ints

		int array[] = {1,2,3};
		vector = array;//Generate Vector from array

		vector.get(i);//Get int from position i
		vector[i];//get int from position i

		vector.set(i,v);//Set int at position i to equal v
		vector[i]=v;//set int at position i to equal v

		vector.size() //Get how many elements the Vector has

		//Iterate through a Vector:
		for(Index i =0;i<vector.size();i++){
			int value = vector[i];
		}
	}
	@see Matrix
	@tparam T what the `Vector` is made of and calculates with.
	@tparam N width of the `Vector`
	*/
	template <typename T,Size N>
	struct Vector {
		/**
		`Matrix` is friends with `Vector` so it can create efficient implementations of `get(Index,Index)` and `set(Index,Index,T)`
		*/
		template<typename,Size,Size>
		friend struct Matrix;
	public:
		/**
		Default constructor. Constructs an empty `Vector`
		*/
		Vector()
		{
			this->setContents(std::array<T, N>());
		};

		/**
		Consructs a `Vector` from the contents of an array.
		@param arr An equally-sized array whose contents will be filled into a `Vector`
		*/
		Vector(T arr[N]) {
			this->setContents(std::array<T,N>());//we need to initialize the array first, or else we will try to access an empty memory location
			this->setContents(arr);//this doesnt create a brand new std::array, it merely fills the existing one with new content
		}

		/**
		Consructs a `Vector` from the contents of an `std::array`.
		@param contents An equally-sized `std::array` whose contents will be filled into a `Vector`
		*/
		Vector(std::array<T, N>& contents)
		{
			this->setContents(contents);
		};
		
		/**
		Copies the contents of a `Vector` into a new `Vector`
		@param obj A `Vector` to clone
		*/
		Vector(const Vector &obj)
		{
			this->setContents(obj.getContents());
		};

		/**
		Retrieves the contents of this `Vector`
		@return An `std::array` of this `Vector` contents
		@see setContents(std::array<T,N>)
		*/
		std::array < T, N>& getContents()
		{
			return this->content;
		};

		/**
		`const` version of `getContents()`
		@return A `const std::array` of this `const Vector` contents
		@see setContents(std::array<T,N>)
		*/
		const std::array < T, N>& getContents() const
		{
			return this->content;
		};
		/**
		Copies the contents of an `std::array` into this `Vector`
		@param contents An `std::array` whose data will be dumped into this `Vector`
		*/
		void setContents(const std::array<T, N> contents)
		{
			this->content = contents;
		};
		/**
		Copies the contents of an array into this `Vector`
		@param arr An equally sized array whose contents will cloned in this `Vector`
		*/
		void setContents(const T arr[N]) {
			for (Index i = 0; i < N; i++) {
				set(i, arr[i]);
			}
		};

		/**
		Retrieves how many elements this `Vector` holds
		@return How large this `Vector` is
		*/
		virtual Size size() const
		{
			return N;
		};


		/**
		Get the value at a position
		@param i `Index` of the requested data
		@return The value located at `i`
		@throw IndexOutOfBounds If `i` is greater than `size()`
		@throw IndexOutOfBounds If `i` is less than 0
		@see operator[](Index)
		*/
		T& get(Index i){
			if (i >= N)throw IndexOutOfBounds(std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			if (i < 0)throw IndexOutOfBounds(std::to_string(i) + " is less than 0!");
			return content[i];
		}
		/**
		`const` version of `get(Index),` in case a `Vector` is declared `const`
		@param i `Index` of the requested data
		@return The `const` value located at `i`
		@throw IndexOutOfBounds If `i` is greater than `size()`
		@throw IndexOutOfBounds If `i` is less than 0
		@see operator[](Index)
		*/
		const T& get(Index i) const{
			if (i >= N)throw IndexOutOfBounds(std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			if (i <0)throw IndexOutOfBounds(std::to_string(i) + " is less than 0!");
			return content.at(i);
		}
		/**
		Set data at a certain position to equal a new value
		@param position Where to put the new value
		@param value What to put in `position`
		@throw IndexOutOfBounds If `i` is greater than `size()`
		@throw IndexOutOfBounds If `i` is less than 0
		@see operator[](Index)
		*/
		void set(Index position, T value) {
			if (position >= N)throw IndexOutOfBounds(std::to_string(position) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			if (position <0)throw IndexOutOfBounds(std::to_string(position) + " is less than 0!");
			content[position] = value;
		}
		/**
		Retrieves the content at a certain `Index.` This operator is faster than `get(Index),` as it doesn't do bounds checking. However, accessing an invalid index will cause a memory exception.
		@param i Where to retrieve the data
		@return The data at `i`
		@see operator[](Index) const
		*/
		T& operator[](Index i)
		{
			return content[i];
		};
		/**
		`const` version of `operator[](Index)` used if a `Vector` is declared `const`
		@param i Where to retrieve the data
		@return The data at `i`
		@see operator[](Index)
		*/
		const T& operator[](Index i) const
		{
			return content[i];
		};
		/**
		Adds 2 `Vectors` together.
		
		@param right Another `Vector`
		@return A `Vector` that was created by adding 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Vector operator+(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i]=((T)(this->get(i) + right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};
		/**
		Subtracts 2 `Vectors` together.

		@param right Another `Vector`
		@return A `Vector` that was created by subtracting 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Vector operator-(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i] = ((T)(this->get(i) - right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};
		/**
		Divides 2 `Vectors` together.

		@param right Another `Vector`
		@return A `Vector` that was created by dividing 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Vector operator/(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i] = ((T)(this->get(i) / right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};
		/**
		Multiplies 2 `Vectors` together.

		@param right Another `Vector`
		@return A `Vector` that was created by multiplying 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Vector operator*(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for (unsigned int i = 0; i < N; i++) {
				out[i] = ((T)(this->get(i) * right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};

		void += (const Vector<T, N> right) {
			this = this + right;
		}

		void -= (const Vector<T, N> right) {
			this = this - right;
		}

		void *= (const Vector<T, N> right) {
			this = this * right;
		}

		void /= (const Vector<T, N> right) {
			this = this / right;
		}

		/**
		Compares whether 2 `Vectors` have the same values
		@param other A `Vector` to compare `this` against
		@return `true` if the 2 are equal, `false` otherwise
		@see operator!=(const Vector<T,N>) const
		*/
		bool operator==(const Vector<T,N>& other) const
		{
			for (Index i = 0; i < N; i++) {
				if (this[i] != other[i]) {
					return false;
				}
			}
			return true;
		};

		/**
		Compares whether 2 `Vectors` don't have the same values
		@param other A `Vector` to compare `this` against
		@return `true` if the 2 are not equal, `false` otherwise
		@see operator==(const Vector<T,N>) const
		*/
		bool operator!=(const Vector<T,N>& other) const
		{
			return !(this == other);
		};

		/**
		Operator used to output to `std::cout`.
		@param output `std::ostream` the `Matrix` was inserted into
		@param m `Matrix` which will be printed
		@return `output` for chaining
		*/
		friend std::ostream &operator<<(std::ostream &output,
			const Vector<T, N> &v) {
			output << "[ ";
			for (Index x = 0; x < N; x++) {
				output << v[x]<<", ";
			}
			output << " ]";
			return output;
		}

	private:
		std::array<T,N> content;
	};


	/**
	A `Vector` consisting of 1 `float`
	*/
	using Vector1f = mc::Vector<float, 1>;
	/**
	A `Vector` consisting of 2 `floats`
	*/
	using Vector2f = mc::Vector<float, 2>;
	/**
	A `Vector` consisting of 3 `floats`
	*/
	using Vector3f = mc::Vector<float, 3>;
	/**
	A `Vector` consisting of 4 `floats`
	*/
	using Vector4f = mc::Vector<float, 4>;
	/**
	A `Vector` consisting of 5 `floats`
	*/
	using Vector5f = mc::Vector<float, 5>;

	/**
	A `Vector` consisting of 1 `int`
	*/
	using Vector1i = mc::Vector<int, 1>;
	/**
	A `Vector` consisting of 2 `ints`
	*/
	using Vector2i = mc::Vector<int, 2>;
	/**
	A `Vector` consisting of 3 `ints`
	*/
	using Vector3i = mc::Vector<int, 3>;
	/**
	A `Vector` consisting of 4 `ints`
	*/
	using Vector4i = mc::Vector<int, 4>;
	/**
	A `Vector` consisting of 5 `ints`
	*/
	using Vector5i = mc::Vector<int, 5>;

	/**
	Used in the `Matrix` class. Defined for clarity for when you iterate over a `Matrix.`
	@see Matrix
	@tparam T What data type the row is made of
	@tparam N The width of the row
	*/
	template <typename T, Size N>
	using MatrixRow = mc::Vector<T, N>;//this is for clarity

	/**
	A class representing a 2-dimensional matrix, and allows for math involving matrices. A `Matrix` can be known as a `Vector` of `Vectors`. 
	<p>
	`Matrices` can be added, subtracted, multiplied, and divided by eachother, and by `Vectors` of equal width.
	<p>
	`Matrix` math is similar to `Vector` math. Lets say we have this:
	{@code
	Matrix a = [
		a1, b1, c1
		a2, b2, c2
		a3, b3, c3
	]
	}
	and
	{@code
	Matrix b = [
		d1,e1,f1
		d2,e2,f2
		d3,e3,f3
	]
	}
	The output `Matrix`, when multiplied, will look like this:
	{@code
	a * b = [
		a1+*1, b1*e1, c1*f1
		a2*d2, b2*e2, c2*f2
		a3*d3, b3*e3, c3*f3
	]
	}
	<p>
	A `Matrix` can also be multiplied by a `Vector` who has the same height.
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
	template<typename T,Size W, Size H>
	struct Matrix : public Vector<MatrixRow<T, H>, W> {
		using Vector<MatrixRow<T, H>, W>::content;

		/**
		Default constructor. Creates a `Matrix` of the specified size where every spot is empty.
		*/
		Matrix()
		{
			this->setContents(std::array<MatrixRow<T, H>, W>());
			for (Index i = 0; i < content.size(); i++) {
				content[i] = MatrixRow<T, H>();
			}
		};

		/**
		Creates a `Matrix` based on a 2-dimensional array. The array's contents will be copied into this `Matrix`
		@param arr An array of contents
		*/
		Matrix(T arr[W][H]) {
			Matrix();
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
					content[x][y] = arr[x][y];
				}
			}
		}

		/**
		Copy constructor. Clones the contents of another `Matrix` into a new `Matrix`
		@param copy What the clone
		*/
		Matrix(const Matrix& copy) {
			this->setContents(copy.getContents());
		}

		/**
		Calculates how many elements are in this `Matrix`
		@return `width()` times `height()`
		@see #height()
		@see #width()
		*/
		Size size() {
			return W*H;
		}

		/**
		Calculates the width of this `Matrix`.
		@return The width specified by the template.
		@see #height()
		@see #size()
		*/
		Size width() {
			return W;
		}
		/**
		Calculates the height of this `Matrix`.
		@return The height specified by the template.
		@see #width()
		@see #size()
		*/
		Size height() {
			return H;
		}

		/**
		Retrieves the content at a certain position
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the data at `X,Y`
		@throw IndexOutOfBounds if `x<0`
		@throw IndexOutOfBounds if `y<0`
		@throw IndexOutOfBounds if `x>width()`
		@throw IndexOutOfBounds if `y>height()`
		@see #set(Index, Index, T)
		@see #operator[]
		*/
		T& get(Index x, Index y) {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x < 0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y < 0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			return content[x][y];
		}

		/**
		`const` version of {@link #get(Index,Index)}
		@param x X-coordinate of the requested data
		@param y Y-coordinate of the requested data
		@return A reference to the `const` data at `X,Y`
		@throw IndexOutOfBounds if `x<0`
		@throw IndexOutOfBounds if `y<0`
		@throw IndexOutOfBounds if `x>width()`
		@throw IndexOutOfBounds if `y>height()`
		@see #set(Index, Index, T)
		@see #operator[]
		*/
		const T& get(Index x, Index y) const {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x < 0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y < 0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			return content[x][y];
		}

		/**
		Writes data at certain coordinates to a new value.
		@param x X-coordinate of the new data
		@param y Y-coordinate of the new data
		@param value New datat to write to `X,Y`
		@throw IndexOutOfBounds if `x<0`
		@throw IndexOutOfBounds if `y<0`
		@throw IndexOutOfBounds if `x>width()`
		@throw IndexOutOfBounds if `y>height()`
		@see #operator[]
		@see #get(Index, Index)
		*/
		void set(Index x, Index y, T value) {
			if (x >= W) throw IndexOutOfBounds(std::to_string(x) + " is greater than this Matrix's width, " + std::to_string(W) + "!");
			if (y >= H) throw IndexOutOfBounds(std::to_string(y) + " is greater than this Matrix's width, " + std::to_string(H) + "!");
			if (x < 0)throw IndexOutOfBounds("The X value, " + std::to_string(x) + " is less than 0!");
			if (y < 0)throw IndexOutOfBounds("The Y value, " + std::to_string(x) + " is less than 0!");
			content[x][y] = value;
		}


		/**
		Adds a `Vector` and a `Matrix` together

		@param right A `Vector` of equal width
		@return A `Vector` that was created by adding a `Vector` and a `Matrix` together
		@throw ArithmeticError If `width()` is greater than `height()`
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		*/
		Vector<T, H> operator+(const Vector<T, h>& right) const {
			if (W > H)throw ArithmeticError("When adding a matrix by a vector, the width of the matrix (" + std::to_string(W) + ") cannot be larger than the height (" + std::to_string(H) + "!)");
			T arr[H];
			for (Index x = 0; x < W; x++) {
				arr[x] = 0;//we must initialize the value first, or else it will be undefined
				for (Index y = 0; y < H; y++) {
					arr[y] += static_cast<T>(content[x][y]) + static_cast<T>(right[x]);
				}
			}
			return arr;
		};

		/**
		Subtracts a `Vector` and a `Matrix` together

		@param right A `Vector` of equal width
		@return A `Vector` that was created by subtracting a `Vector` and a `Matrix` together
		@throw ArithmeticError If `width()` is greater than `height()`
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		*/
		Vector<T, H> operator-(const Vector<T, H>& right) const {
			if (W > H)throw ArithmeticError("When subtracting a matrix by a vector, the width of the matrix (" + std::to_string(W) + ") cannot be larger than the height (" + std::to_string(H) + "!)");
			T arr[H];
			for (Index x = 0; x < W; x++) {
				arr[x] = 0;//we must initialize the value first, or else it will be undefined
				for (Index y = 0; y < H; y++) {
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
			if (W > H)throw ArithmeticError("When multiplying a matrix by a vector, the width of the matrix ("+std::to_string(W)+") cannot be larger than the height ("+std::to_string(H)+"!)");
			T arr[H];
			for (Index x = 0; x < W; x++) {
				arr[x] = 0;//we must initialize the value first, or else it will be undefined
				for (Index y = 0; y < H; y++) {
					arr[y] += static_cast<T>(content[x][y]) * static_cast<T>(right[x]);
				}
			}
			return arr;
		};

		/**
		Divides a `Vector` and a `Matrix` together

		@param right A `Vector` of equal width
		@return A `Vector` that was created by dividing a `Vector` and a `Matrix` together
		@throw ArithmeticError If `width()` is greater than `height()`
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		*/
		Vector<T,H> operator/(const Vector<T, H>& right) const {
			if (W > H)throw ArithmeticError("When dividing a matrix by a vector, the width of the matrix (" + std::to_string(W) + ") cannot be larger than the height (" + std::to_string(H) + "!)");
			T arr[H];
			for (Index x = 0; x < W; x++) {
				arr[x] = 0;//we must initialize the value first, or else it will be undefined
				for (Index y = 0; y < H; y++) {
					arr[y] += static_cast<T>(content[x][y]) / static_cast<T>(right[x]);
				}
			}
			return arr;
		};

		/**
		Adds 2 `Matrix` together.
		@param right A `Matrix` to add
		@return A `Matrix` whose contents is 2 `Matrix` added together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		Matrix<T, W, H> operator+(const Matrix<T, W, H>& right) const {
			T arr[W][H];
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
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
			T arr[W][H];
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
					arr[x][y] = content[x][y] - right[x][y];
				}
			}
			return arr;
		}

		/**
		Multiplies 2 `Matrix` together.
		@param right A `Matrix` to multiply
		@return A `Matrix` whose contents is 2 `Matrix` multiplied together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		Matrix<T, W, H> operator*(const Matrix<T, W, H>& right) const {
			T arr[W][H];
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
					arr[x][y] = content[x][y] * right[x][y];
				}
			}
			return arr;
		}

		/**
		Divides 2 `Matrix` together.
		@param right A `Matrix` to divide
		@return A `Matrix` whose contents is 2 `Matrix` divide together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		Matrix<T, W, H> operator/(const Matrix<T, W, H>& right) const {
			T arr[W][H];
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
					arr[x][y] = content[x][y] / right[x][y];
				}
			}
			return arr;
		}

		void operator+=(const Matrix<T, W, H>& right) {
			this = this + right;
		}

		void operator-=(const Matrix<T, W, H>& right) {
			this = this - right;
		}

		void operator*=(const Matrix<T, W, H>& right) {
			this = this * right;
		}

		void operator/=(const Matrix<T, W, H>& right) {
			this = this / right;
		}

		/**
		Operator used to output to `std::cout`.
		<p>
		The output will take up multiple lines and will show all of the `Matrix` contents
		@param output `std::ostream` the `Matrix` was inserted into
		@param m `Matrix` which will be printed
		@return `output` for chaining
		*/
		friend std::ostream &operator<<(std::ostream &output,
			const Matrix<T,W,H> &m) {
			output << W <<" x " <<H<<" Matrix: [";
			for (Index x = 0; x < W; x++) {
				output << std::endl<<"  ";
				for (Index y = 0; y < H; y++) {
					output << m[y][x]<<", ";
				}
			}
			output << " ]";
			return output;
		}



	};

	/**
	A `1x1` `Matrix` of `floats`.
	*/
	using Matrix1f = mc::Matrix<float, 1, 1>;//what a thin matrix
	/**
	A `2x2` `Matrix` of `floats`.
	*/
	using Matrix2f = mc::Matrix<float, 2, 2>;
	/**
	A `3x3` `Matrix` of `floats`.
	*/
	using Matrix3f = mc::Matrix<float, 3, 3>;
	/**
	A `4x4` `Matrix` of `floats`.
	*/
	using Matrix4f = mc::Matrix<float, 4, 4>;
	/**
	A `5x5` `Matrix` of `floats`.
	*/
	using Matrix5f = mc::Matrix<float, 5, 5>;

	/**
	A `1x1` `Matrix` of `ints`.
	*/
	using Matrix1i = mc::Matrix<int, 1, 1>;
	/**
	A `2x2` `Matrix` of `ints`.
	*/
	using Matrix2i = mc::Matrix<int, 2, 2>;
	/**
	A `3x3` `Matrix` of `ints`.
	*/
	using Matrix3i = mc::Matrix<int, 3, 3>;
	/**
	A `4x4` `Matrix` of `ints`.
	*/
	using Matrix4i = mc::Matrix<int, 4, 4>;
	/**
	A `5x5` `Matrix` of `ints`.
	*/
	using Matrix5i = mc::Matrix<int, 5, 5>;
}