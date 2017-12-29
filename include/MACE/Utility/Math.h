/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_MATH_H
#define MACE__UTILITY_MATH_H

#include <MACE/Core/Constants.h>

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
		template<typename T = long double>
		MACE_CONSTEXPR inline const T pi() {
			return T(3.14159265358979323846264338327950288419716939937510l);
		}

		/**
		Calculates the mathmatical constant `tau`. `Tau` is known as `2pi` and represents the full turn of a circle. It also represents 6.28 radians, a full circle, equal to 360 degrees.
		@return Tau
		@see #toRadians()
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T tau() {
			return T(6.28318530717958647692528676655900576839433879875021l);
		}

		/**
		Calculates the mathmatical constant represented by `gamma`, or the `Euler–Mascheroni constant.` Gamma is used in number theory and analysis.
		@return The `Euler–Mascheroni constant`
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T gamma() {
			return T(0.57721566490153286060651209008240243104215933593992l);
		}

		/**
		Calculates the mathmatical constant known as `Eulers Number,` or `e`. `E` is used as the base for the natural logarithim.
		@return `Eulers Number`
		@see logn(double);
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T e() {
			return T(2.71828182845904523536028747135266249775724709369995l);
		}

		/**
		Calculates the square root of 2, also known as the `principal square root of 2`. This number when multiplied by itself returns 2. It is used in trigonometry.
		@return Square root of 2
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T root2() {
			return T(1.41421356237309504880168872420969807856967187537694l);
		}

		/**
		Calculates the square root of 3, also known as the `principal square root of 3`. This number when multiplied by itself returns 3. It is used in geometery and trigonometry.
		@return Square root of 3
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T root3() {
			return T(1.73205080756887729352744634150587236694280525381038l);
		}

		/**
		Calculates the square root of 5, also known as the `principal square root of 5`. This number when multiplied by itself returns 5. It is used in geometery and trigonometry.
		@return Square root of 5
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T root5() {
			return T(2.23606797749978969640917366873127623544061835961152l);
		}

		/**
		Calculates the golden ratio, also represented by `phi`. The golden ratio is referenced throughout many different parts of mathmatics, including geometery, trigonometry, pyramids, the fibonacci sequence, and algebra.
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T phi() {
			return T(1.61803398874989484820458683436563811772030917980576l);
		}

		/**
		Gets the absolute value of a number. The absolute value is the unsigned version of a number.
		@param value Number that is either negative or positive
		@return `|value|`
		*/
		template<typename T>
		MACE_CONSTEXPR inline const T abs(const T value) {
			return value < T(0) ? -value : value;
		}

		/**
		Gets the lesser of two values
		@param val1 First value
		@param val2 Second value
		@return val1 if val1 < val2, val2 otherwise
		@see max(const T, const T)
		@see clamp(const T, const T, const T)
		*/
		template<typename T>
		MACE_CONSTEXPR inline const T min(const T val1, const T val2) {
			return val1 > val2 ? val2 : val1;
		}

		/**
		Gets the bigger of two values
		@param val1 First value
		@param val2 Second value
		@return `val1` if `val1 > val2`, `val2` otherwise
		@see min(const T, const T)
		@see clamp(const T, const T, const T)
		*/
		template<typename T>
		MACE_CONSTEXPR inline const T max(const T val1, const T val2) {
			return val1 > val2 ? val1 : val2;
		}

		/**
		Clamps a value in between a range.
		
		@param val Value to clamp
		@param minVal The smallest possible value. If `val < minVal`, `minVal` is returned
		@param maxVal The biggest possible value. If `val > maxVal`, `maxVal` is returned
		@return Clamped value
		@see min(const T, const T)
		@see max(const T, const T)
		*/
		template<typename T>
		MACE_CONSTEXPR inline const T clamp(const T val, const T minVal, const T maxVal) {
			return min(max(val, minVal), maxVal);
			//return val < min ? min : (val > max ? max : val);
		}

		//let the bodies hit the floor
		/**
		"Floors" a number, or rounds it down.
		@param value Number to floor
		@return `value,` but rounded down.
		@see ceil(const double)
		@see round(const double)
		*/
		template<typename T = int, typename T2 = double>
		MACE_CONSTEXPR inline const T floor(const T2 value) {
			//IEEE Standard states that it will round down in a static cast.
			//however, compilers will round towards zero, and floor should not.
			return static_cast<T>(value) < T(0) ? static_cast<T>(value) - T(1) : static_cast<T>(value);
		}

		/**
		"Ceils" a number, or rounds it up.
		@param value Number to ceil
		@return `value,` but rounded up.
		@see floor(const double)
		@see round(const double)
		*/
		template<typename T = int, typename T2 = double>
		MACE_CONSTEXPR inline const T ceil(const T2 value) {
			return floor<T, T2>(value) != value ? floor<T, T2>(value) + T(1) : floor<T, T2>(value);//so cheap, but it works
		}

		/**
		Checks if a number is even. An even number is one that is divisable by 2.
		@param value Number to check if even
		@return `true` if the `value` is divisable by 2, `false` otherwise
		@see isOdd(const int)
		*/
		template<typename T>
		MACE_CONSTEXPR inline const bool isEven(const T value) {
			return value % T(2) == T(0);
		}
		/**
		Checks if a number is odd. An odd number is one that is not divisable by 2.
		@param value Number to check if odd
		@return `true` if the `value` is not divisable by 2, `false` otherwise
		@see isEven(const int)
		*/
		template<typename T>
		MACE_CONSTEXPR inline const bool isOdd(const T value) {
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
		MACE_CONSTEXPR inline const T sqr(const T value) {
			return value * value;
		}

		/**
		Cubes a number. Cubing a number is equal to itself to the power of 3.
		@param value Number to cube
		@return `value^3` or `value*value*value`
		@see pow(const double, const double)
		@see sqr(const T)
		*/
		template<typename T>
		MACE_CONSTEXPR inline const T cube(const T value) {
			return value * value * value;
		}

		/**
		Conversion of degreees to radians.
		@param degrees An angle in degrees
		@return The same number represented as radians
		@see #pi()
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T toRadians(const T degrees) {
			return degrees * (pi<T>() / T(180.0));
		}

		/**
		Conversion of radians to degrees.
		@param radians An angle in radians
		@return The same number represented as degrees
		@see #pi()
		*/
		template<typename T = long double>
		MACE_CONSTEXPR inline const T toDegrees(const T radians) {
			return radians * (T(180.0) / pi<T>());
		}
	}//math
}//mc

#endif