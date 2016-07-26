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
#include <initializer_list>
#include <MC-System/Utility/Math.h>

namespace mc {
	//forward defining Matrix here for friend declaration in Vector
	template<typename T, Size W, Size H>
	struct Matrix;

	/**
	1-dimensional vector class that supports mathmatical operations.
	<p>
	`Vectors` can be added, subtracted, and multiplied. by other `Vectors` of equal width. Additionally, they can also be operated by a `Matrix` of equal width.
	`Vectors` CANNOT be divided.
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
	Multiplication, and subtraction are the same concept. To do math with MACE, all you need to do is to use the mathmatical operators.
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
			static_assert(N != 0, "A Vector's size must be greater than 0!");
			this->setContents(std::array<T, N>());
		};

		/**
		Consructs a `Vector` from the contents of an array.
		@param arr An equally-sized array whose contents will be filled into a `Vector`
		*/
		Vector(const T arr[N]) {
			static_assert(N != 0, "A Vector's size must be greater than 0!");
			this->setContents(std::array<T,N>());//we need to initialize the array first, or else we will try to access an empty memory location
			this->setContents(arr);//this doesnt create a brand new std::array, it merely fills the existing one with new content
		}

		/**
		Consructs a `Vector` from the contents of an `std::array`.
		@param contents An equally-sized `std::array` whose contents will be filled into a `Vector`
		*/
		Vector(const std::array<T, N>& contents)
		{
			static_assert(N != 0, "A Vector's size must be greater than 0!");
			this->setContents(contents);
		};

		Vector(const std::initializer_list<T> args) {//this is for aggregate initializaition
			static_assert(N != 0, "A Vector's size must be greater than 0!");
			if (args.size() != N)throw IndexOutOfBounds("The number of arguments MUST be equal to the size of the array.");
			this->setContents(std::array<T, N>());
			Index counter = 0;
			for (auto elem : args) {
				content[counter] = elem;
				counter++;
			}
		}
		
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
		Get the value at a position. Slower than `operator[]` because it does bounds checking.
		@param i `Index` of the requested data, zero-indexed
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
		@param i `Index` of the requested data, zero-indexed
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
		Set data at a certain position to equal a new value. Slower than `operator[]` because it does bounds checking.
		@param position Where to put the new value, zero indexed.
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
		Retrieves the content at a certain `Index`, zero indexed. This operator is faster than `get(Index),` as it doesn't do bounds checking. However, accessing an invalid index will be undefined.
		@param i Where to retrieve the data
		@return The data at `i`
		@see operator[](Index) const
		*/
		T& operator[](Index i)
		{
			return content[i];
		};
		/**
		`const` version of `operator[](Index)` used if a `Vector` is declared `const`.
		@param i Where to retrieve the data
		@return The data at `i`
		@see operator[](Index)
		*/
		const T& operator[](Index i) const
		{
			return content[i];
		};

		/**
		Retrieves content at a certain `Index`, not zero indexed. 
		<p>
		Equal to {@code
			vector[i-1]
		}
		@param i Not zero indexed `Index`
		@return Value at `i-1`
		@see operator[](Index)
		*/
		virtual T& operator()(Index i) {
			return content[i - 1];
		}

		/**
		`const` version of `operator()(Index)`.
		@param i Not zero indexed `Index`
		@return Value at `i-1`
		*/
		const T& operator()(Index i) const {
			return content[i - 1];
		}

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
		Calculates the cross product of 2 `Vectors`

		@param right Another `Vector`
		@return The cross product
		@see Vector for an explanation of `Vector` math
		@see cross(const Vector&, const Vector&)
		@see operator*(const T)
		@see operator%(const Vector&)
		*/
		Vector operator*(const Vector<T, 3>& right) const {
			return math::cross(*this,right);
		};

		/**
		Calculates the dot product of 2 `Vectors`

		@param right Another `Vector`
		@return The dot product
		@see Vector for an explanation of `Vector` math
		@see cross(const Vector&, const Vector&)
		@see operator*(const T)
		@see operator*(const Vector&)
		*/
		Vector operator%(const Vector<T, 3>& right) const {
			return math::dot(*this, right);
		}

		/**
		Multiplies a `Vector` by a scalar.
		@param scalar What to multiply this `Vector` by
		@return A `Vector` scaled.
		@see operator*(const Vector&) const
		*/
		Vector operator*(const T scalar) const {
			Vector<T, N> out = *this;
			for (Index i = 0; i < N;i++) {
				out[i] *= scalar;
			}
			return out;
		}

		/**
		Divides a `Vector` by a scalar.
		@param scalar What to divided this `Vector` by
		@return A `Vector` scaled.
		@see operator*(const T&) const
		*/
		Vector operator/(const T scalar) const {
			Vector<T, N> out = *this;
			for (Index i = 0; i < N; i++) {
				out[i] /= scalar;
			}
			return out;
		}

		/**
		Adds a `Vector` to this one.
		@param right A `Vector` to add
		@see operator+(const Vector<T,N>&) const
		*/
		void operator+= (const Vector<T, N>& right) {
			setContents((*this - right).getContents());
		}

		/**
		Subtracts a `Vector` from this one.
		@param right A `Vector` to subtract
		@see operator-(const Vector<T,N>&) const
		*/
		void operator-= (const Vector<T, N>& right) {
			setContents((*this - right).getContents());
		}
		/**
		Scales this `Vector`
		@param scalar How much to scale
		@see operator*(const Vector<T,3>&) const
		@see operator*(const T) const
		*/
		void operator*= (const T& scalar) {
			setContents((*this * scalar).getContents());
		}

		/**
		Compares whether 2 `Vectors` have the same values
		@param other A `Vector` to compare `this` against
		@return `true` if the 2 are equal, `false` otherwise
		@see operator!=(const Vector<T,N>) const
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator>(const Vector&) const
		*/
		bool operator==(const Vector<T,N>& other) const
		{
			for (Index i = 0; i < N; i++) {
				if (this->operator[](i) != other[i]) {
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
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator>(const Vector&) const
		*/
		bool operator!=(const Vector<T,N>& other) const
		{
			return !(*this == other);
		};

		/**
		Compares the `>` operator on 2 `Vectors` elements.
		@param other A `Vector` to compare against
		@return The result of the `>` operator on each element
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator==(const Vector&) const
		@see operator!=(const Vector&) const
		*/
		bool operator>(const Vector<T, N>& other) const {
			for (Index i = 0; i < N; i++) {
				if (this->operator[](i)<=other[i]) {
					return false;
				}
			}
			return true;
		}

		/**
		Compares the `>=` operator on 2 `Vectors` elements.
		@param other A `Vector` to compare against
		@return The result of the `>=` operator on each element
		@see operator<(const Vector&) const
		@see operator>(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator==(const Vector&) const
		@see operator!=(const Vector&) const
		*/
		bool operator>=(const Vector<T, N>& other) const {
			return *this > other || *this == other;
		}

		/**
		Compares the `<` operator on 2 `Vectors` elements.
		@param other A `Vector` to compare against
		@return The result of the `<` operator on each element
		@see operator<=(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator>(const Vector&) const
		@see operator==(const Vector&) const
		@see operator!=(const Vector&) const
		*/
		bool operator<(const Vector<T, N>& other) const {
			return !(*this >= other);
		}

		/**
		Compares the `<=` operator on 2 `Vectors` elements.
		@param other A `Vector` to compare against
		@return The result of the `<=` operator on each element
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator>(const Vector&) const
		@see operator==(const Vector&) const
		@see operator!=(const Vector&) const
		*/
		bool operator<=(const Vector<T, N>& other) const {
			return !(*this>other);
		}

		/**
		Operator used to output to `std::cout`.
		@param output `std::ostream` the `Matrix` was inserted into
		@param v `Matrix` which will be printed
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
	template<typename T,Size W, Size H>
	struct Matrix : public Vector<MatrixRow<T, H>, W> {
		using Vector<MatrixRow<T, H>, W>::content;

		/**
		Default constructor. Creates a `Matrix` of the specified size where every spot is unallocated
		*/
		Matrix()
		{	
			static_assert(W != 0, "A Matrix's width must be greater than 0!");
			static_assert(H != 0, "A Matrix's height must be greater than 0!");
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


		Matrix(const std::initializer_list<const std::initializer_list<T>> args) {//this is for aggregate initializaition
			if (args.size() != W)throw IndexOutOfBounds("The width of the argument must be equal to to the height of the Matrix!");
			Matrix();
			Index counterX = 0, counterY = 0;
			for (std::initializer_list<T> elemX : args) {
				if (elemX.size() != H)throw IndexOutOfBounds("The height of the argument must be equal to to the height of the Matrix!");
				counterY = 0;
				for(T elemY: elemX){
					content[counterX][counterY] = elemY;
					counterY ++;
				}

				counterX++;
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
		virtual T& operator()(Index x,Index y) {
			return content[x - 1][y-1];
		}

		/**
		`const` version of `operator()(Index,Index)`.
		@param x Not zero-indexed X-coordinate
		@param y Not zero-indexed y-coordinate
		@return Value at `x-1` and `y-1`
		@see set(Index, Index,T)
		@see get(Index,Index)
		*/
		const T& operator()(Index x,Index y) const {
			return content[x - 1][y - 1];
		}

		/**
		Adds a `Vector` and a `Matrix` together

		@param right A `Vector` of equal width
		@return A `Vector` that was created by adding a `Vector` and a `Matrix` together
		@throw ArithmeticError If `width()` is greater than `height()`
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		@bug If the vector is of non-numerical value, breaks
		*/
		Vector<T, H> operator+(const Vector<T, H>& right) const {
			static_assert(W <= H, "When doing Matrix by Vector math, the Matrix's width must not be larger than the height.");
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
		Subtracts a `Vector` and a `Matrix` together. The Matrix's `width()` must not be larger than `height()`

		@param right A `Vector` of equal width
		@return A `Vector` that was created by subtracting a `Vector` and a `Matrix` together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Vector` and `Matrix` math
		@bug If the vector is of non-numerical values, breaks
		*/
		Vector<T, H> operator-(const Vector<T, H>& right) const {
			static_assert(W <= H, "When doing Matrix by Vector math, the Matrix's width must not be larger than the height.");
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
		@bug If the vector is of non-numerical values, breaks
		*/
		Vector<T, H> operator*(const Vector<T, H>& right) const {
			static_assert(W <= H, "When doing Matrix by Vector math, the Matrix's width must not be larger than the height.");
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
		Adds 2 `Matrix` together. `width()` MUST be equal to `height()` in order to do `Matrix` math.
		@param right A `Matrix` to add
		@return A `Matrix` whose contents is 2 `Matrix` added together
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		Matrix<T, W, H> operator+(const Matrix<T, W, H>& right) const {
			static_assert(W==H,"In order to do Matrix math, the width and height of both Matrices have to be be equal.");
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
			static_assert(W == H, "In order to do Matrix math, the width and height of both Matrices have to be be equal.");
			T arr[W][H];
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
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
		@bug If the matrix is of non-numerical values, breaks
		*/
		Matrix<T, W, H> operator*(const Matrix<T, W, H>& right) const {
			static_assert(W == H, "In order to multiply matrices, the width must equal to the height.");
			T arr[W][H];
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
					arr[x][y] = 0;
					for (Index x1 = 0; x1 < W; x1++) {
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
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
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
			setContents((*this+right).getContents());
		}

		/**
		Subtracts a `Matrix` from this one.
		@param right A `Matrix` to subtracts
		@see operator-(const Matrix<T,W,H>&) const
		@see Vector for an explanation of `Vector` math
		@see Matrix for an explanation of `Matrix` math
		*/
		void operator-=(const Matrix<T, W, H>& right) {
			setContents((*this-right).getContents());
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
			setContents((*this/right).getContents());
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
			const Matrix<T,W,H> &m) {
			for (Index x = 0; x < W; x++) {
				if (x == 0)output << "[ ";
				else output <<std::endl<< "  ";
				for (Index y = 0; y < H; y++) {
					output << m[x][y];
					if (y < H - 1)output << ", ";
				}
			}
			output << " ]";
			return output;
		}



	};

	namespace math {
		//math calculations

		/**
		Calculates the cross product of 2 `Vectors`. The `Vector` must be 3-dimensional.
		@param a First `Vector`
		@param b Second `Vector`
		@return A vector calculated from the cross product of `a` and `b`
		@see dot(const Vector&, const Vector&)
		@see magnitude(const Vector&)
		@tparam T Type of the `Vectors` being calculated. This does not need to be explicitely set.
		*/
		template<typename T>
		Vector<T,3> cross(const Vector<T, 3>& a, const  Vector<T, 3>& b) {
			//i though of more than one pun for this one, so here is a list:

			//the cross of jesus or the cross of cris? these are important questions
			//i wont double cross you!
			//the bible is a cross product
			//so is a swiss army knife
			//railroad crossing; the train is carying important product
			//i should just cross this off the list of unproductive things i have done

			Vector<T, 3> out = Vector<T, 3>();
			//whew math
			out[0] = (a[1] * b[2]) - (a[2] * b[1]);
			out[1] = (a[2] * b[0]) - (a[0] * b[2]);
			out[2] = (a[0] * b[1]) - (a[1] * b[0]);

			return out;
		};

		/**
		Calculates the dot product of 2 `Vectors`
		@param a First `Vector` to use
		@param b Second `Vector` to use
		@return A scalar calculated from the dot product of `a` and `b`
		@see cross(const Vector&, const Vector&)
		@see magnitude(const Vector&)
		@bug If the vector is of non-numerical value, breaks
		@tparam T Type of the `Vectors` being calculated. This does not need to be explicitely set.
		@tparam N Size of the `Vectors` being calculated. This does not need to be explicitely set.
		*/
		template<typename T, Size N>
		T dot(const Vector<T, N>& a, const Vector<T, N>& b) {
			T out = 0;
			for (Index i = 0; i < N; i++) {
				out += static_cast<T>(a[i] * b[i]);
			}
			return out;
		}

		/**
		Calculates the magnitude of a `Vector`, or how long it is.
		@param a The `Vector` to calculate from
		@return The magnitude of `Vector a`
		@bug If the vector is of non-numerical value, breaks
		@see cross(const Vector&, const Vector&)
		@see dot(const Vector&, const Vector&)
		@tparam T Type of the `Vectors` being calculated. This does not need to be explicitely set.
		@tparam N Size of the `Vectors` being calculated. This does not need to be explicitely set.
		*/
		template<typename T,Size N>
		T magnitude(const Vector<T,N>& a) {
			T out = 0;//assuming its numerical
			//basically the pythagereon theorum
			for (Index i = 0; i < N;i++) {
				out += static_cast<T>(sqr(a[i]));
			}
			return sqrt(out);
		}

		/**
		Normalize a `Vector`. A normalized `Vector` has a length of 1, and is also known as a unit vector.
		@param vector `Vector` to normalize
		@return A unit `Vector` with a norm of `
		@see magnitude(const Vector&)
		@see dot(const Vector&, const Vector&)
		@see cross(const Vector&, const Vector&)
		@tparam T Type of `Vector`
		@tparam N Size of the `Vector`
		*/
		template<typename T, Size N>
		Vector<T, N> normalize(Vector<T, N>& vector) {
			return vector / magnitude(vector);
		}

		//start of matrix functions

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
			for (Index x = 0; x < W; x++) {
				for (Index y = 0; y < H; y++) {
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
			T sum=0;
			Index counter=0;
			for (Index i = 0; i < N; i++) {
				const Size newMatrixSize = N - 1;
				Matrix<T, newMatrixSize, newMatrixSize> m = Matrix<T, newMatrixSize, newMatrixSize>();
				Index counterX = 0;
				Index counterY = 0;
				for (Index x = 0; x < N; x++) {
					for (Index y = 1; y < N; y++) {
						if (x != i) {
							m[counterX][counterY] = matrix[x][y];
							counterX++;
						}
						counterY++;

					}
					counterY = 0;
				}
				T tempSum = ((isEven(counter) ? -1 : 1) * matrix[i][0]);
				if (N-1 == 2) {
					tempSum *= det<T>(matrix);
				}
				else {
					tempSum *= det<T, N>(matrix);
				}
				sum += tempSum;
				counter++;
			}
			return sum;
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
		template<typename T,Size N>
		Matrix<T, N,N> inverse(const Matrix<T,N,N>& matrix) {
			static_assert(N >= 2, "In order to inverse a matrix, it's size must be greater than 1!");
			//honestly, this next line really seems magical to me. matrices really seem magical in general. But
			//this especialy. matrices are really something, aren't they?
			return	(((identity<T,N>()*tr(matrix))-matrix) * (1 / det(matrix)));//calculate via the caley-hamilton method
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
		T tr(const Matrix<T, N,N>& m) {
			T out = 0;
			//done without a trace!
			for (Index x = 0; x < N; x++) {
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
			for (Index x = 0; x < N; x++) {
				for (Index y = 0; y < N; y++) {
					if (x == y) {
						out[x][y] = 1;
					}
					else {
						out[x][y] = 0;
					}
				}
			}
			return out;
		}
	}


	//here be typedefs

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

	//for matrix now

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