/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/Math.h>
#include <cmath>
/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
namespace mc {
	Matrix<float, 4, 4> math::rotate(const float x, const float y, const float z) {
		return rotate(identity<float, 4>(), x, y, z);
	}
	Matrix<float, 4, 4> math::rotate(const Vector<float, 3> & v) {
		return rotate(v[0], v[1], v[2]);
	}
	Matrix<float, 4, 4> math::rotate(const Matrix<float, 4, 4> & m, const Vector<float, 3> & v) {
		return rotate(m, v[0], v[1], v[2]);
	}
	Matrix<float, 4, 4> math::rotate(const Matrix<float, 4, 4>& m, const float x, const float y, const float z) {
		//Instead of having to calculate it twice, which is quite expensive, we store it in variables.
		const float cosZ = cos(z), sinZ = sin(z);
		const float cosY = cos(y), sinY = sin(y);
		const float cosX = cos(x), sinX = sin(x);


		Matrix<float, 4, 4> out = m;
		out[0][0] = cosZ*cosY;
		out[1][1] = cosZ*cosX;
		out[2][2] = cosX*cosY;

		out[0][1] = sinZ;
		out[0][2] = -sinY;
		out[1][0] = -sinZ;
		out[1][2] = sinX;
		out[2][0] = sinY;
		out[2][1] = -sinX;
		return out;
	}

	Matrix<float, 4, 4> math::scale(const float x, const float y, const float z) {
		return scale(identity<float, 4>(), x, y, z);
	}

	Matrix<float, 4, 4> math::scale(const Matrix<float, 4, 4>& m, const float x, const float y, const float z) {
		Matrix<float, 4, 4> out = m;
		out[0][0] = x;
		out[1][1] = y;
		out[2][2] = z;
		return out;
	}
	Matrix<float, 4, 4> math::translate(const float x, const float y, const float z) {
		return translate(identity<float, 4>(), x, y, z);
	}
	Matrix<float, 4, 4> math::translate(const Matrix<float, 4, 4>& in, const float x, const float y, const float z) {
		Matrix<float, 4, 4> m = in;
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
		return m;
	}
	Matrix<float, 4, 4> math::projection(const float FOV, const float NEAR_PLANE, const float FAR_PLANE, const float aspectRatio) {
		const float y_scale = (float)((1.0f / tan(math::toRadians(FOV / 2.0f))) * aspectRatio);
		const float x_scale = y_scale / aspectRatio;
		const float frustum_length = FAR_PLANE - NEAR_PLANE;

		Matrix<float, 4, 4> projectionMatrix = Matrix<float, 4, 4>();
		projectionMatrix[0][0] = x_scale;
		projectionMatrix[1][1] = y_scale;
		projectionMatrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
		projectionMatrix[2][3] = -1;
		projectionMatrix[3][2] = -((2 * NEAR_PLANE * FAR_PLANE) / frustum_length);
		projectionMatrix[3][3] = 0;

		return projectionMatrix;
	}
	Matrix<float, 4, 4> math::ortho(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) {
		Matrix<float, 4, 4> orthoMatrix;
		orthoMatrix[0][0] = 2.0f / (right - left);
		orthoMatrix[1][1] = 2.0f / (top - bottom);
		orthoMatrix[2][2] = -2.0f / (farPlane - nearPlane);

		orthoMatrix[3][0] = -((right + left) / (right - left));
		orthoMatrix[3][1] = -((top + bottom) / (top - bottom));
		orthoMatrix[3][2] = -((farPlane + nearPlane) / (farPlane - nearPlane));

		orthoMatrix[3][3] = 1.0f;

		return orthoMatrix;
	}
	TransformMatrix::TransformMatrix() {
		translation = { 0,0,0 };
		rotation = { 0,0,0 };
		scaler = { 1,1,1 };


	}
	TransformMatrix & TransformMatrix::translate(const float x, const float y, const float z) {
		translation[0] += x;
		translation[1] += y;
		translation[2] += z;
		return *this;
	}
	TransformMatrix & TransformMatrix::rotate(const float x, const float y, const float z) {
		rotation[0] += x;
		rotation[1] += y;
		rotation[2] += z;
		return *this;
	}
	TransformMatrix & TransformMatrix::scale(const float x, const float y, const float z) {
		scaler[0] += x;
		scaler[1] += y;
		scaler[2] += z;
		return *this;
	}
	TransformMatrix & TransformMatrix::reset() {
		TransformMatrix();
		return *this;
	}
	Matrix<float, 4, 4> TransformMatrix::get() const {
		return math::identity<float, 4>() * math::translate(translation[0], translation[1], translation[2])*math::rotate(rotation[0], rotation[1], rotation[2])*math::scale(scaler[0], scaler[1], scaler[2]);
	}
	bool TransformMatrix::operator==(const TransformMatrix & other) const {
		return other.translation == translation&&other.rotation == rotation&&other.scaler == scaler;
	}
	bool TransformMatrix::operator!=(const TransformMatrix & other) const {
		return !operator==(other);
	}
	bool TransformMatrix::operator>(const TransformMatrix & other) const {
		return translation > other.translation && rotation > other.rotation && scaler > other.scaler;
	}
	bool TransformMatrix::operator>=(const TransformMatrix & other) const {
		return operator>(other) || operator==(other);
	}
	bool TransformMatrix::operator<(const TransformMatrix & other) const {
		return !(operator>(other) || operator==(other));
	}
	bool TransformMatrix::operator<=(const TransformMatrix & other) const {
		return !operator>(other) || operator==(other);
	}
}
