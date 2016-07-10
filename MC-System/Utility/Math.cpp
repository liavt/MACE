#include "Math.h"

namespace mc{

	int mc::Math::floor(double value)
	{
		return static_cast<int>(value);
	}

	bool mc::Math::isPrime(int n)
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

	int Math::nextDoubleOf2(int n)
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
	Matrix4f mc::Math::getProjectionMatrix(float FOV, float NEAR_PLANE, float FAR_PLANE, float aspectRatio)
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