#pragma once
#include <MC-System/Utility/Vector.h>

namespace mc {
	/**
	Namespace with various mathmatical functions
	*/
	namespace math{

		const double EULERS_CONSTANT = 0.5772156649;

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
		Calculate the natural logarithim of a number
		@see log2(const int)
		*/
		double logn(double value);
		
		/**
		Calculates a number to an exponenet, or `value^power`
		@param value Base number
		@param power The exponent
		@return `value^power`
		@see sqr(const double)
		@see cube(const double)
		*/
		double pow(const double value, const double power);

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
		Calculates the tangent of an angle.
		@param radians Angle in radians
		@return The tangent of the input
		@see toRadians(cosnt double)
		*/
		double tan(const double radians);

		double toRadians(const double degrees);

		int log2(const int n);

		Matrix4f getProjectionMatrix(float FOV, float NEAR_PLANE, float FAR_PLANE, float aspectRatio);
	};
}