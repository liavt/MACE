#include "Math.h"
#include <cmath>

namespace mc{
	double math::abs(const double value)
	{
		return value < 0 ? -value : value;
	}

	int math::ceil(const double value)
	{
		int out = floor(value);
		if (out != value)out++;//ceil(5) should equal 5, not 6. check to make sure it is actually a whole number
		return out;//so cheap, but it works
	}
	int mc::math::floor(const double value)
	{
		int out = static_cast<int>(value);
		if (out < 0)out--;//so floor(-5.7) equals -6, not -5. compilers will round towards zero, floor() shouldnt
		return out;
	}

	bool mc::math::isPrime(const int n)
	{
		if (n % 2 == 0) {
			return false;
		}
		for (int i = 3; i * i <= n; i += 2) {
			if (n % i == 0) {
				return false;
			}
		}
		return true;
	}

	bool math::isEven(const int value)
	{
		int out = value;
		return out % 2==0;
	}

	bool math::isOdd(const int value)
	{
		return !isEven(value);
	}

	//pow pow is having fun tonight
	double math::pow(const double value, const double power)
	{
		if (power == 0)return 1.0;//any number to the power of 0 is 1. even 0. 0^0=1. axioms are hilarious.

		double out = value;
		//boom! pow!
		for (unsigned int i = 0; i < abs(power)-1; i++) {
			out = out * value;
		}

		if (power < 0) {
			out = (1.0 / out);
		}

		return out ;
	}

	double math::sqr(const double value)
	{
		return value*value;
	}

	double math::cube(const double value)
	{
		return value*value*value;
	}

	double math::tan(const double x)
	{
		//whew lad polynomial approximations are quite crazy
		return (double)(x+((pow(x,3.0))/3.0)+((2.0*(pow(x,5.0)))/15.0)+((17*pow(x,7.0))/315.0));
	}

	int math::log2(const int n)
	{
		int v = n;
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	/*	
	Matrix4f mc::math::getProjectionMatrix(float FOV, float NEAR_PLANE, float FAR_PLANE, float aspectRatio)
	{
		float y_scale = (float)((1.0f / tan(toRadians(FOV / 2.0f))) * aspectRatio);
		float x_scale = y_scale / aspectRatio;
		float frustum_length = FAR_PLANE - NEAR_PLANE;

		Matrix4f projectionMatrix = Matrix4f();
		projectionMatrix[0][0] = x_scale;
		projectionMatrix[1][1] = y_scale;
		projectionMatrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
		projectionMatrix[2][3] = -1;
		projectionMatrix[3][2] = -((2 * NEAR_PLANE * FAR_PLANE) / frustum_length);
		projectionMatrix[3][3] = 0;

		return projectionMatrix;
	}*/
}