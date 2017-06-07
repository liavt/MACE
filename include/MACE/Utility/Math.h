/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_MATH_H
#define MACE_UTILITY_MATH_H

#include <MACE/Utility/Vector.h>

namespace mc {
	/**
	Namespace with various mathmatical functions
	*/
	namespace math {
		/**
		Calculates the mathmatical constant `pi`. `Pi` is the ratio of a circle's circumfernce to it's diameter, always coming out to equal roughly 3.14, no matter the size of the circle.
		<p>
		It is located here as a `long double` to have the most possible digits.
		@return Pi
		*/
		constexpr inline long double pi() {
			return 3.14159265358979323846264338327950288419716939937510l;
		}

		/**
		Calculates the mathmatical constant `tau`. `Tau` is known as `2pi` and represents the full turn of a circle. It also represents 6.28 radians, a full circle, equal to 360 degrees.
		@return Tau
		@see #toRadians()
		*/
		constexpr inline long double tau() {
			return 6.28318530717958647692528676655900576839433879875021l;
		}

		/**
		Calculates the mathmatical constant represented by `gamma`, or the `Euler–Mascheroni constant.` Gamma is used in number theory and analysis.
		@return The `Euler–Mascheroni constant`
		*/
		constexpr inline long double gamma() {
			return 0.57721566490153286060651209008240243104215933593992l;
		}

		/**
		Calculates the mathmatical constant known as `Eulers Number,` or `e`. `E` is used as the base for the natural logarithim.
		@return `Eulers Number`
		@see logn(double);
		*/
		constexpr inline long double e() {
			return 2.71828182845904523536028747135266249775724709369995l;
		}

		/**
		Calculates the square root of 2, also known as the `principal square root of 2`. This number when multiplied by itself returns 2. It is used in trigonometry.
		@return Square root of 2
		*/
		constexpr inline long double root2() {
			return 1.41421356237309504880168872420969807856967187537694l;
		}

		/**
		Calculates the square root of 3, also known as the `principal square root of 3`. This number when multiplied by itself returns 3. It is used in geometery and trigonometry.
		@return Square root of 3
		*/
		constexpr inline long double root3() {
			return 1.73205080756887729352744634150587236694280525381038l;
		}

		/**
		Calculates the square root of 5, also known as the `principal square root of 5`. This number when multiplied by itself returns 5. It is used in geometery and trigonometry.
		@return Square root of 5
		*/
		constexpr inline long double root5() {
			return 2.23606797749978969640917366873127623544061835961152l;
		}

		/**
		Calculates the golden ratio, also represented by `phi`. The golden ratio is referenced throughout many different parts of mathmatics, including geometery, trigonometry, pyramids, the fibonacci sequence, and algebra.
		*/
		constexpr inline long double phi() {
			return 1.61803398874989484820458683436563811772030917980576l;
		}

		/**
		Gets the absolute value of a number. The absolute value is the unsigned version of a number.
		@param value Number that is either negative or positive
		@return `|value|`
		*/
		template<typename T>
		constexpr inline T abs(const T value) {
			return value < 0 ? -value : value;
		}

		template<signed char>
		constexpr inline unsigned char abs(const signed char value) {
			return value < 0 ? -value : value;
		}

		template<signed int>
		constexpr inline unsigned int abs(const signed int value) {
			return value < 0 ? -value : value;
		}

		template<signed short int>
		constexpr inline unsigned short int abs(const signed short int value) {
			return value < 0 ? -value : value;
		}

		template<signed long int>
		constexpr inline unsigned long int abs(const signed long int value) {
			return value < 0 ? -value : value;
		}

		template<signed long long int>
		constexpr inline unsigned long long int abs(const signed long long int value) {
			return value < 0 ? -value : value;
		}

		/**
		"Ceils" a number, or rounds it up.
		@param value Number to ceil
		@return `value,` but rounded up.
		@see floor(const double)
		@see round(const double)
		*/
		int ceil(const double value);

		//let the bodies hit the floor
		/**
		"Floors" a number, or rounds it down.
		@param value Number to floor
		@return `value,` but rounded down.
		@see ceil(const double)
		@see round(const double)
		*/
		constexpr inline int floor(const double value) {
			//IEEE Standard states that it will round down in a static cast.
			//however, compilers will round towards zero, and floor should not.
			return static_cast<int>(value) < 0 ? static_cast<int>(value) - 1 : static_cast<int>(value);
		}

		/**
		Verifies whether a number is prime. A prime number is one that's only factors are one and itself.
		@param value Number to check if prime
		@return `true` if the number is prime, `false` otherwise
		*/
		bool isPrime(const int value);

		/**
		Checks if a number is even. An even number is one that is divisable by 2.
		@param value Number to check if even
		@return `true` if the `value` is divisable by 2, `false` otherwise
		@see isOdd(const int)
		*/
		constexpr inline bool isEven(const int value) {
			return value % 2 == 0;
		}
		/**
		Checks if a number is odd. An odd number is one that is not divisable by 2.
		@param value Number to check if odd
		@return `true` if the `value` is not divisable by 2, `false` otherwise
		@see isEven(const int)
		*/
		constexpr inline bool isOdd(const int value) {
			return !isEven(value);
		}

		/**
		Squares a number. Squaring a number equals a number times iteslf.
		@param value Number to square
		@return `value^2` or `value*value`
		@see pow(const double, const double)
		@see cube(const T)
		*/
		template<typename T>
		constexpr inline T sqr(const T value) {
			return value*value;
		}

		/**
		Cubes a number. Cubing a number is equal to itself to the power of 3.
		@param value Number to cube
		@return `value^3` or `value*value*value`
		@see pow(const double, const double)
		@see sqr(const T)
		*/
		template<typename T>
		constexpr inline T cube(const T value) {
			return value*value*value;
		}

		/**
		Conversion of degreees to radians.
		@param degrees An angle in degrees
		@return The same number represented as radians
		@see #pi()
		*/
		constexpr inline double toRadians(const double degrees) {
			return degrees*(static_cast<double>(pi()) / 180.0);
		}

		/**
		Conversion of radians to degrees.
		@param radians An angle in radians
		@return The same number represented as degrees
		@see #pi()
		*/
		constexpr inline double toDegrees(const double radians) {
			return radians*(180.0 / static_cast<double>(pi()));
		}
	}//math
}//mc

#endif