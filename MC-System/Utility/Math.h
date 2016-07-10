#pragma once
#include <MC-System/Utility/Vector.h>

namespace mc {
	class Math{
		Math();
	public:
		//let the bodies hit the floor
		static int floor(double value);

		static bool isPrime(int value);

		static double tan(double radians);

		static double toRadians(double degrees);

		static int nextDigitOf2(int n);

		static Matrix4f getProjectionMatrix(float FOV, float NEAR_PLANE, float FAR_PLANE, float aspectRatio);
	};
}