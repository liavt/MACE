/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_VECTOR_H
#define MACE__UTILITY_VECTOR_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Error.h>
#include <MACE/Utility/Math.h>
#include <array>
#include <string>
#include <ostream>
#include <initializer_list>
#include <cmath>

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
	<p>
	There are various type aliases in place to prevent using the template parameters. They all use the following syntax:
	`Vector[size][suffix]`
	<p>
	suffixes exist for every primitive type and are the first letter of the primitive name. For example, the suffix
	for a `float` would be `f` and the suffix for an `int` would be `i`. Primitives with modifiers simply add the
	letter. The suffixes for an `unsigned char` would be `uc` and the prefix for a `long long int` would be `lli`
	<p>
	Sizes exist for vertices up to 5 objects
	<p>
	For example, to create a `Vector` that is made up of 4 floats, you would use `Vector4f`. For a `Vector` of 2
	unsigned ints, you would use `Vector2ui`
	@see Matrix
	@tparam T what the `Vector` is made of and calculates with. Can be any type/
	@tparam N amount of elements in the `Vector` which must be greater than 0.
	*/
	template <typename Child, typename T, Size N>
	struct VectorBase {
	public:
		/**
		Default constructor. Constructs an empty `Vector`.
		<p>
		The data is default initialized.
		*/
		VectorBase() : content{ } {};

		VectorBase(const T& val) : VectorBase() {
			for (Index i = 0; i < N; ++i) {
				content[i] = val;
			}
		};

		/**
		Consructs a `Vector` from the contents of an array.
		@param arr An equally-sized array whose contents will be filled into a `Vector`
		*/
		VectorBase(const T arr[N]) : VectorBase()//we need to initialize the array first, or else we will try to access an empty memory location
		{
			this->setContents(arr);//this doesnt create a brand new array, it merely fills the existing one with new content
		}

		/**
		Consructs a `Vector` from the contents of an `std::array`.
		@param contents An equally-sized `std::array` whose contents will be filled into a `Vector`
		*/
		VectorBase(const std::array<T, N>& contents) : VectorBase(contents.data()) {};

		/**
		Creates a `Vector` from an `std::initializer_list`. Allows for an aggregate-style creation.
		<p>
		Example:
		{@code
		Vector3i mat = {1, 2, 3};
		}
		@param args What to create this `Vector` with
		@todo Make this MACE_CONSTEXPR
		@throws IndexOutOfBoundsException If the amount of arguments in the initializer is not equal to the amount of objects this `Vector` holds
		*/
		VectorBase(const std::initializer_list<T> args) : VectorBase() {//this is for aggregate initializaition
			if (args.size() != N) {
				MACE__THROW(OutOfBounds, "The number of arguments MUST be equal to the size of the array.");
			}

			Index counter = 0;
			for (auto elem : args) {
				//the post increment is on purpose to make sure content[0] is accessed
				content[counter++] = elem;
			}
		}

		/**
		Copies the contents of a `Vector` into a new `Vector`
		@param obj A `Vector` to clone
		*/
		VectorBase(const Child &obj) {
			for (Index i = 0; i < N; ++i) {
				content[i] = obj[i];
			}
		};

		virtual ~VectorBase() = default;

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
			for (Index i = 0; i < N; ++i) {
				set(i, arr[i]);
			}
		};

		/**
		Retrieves how many elements this `Vector` holds
		@return How large this `Vector` is
		*/
		MACE_CONSTEXPR Size size() const noexcept {
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
#ifdef MACE_DEBUG_CHECK_ARGS
			if (i >= N) {
				MACE__THROW(OutOfBounds, std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			}
#endif
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
#ifdef MACE_DEBUG_CHECK_ARGS
			if (i >= N) {
				MACE__THROW(OutOfBounds, std::to_string(i) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			}
#endif
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
#ifdef MACE_DEBUG_CHECK_ARGS
			if (position >= N) {
				MACE__THROW(OutOfBounds, std::to_string(position) + " is greater than the size of this vector, " + std::to_string(N) + "!");
			}
#endif

			content[position] = value;
		}

		/**
		Creates an array with the data of this `Vector`, in O(N) time
		@return Pointer to `arr`
		@param arr The array to fill
		*/
		const T* flatten(T arr[N]) const {
			for (Index i = 0; i < N; ++i) {
				arr[i] = content[i];
			}
			return arr;
		}

		T* begin() {
			return content;
		}

		const T* begin() const {
			return content;
		}

		T* end() {
			return content + (sizeof(T) * (N - 1));
		}

		const T* end() const {
			return content + (sizeof(T) * (N - 1));
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
		Child operator+(const Child& right) const {
			Child out = Child(content);
			out += right;
			return out;
		};
		/**
		Subtracts 2 `Vectors` together.
		<p>
		This is done in O(N) time

		@param right Another `Vector`
		@return A `Vector` that was created by subtracting 2 `Vectors` together
		@see Vector for an explanation of `Vector` math
		*/
		Child operator-(const Child& right) const {
			Child out = Child(content);
			out -= right;
			return out;
		};

		/**
		Multiplies 2 `Vectors` together.
		<p>
		This is done in O(N) time

		@param right Another `Vector`
		@return The product of the multiplication
		@see Vector for an explanation of `Vector` math
		*/
		Child operator*(const Child& right) const {
			Child out = Child(content);
			out *= right;
			return out;
		}

		/**
		Divides 2 `Vectors` together.
		<p>
		This is done in O(N) time

		@param right Another `Vector`
		@return The quotient of 2 `Vectors`
		@see Vector for an explanation of `Vector` math
		*/
		Child operator/(const Child& right) const {
			Child out = Child(content);
			out /= right;
			return out;
		}

		/**
		Translates a `Vector` with a scalar.
		<p>
		This is done in O(N) time
		@param scalar What to translate this `Vector` by
		@return A `Vector` translated.
		@see operator*(const Vector&) const
		*/
		Child operator+(const T scalar) const {
			Child out = Child(content);
			out += scalar;
			return out;
		}

		/**
		Translates a `Vector` with a scalar.
		<p>
		This is done in O(N) time
		@param scalar What to translate this `Vector` by
		@return A `Vector` translated.
		@see operator*(const Vector&) const
		*/
		Child operator-(const T scalar) const {
			Child out = Child(content);
			out -= scalar;
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
		Child operator*(const T scalar) const {
			Child out = Child(content);
			out *= scalar;
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
		Child operator/(const T scalar) const {
			Child out = Child(content);
			out /= scalar;
			return out;
		}

		/**
		Adds a `Vector` to this one.
		@param right A `Vector` to add
		@see operator+(const Vector<T,N>&) const
		*/
		void operator+= (const Child& right) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) += right[i];
			}
		}

		/**
		Subtracts a `Vector` from this one.
		@param right A `Vector` to subtract
		@see operator-(const Vector<T,N>&) const
		*/
		void operator-= (const Child& right) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) -= right[i];
			}
		}

		/**
		Multiplies a `Vector` by this one
		@param right A `Vector` to multiply
		@see operator+(const Vector<T,N>&) const
		*/
		void operator*= (const Child& right) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) *= right[i];
			}
		}

		/**
		Divides a `Vector` by this one
		@param right A `Vector` to divide
		@see operator+(const Vector<T,N>&) const
		*/
		void operator/= (const Child& right) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) /= right[i];
			}
		}


		/**
		Translates this `Vector`
		@param scalar How much to translate by
		@see operator*(const Vector<T,3>&) const
		@see operator*(const T&) const
		*/
		void operator+= (const T& scalar) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) += scalar;
			}
		}

		/**
		Translates this `Vector`
		@param scalar How much to translate by
		@see operator*(const Vector<T,3>&) const
		@see operator*(const T&) const
		*/
		void operator-= (const T& scalar) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) -= scalar;
			}
		}

		/**
		Scales this `Vector`
		@param scalar How much to scale
		@see operator*(const Vector<T,3>&) const
		@see operator*(const T&) const
		*/
		void operator*= (const T& scalar) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) *= scalar;
			}
		}

		/**
		Divides this `Vector`
		@param scalar How much to divide by
		@see operator*(const Vector<T,3>&) const
		@see operator*(const T&) const
		*/
		void operator/= (const T& scalar) {
			for (Index i = 0; i < N; ++i) {
				operator[](i) /= scalar;
			}
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
		bool operator==(const Child& other) const {
			for (Index i = 0; i < N; ++i) {
				if (operator[](i) != other[i]) {
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
		bool operator!=(const Child& other) const {
			return !operator==(other);
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
		bool operator>(const Child& other) const {
			for (Index i = 0; i < N; ++i) {
				if (operator[](i) <= other[i]) {
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
		bool operator>=(const Child& other) const {
			return operator>(other) || operator==(other);
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
		bool operator<(const Child& other) const {
			return !operator>=(other);
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
		bool operator<=(const Child& other) const {
			return !operator>(other);
		}

		/**
		Operator used to output to `std::cout`.
		<p>
		This is done in O(N) time
		@param output `std::ostream` the `Matrix` was inserted into
		@param v `Matrix` which will be printed
		@return `output` for chaining
		*/
		friend std::ostream& operator<<(std::ostream& output,
										const Child& v) {
			output << '[' << ' ';//why not just "[ "? well, that needs std::string to be included, and thats more compiliation time. this way doesnt need that.
			for (Index x = 0; x < N; ++x) {
				output << v[x];
				if (x != N - 1) {
					output << ", ";
				}
			}
			output << ' ' << ']';
			return output;
		}

	protected:
		T content[N];
	};//VectorBase

	template<typename T, Size N>
	struct Vector: public VectorBase<Vector<T, N>, T, N> {
	public:
		using VectorBase<Vector<T, N>, T, N>::VectorBase;
	};

	template<typename T>
	struct Vector<T, 0> {
	private:
		Vector() = delete;
		~Vector() = delete;
	};

	template<Size N>
	struct Vector<void, N> {
	private:
		Vector() = delete;
		~Vector() = delete;
	};

	template<>
	struct Vector<void, 0> {
	private:
		Vector() = delete;
		~Vector() = delete;
	};

	template<typename T>
	struct Vector<T, 1>: public VectorBase<Vector<T, 1>, T, 1> {
	public:
		using VectorBase<Vector<T, 1>, T, 1>::VectorBase;

		operator T() {
			return this->operator[](0);
		}

		void operator=(const T& op) {
			this->operator[](0) = op;
		}
	};

	template<typename T>
	struct Vector<T, 2>: public VectorBase<Vector<T, 2>, T, 2> {
	public:
		using VectorBase<Vector<T, 2>, T, 2>::VectorBase;

		T& x() {
			return this->operator[](0);
		}

		const T& x() const {
			return this->operator[](0);
		}

		T& y() {
			return this->operator[](1);
		}

		const T& y() const {
			return this->operator[](1);
		}
	};

	template<typename T>
	struct Vector<T, 3>: public VectorBase<Vector<T, 3>, T, 3> {
	public:
		using VectorBase<Vector<T, 3>, T, 3>::VectorBase;

		T& x() {
			return this->operator[](0);
		}

		const T& x() const {
			return this->operator[](0);
		}

		T& y() {
			return this->operator[](1);
		}

		const T& y() const {
			return this->operator[](1);
		}

		T& z() {
			return this->operator[](2);
		}

		const T& z() const {
			return this->operator[](2);
		}
	};

	template<typename T>
	struct Vector<T, 4>: public VectorBase<Vector<T, 4>, T, 4> {
	public:
		using VectorBase<Vector<T, 4>, T, 4>::VectorBase;

		T& x() {
			return this->operator[](0);
		}

		const T& x() const {
			return this->operator[](0);
		}

		T& y() {
			return this->operator[](1);
		}

		const T& y() const {
			return this->operator[](1);
		}

		T& z() {
			return this->operator[](2);
		}

		const T& z() const {
			return this->operator[](2);
		}

		T& w() {
			return this->operator[](3);
		}

		const T& w() const {
			return this->operator[](3);
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
		Vector<T, 3> cross(const Vector<T, 3>& a, const  Vector<T, 3>& b) {
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
		}

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
			T out = 0;
			for (Index i = 0; i < N; ++i) {
				out += static_cast<T>(a[i] * b[i]);
			}
			return out;
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
			for (Index i = 0; i < N; ++i) {
				out += static_cast<T>(sqr(a[i]));
			}
			return std::sqrt(out);
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

		/**
		Linearly interpolate two `Vectors.`
		@param prog Progress between `0` and `1`, where `0` is the start and `1` is the end. Values in between `0` and `1` will return an interpolated vector
		@see `gfx::TweenComponent'
		@tparam T Type of `Vector.` Must be addable and multipliable.
		@tparam N Size of the `Vector`
		*/
		template<typename T, Size N>
		Vector<T, N> lerp(const Vector<T, N>& start, const Vector<T, N>& end, const T prog) {
			if (prog <= 0.0f) {
				return start;
			} else if (prog >= 1.0f) {
				return end;
			}
			return (start * (T(1) - prog)) + (end * prog);
		}
	}//Vector

}//mc

#endif