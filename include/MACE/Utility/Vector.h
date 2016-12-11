/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_VECTOR_H
#define MACE_UTILITY_VECTOR_H

#include <MACE/System/Constants.h>
#include <MACE/Utility/Math.h>
#include <array>
#include <ostream>
#include <initializer_list>

/*
the fact that vectors are templated maeans that this cant have a cpp file correspondign to it. because of that, this file has HORRIBLE compile times.
maybe i will fix it later
*/

namespace mc {
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
	template <typename T, Size N>
	struct Vector {
	public:
		/**
		Default constructor. Constructs an empty `Vector`
		*/
		Vector() : content() {
			static_assert(N != 0, "A Vector's size must be greater than 0!");
		};

		/**
		Consructs a `Vector` from the contents of an array.
		@param arr An equally-sized array whose contents will be filled into a `Vector`
		*/
		Vector(const T arr[N]) : content()//we need to initialize the array first, or else we will try to access an empty memory location
		{
			static_assert(N != 0, "A Vector's size must be greater than 0!");
			this->setContents(arr);//this doesnt create a brand new std::array, it merely fills the existing one with new content
		}

		/**
		Consructs a `Vector` from the contents of an `std::array`.
		@param contents An equally-sized `std::array` whose contents will be filled into a `Vector`
		*/
		Vector(const std::array<T, N>& contents) : content(contents) {
			static_assert(N != 0, "A Vector's size must be greater than 0!");
		};

		Vector(const std::initializer_list<T> args) : content() {//this is for aggregate initializaition
			static_assert(N != 0, "A Vector's size must be greater than 0!");
			if( args.size() != N )throw IndexOutOfBoundsException("The number of arguments MUST be equal to the size of the array.");
			Index counter = 0;
			for( auto elem : args ) {
				content[counter] = elem;
				++counter;
			}
		}

		/**
		Copies the contents of a `Vector` into a new `Vector`
		@param obj A `Vector` to clone
		*/
		Vector(const Vector &obj) : content(obj.content) {};

		/**
		Retrieves the contents of this `Vector`
		@return An `std::array` of this `Vector` contents
		@see setContents(std::array<T,N>)
		*/
		std::array < T, N>& getContents() {
			return this->content;
		};

		/**
		`const` version of `getContents()`
		@return A `const std::array` of this `const Vector` contents
		@see setContents(std::array<T,N>)
		*/
		const std::array < T, N>& getContents() const {
			return this->content;
		};
		/**
		Copies the contents of an `std::array` into this `Vector`
		@param contents An `std::array` whose data will be dumped into this `Vector`
		*/
		void setContents(const std::array<T, N> contents) {
			this->content = contents;
		};
		/**
		Copies the contents of an array into this `Vector`
		@param arr An equally sized array whose contents will cloned in this `Vector`
		*/
		void setContents(const T arr[N]) {
			for( Index i = 0; i < N; ++i ) {
				set(i, arr[i]);
			}
		};

		/**
		Retrieves how many elements this `Vector` holds
		@return How large this `Vector` is
		*/
		constexpr Size size() const noexcept {
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
		T& get(Index i) {
			if( i >= N )throw IndexOutOfBoundsException(std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
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
		const T& get(Index i) const {
			if( i >= N )throw IndexOutOfBoundsException(std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
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
			if( position >= N )throw IndexOutOfBoundsException(std::to_string(position) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			content[position] = value;
		}

		/**
		Creates an array with the data of this `Vector`, in O(N) time
		@return Pointer to `arr`
		@param arr The array to fill
		*/
		const T* flatten(T arr[N]) const {
			for( Index i = 0; i < N; i++ ) {
				arr[i] = content[i];
			}
			return arr;
		}

		/**
		Retrieves the content at a certain `Index`, zero indexed. This operator is faster than `get(Index),` as it doesn't do bounds checking. However, accessing an invalid index will be undefined.
		@param i Where to retrieve the data
		@return The data at `i`
		@see operator[](Index) const
		*/
		T& operator[](Index i) {
			return content[i];
		};
		/**
		`const` version of `operator[](Index)` used if a `Vector` is declared `const`.
		@param i Where to retrieve the data
		@return The data at `i`
		@see operator[](Index)
		*/
		const T& operator[](Index i) const {
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
		<p>
		This is done in o(N) time

		@param right Another `Vector`
		@return A `Vector` that was created by adding 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Vector operator+(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for( unsigned int i = 0; i < N; i++ ) {
				out[i] = ((T) (this->get(i) + right.get(i)));
			}
			return mc::Vector<T, N>(out);
		};
		/**
		Subtracts 2 `Vectors` together.
		<p>
		This is done in o(N) time

		@param right Another `Vector`
		@return A `Vector` that was created by subtracting 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Vector operator-(const Vector<T, N>& right) const {
			std::array<T, N> out = std::array<T, N>();
			for( unsigned int i = 0; i < N; i++ ) {
				out[i] = ((T) (this->get(i) - right.get(i)));
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
			//i though of more than one pun for this one, so here is a list:

			//the cross of jesus or the cross of cris? these are important questions
			//i wont double cross you!
			//the bible is a cross product
			//so is a swiss army knife
			//railroad crossing; the train is carying important product
			//i should just cross this off the list of unproductive things i have done

			Vector<T, 3> out = Vector<T, 3>();
			//whew math
			out[0] = (this->operator[](1) * right[2]) - (this->operator[](2) * right[1]);
			out[1] = (this->operator[](2) * right[0]) - (this->operator[](0) * right[2]);
			out[2] = (this->operator[](0) * right[1]) - (this->operator[](1) * right[0]);

			return out;
		};

		/**
		Calculates the dot product of 2 `Vectors`
		<p>
		This is done in o(N) time

		@param right Another `Vector`
		@return The dot product
		@see Vector for an explanation of `Vector` math
		@see cross(const Vector&, const Vector&)
		@see operator*(const T)
		@see operator*(const Vector&)
		*/
		T operator%(const Vector<T, 3>& right) const {

			T out = 0;
			for( Index i = 0; i < N; i++ ) {
				out += static_cast<T>(this->operator[](i) * right[i]);
			}
			return out;
		}

		/**
		Multiplies a `Vector` by a scalar.
		<p>
		This is done in O(N) time
		@param scalar What to multiply this `Vector` by
		@return A `Vector` scaled.
		@see operator*(const Vector&) const
		*/
		Vector operator*(const T scalar) const {
			Vector<T, N> out = *this;
			for( Index i = 0; i < N; i++ ) {
				out[i] *= scalar;
			}
			return out;
		}

		/**
		Divides a `Vector` by a scalar.
		<p>
		This is done in O(N) time
		@param scalar What to divided this `Vector` by
		@return A `Vector` scaled.
		@see operator*(const T&) const
		*/
		Vector operator/(const T scalar) const {
			Vector<T, N> out = *this;
			for( Index i = 0; i < N; i++ ) {
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
		Multiplies a `Vector` by this one
		@param right A `Vector` to multiply
		@see operator+(const Vector<T,N>&) const
		*/
		void operator*= (const Vector<T, N>& right) {
			setContents((*this * right).getContents());
		}

		/**
		Divides a `Vector` by this one
		@param right A `Vector` to divide
		@see operator+(const Vector<T,N>&) const
		*/
		void operator/= (const Vector<T, N>& right) {
			setContents((*this / right).getContents());
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
		Compares whether 2 `Vectors` have the same values.
		<p>
		This is done in O(N) time
		@param other A `Vector` to compare `this` against
		@return `true` if the 2 are equal, `false` otherwise
		@see operator!=(const Vector<T,N>) const
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator>(const Vector&) const
		*/
		bool operator==(const Vector<T, N>& other) const {
			for( Index i = 0; i < N; i++ ) {
				if( this->operator[](i) != other[i] ) {
					return false;
				}
			}
			return true;
		};

		/**
		Compares whether 2 `Vectors` don't have the same values.
		<p>
		This is done in O(N) time
		@param other A `Vector` to compare `this` against
		@return `true` if the 2 are not equal, `false` otherwise
		@see operator==(const Vector<T,N>) const
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator>(const Vector&) const
		*/
		bool operator!=(const Vector<T, N>& other) const {
			return !(*this == other);
		};

		/**
		Compares the `>` operator on 2 `Vectors` elements.
		<p>
		This is done in O(N) time
		@param other A `Vector` to compare against
		@return The result of the `>` operator on each element
		@see operator<(const Vector&) const
		@see operator>=(const Vector&) const
		@see operator<=(const Vector&) const
		@see operator==(const Vector&) const
		@see operator!=(const Vector&) const
		*/
		bool operator>(const Vector<T, N>& other) const {
			for( Index i = 0; i < N; i++ ) {
				if( this->operator[](i) <= other[i] ) {
					return false;
				}
			}
			return true;
		}

		/**
		Compares the `>=` operator on 2 `Vectors` elements.
		<p>
		This is done in O(N) time
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
		<p>
		This is done in O(N) time
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
		<p>
		This is done in O(N) time
		@param output `std::ostream` the `Matrix` was inserted into
		@param v `Matrix` which will be printed
		@return `output` for chaining
		*/
		friend std::ostream &operator<<(std::ostream &output,
										const Vector<T, N> &v) {
			output << '[' << ' ';//why not just "[ "? well, that needs std::string to be included, and thats more compiliation time. this way doesnt need that.
			for( Index x = 0; x < N; x++ ) {
				output << v[x];
				if( x != N - 1 )output << ", ";
			}
			output << ' ' << ']';
			return output;
		}

	protected:
		std::array<T, N> content;
	};//Vector

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
		Vector<T, 3> cross(const Vector<T, 3>& a, const  Vector<T, 3>& b) {

			return a* b;
		};

		/**
		Calculates the dot product of 2 `Vectors`
		@param a First `Vector` to use
		@param b Second `Vector` to use
		@return A scalar calculated from the dot product of `a` and `b`
		@see cross(const Vector&, const Vector&)
		@see magnitude(const Vector&)
		@tparam T Type of the `Vectors` being calculated. This does not need to be explicitely set.
		@tparam N Size of the `Vectors` being calculated. This does not need to be explicitely set.
		*/
		template<typename T, Size N>
		T dot(const Vector<T, N>& a, const Vector<T, N>& b) {
			return a % b;
		}

		/**
		Calculates the magnitude of a `Vector`, or how long it is.
		@param a The `Vector` to calculate from
		@return The magnitude of `Vector a`
		@see cross(const Vector&, const Vector&)
		@see dot(const Vector&, const Vector&)
		@tparam T Type of the `Vectors` being calculated. This does not need to be explicitely set.
		@tparam N Size of the `Vectors` being calculated. This does not need to be explicitely set.
		*/
		template<typename T, Size N>
		T magnitude(const Vector<T, N>& a) {
			T out = T();//assuming its numerical
			//basically the pythagereon theorum
			for( Index i = 0; i < N; i++ ) {
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
	}//Vector

}//mc

#endif