/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <MACE/System/Utility/Vector.h>

namespace mc {
	/**
	Namespace with various mathmatical functions
	*/
	namespace math{
		/**
		Calculates the mathmatical constant `pi`. `Pi` is the ratio of a circle's circumfernce to it's diameter, always coming out to equal roughly 3.14, no matter the size of the circle.
		<p>
		It is located here as a `long double` to have the most possible digits.
		@return Pi
		*/
		long double pi();

		/**
		Calculates the mathmatical constant `tau`. `Tau` is known as `2pi` and represents the full turn of a circle. It also represents 6.28 radians, a full circle, equal to 360 degrees.
		@return Tau
		@see #toRadians()
		*/
		long double tau();
		
		/**
		Calculates the mathmatical constant represented by `gamma`, or the `Euler–Mascheroni constant.` Gamma is used in number theory and analysis.
		@return The `Euler–Mascheroni constant`
		*/
		long double gamma();

		/**
		Calculates the mathmatical constant known as `Eulers Number,` or `e`. `E` is used as the base for the natural logarithim.
		@return `Eulers Number`
		@see logn(double);
		*/
		long double e();

		/**
		Calculates the square root of 2, also known as the `principal square root of 2`. This number when multiplied by itself returns 2. It is used in trigonometry.
		@return Square root of 2
		*/
		long double root2();

		/**
		Calculates the square root of 3, also known as the `principal square root of 3`. This number when multiplied by itself returns 3. It is used in geometery and trigonometry.
		@return Square root of 3
		*/
		long double root3();

		/**
		Calculates the square root of 5, also known as the `principal square root of 5`. This number when multiplied by itself returns 5. It is used in geometery and trigonometry.
		@return Square root of 5
		*/
		long double root5();

		/**
		Calculates the golden ratio, also represented by `phi`. The golden ratio is referenced throughout many different parts of mathmatics, including geometery, trigonometry, pyramids, the fibonacci sequence, and algebra.
		*/
		long double phi();

		/**
		Gets the absolute value of a number. The absolute value is the unsigned version of a number.
		@param value Number that is either negative or positive
		@return `|value|`
		*/
		double abs(const double value);

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
		int floor(const double value);

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
		bool isEven(const int value);
		/**
		Checks if a number is odd. An odd number is one that is not divisable by 2.
		@param value Number to check if odd
		@return `true` if the `value` is not divisable by 2, `false` otherwise
		@see isEven(const int)
		*/
		bool isOdd(const int value);
		
		/**
		Calculates a number to an exponenet, or `value^power`
		@param value Base number
		@param power The exponent
		@return `value^power`
		@see sqr(const double)
		@see cube(const double)
		*/
		double pow(const double value, const int power);

		/**
		Squares a number. Squaring a number equals a number times iteslf.
		@param value Number to square
		@return `value^2` or `value*value`
		@see pow(const double, const double)
		@see cube(const double)
		*/
		double sqr(const double value);

		/**
		Cubes a number. Cubing a number is equal to itself to the power of 3.
		@param value Number to cube
		@return `value^3` or `value*value*value`
		@see pow(const double, const double)
		@see sqr(const double)
		*/
		double cube(const double value);

		/**
		Conversion of degreees to radians.
		@param degrees An angle in degrees
		@return The same number represented as radians
		@see #pi()
		*/
		double toRadians(const double degrees);

		/**
		Conversion of radians to degrees.
		@param radians An angle in radians
		@return The same number represented as degrees
		@see #pi()
		*/
		double toDegrees(const double radians);
	};
}