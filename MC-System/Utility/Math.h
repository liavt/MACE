#pragma once
#include <MC-System/Utility/Vector.h>

namespace mc {
	/**
	Namespace with various mathmatical functions
	*/
	namespace math{

		const double EULERS_CONSTANT = 0.5772156649;

		/**
		Gets the absolute value of a number
		@param value Number that is either negative or positive
		@return a number that has to be postive
		*/
		double abs(const double value);

		/**
		"Ceils" a number, or rounds it up.
		@param value Number to ceil
		@return `value,` but rounded up.
		*/
		int ceil(const double value);

		//let the bodies hit the floor
		/**
		"Floors" a number, or rounds it down.
		@param value Number to floor
		@return `value,` but rounded down.
		*/
		int floor(const double value);

		bool isPrime(const int value);
		
		/**
		Calculate the natural logarithim of a number
		
		*/
		double ln(double value);
		
		double pow(const double value, const double power);

		double tan(const double radians);

		double toRadians(const double degrees);

		int nextDigitOf2(const int n);

		Matrix4f getProjectionMatrix(float FOV, float NEAR_PLANE, float FAR_PLANE, float aspectRatio);
	};
}