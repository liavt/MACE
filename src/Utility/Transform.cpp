/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/Math.h>
#include <cmath>

namespace mc {
	Matrix<RelativeUnit, 4, 4> math::rotate(const RelativeRadian x, const RelativeRadian y, const RelativeRadian z) {
		return rotate(identity<float, 4>(), x, y, z);
	}
	Matrix<RelativeUnit, 4, 4> math::rotate(const Vector<RelativeRadian, 3> & v) {
		return rotate(v[0], v[1], v[2]);
	}
	Matrix<RelativeUnit, 4, 4> math::rotate(const Matrix<RelativeUnit, 4, 4> & m, const Vector<RelativeRadian, 3> & v) {
		return rotate(m, v[0], v[1], v[2]);
	}
	Matrix<RelativeUnit, 4, 4> math::rotate(const Matrix<RelativeUnit, 4, 4>& m, const RelativeRadian x, const RelativeRadian y, const RelativeRadian z) {
		//Instead of having to calculate it twice, which is quite expensive, we store it in variables.
		const float cosZ = std::cos(z), sinZ = std::sin(z);
		const float cosY = std::cos(y), sinY = std::sin(y);
		const float cosX = std::cos(x), sinX = std::sin(x);


		Matrix<RelativeUnit, 4, 4> out = m;
		out[0][0] = cosZ * cosY;
		out[1][1] = cosZ * cosX;
		out[2][2] = cosX * cosY;

		out[0][1] = sinZ;
		out[0][2] = -sinY;
		out[1][0] = -sinZ;
		out[1][2] = sinX;
		out[2][0] = sinY;
		out[2][1] = -sinX;
		return out;
	}

	Matrix<RelativeUnit, 4, 4> math::scale(const RelativeScale x, const RelativeScale y, const RelativeScale z) {
		return scale(identity<float, 4>(), x, y, z);
	}

	Matrix<RelativeUnit, 4, 4> math::scale(const Matrix<RelativeUnit, 4, 4>& m, const RelativeScale x, const RelativeScale y, const RelativeScale z) {
		Matrix<RelativeUnit, 4, 4> out = m;
		out[0][0] *= x;
		out[1][1] *= y;
		out[2][2] *= z;
		return out;
	}
	Matrix<RelativeUnit, 4, 4> math::translate(const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z) {
		return translate(identity<float, 4>(), x, y, z);
	}
	Matrix<RelativeUnit, 4, 4> math::translate(const Matrix<RelativeUnit, 4, 4>& in, const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z) {
		Matrix<RelativeUnit, 4, 4> m = in;
		m[3][0] += x;
		m[3][1] += y;
		m[3][2] += z;
		return m;
	}
	Matrix<RelativeUnit, 4, 4> math::projection(const float FOV, const float NEAR_PLANE, const float FAR_PLANE, const float aspectRatio) {
		const float y_scale = (float)((1.0f / std::tan(math::toRadians(FOV / 2.0f))) * aspectRatio);
		const float x_scale = y_scale / aspectRatio;
		const float frustum_length = FAR_PLANE - NEAR_PLANE;

		Matrix<RelativeUnit, 4, 4> projectionMatrix = Matrix<RelativeUnit, 4, 4>();
		projectionMatrix[0][0] = x_scale;
		projectionMatrix[1][1] = y_scale;
		projectionMatrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
		projectionMatrix[2][3] = -1.0f;
		projectionMatrix[3][2] = -((2.0f * NEAR_PLANE * FAR_PLANE) / frustum_length);
		projectionMatrix[3][3] = 0.0f;

		return projectionMatrix;
	}
	Matrix<RelativeUnit, 4, 4> math::ortho(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) {
		Matrix<RelativeUnit, 4, 4> orthoMatrix;
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
	TransformMatrix & TransformMatrix::translate(const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z) {
		translation[0] += x;
		translation[1] += y;
		translation[2] += z;
		return *this;
	}
	TransformMatrix & TransformMatrix::rotate(const RelativeRadian x, const RelativeRadian y, const RelativeRadian z) {
		rotation[0] += x;
		rotation[1] += y;
		rotation[2] += z;
		return *this;
	}
	TransformMatrix & TransformMatrix::scale(const RelativeScale x, const RelativeScale y, const RelativeScale z) {
		scaler[0] *= x;
		scaler[1] *= y;
		scaler[2] *= z;
		return *this;
	}
	TransformMatrix & TransformMatrix::reset() {
		scaler = { 1.0f, 1.0f, 1.0f };
		translation = { 0.0f, 0.0f, 0.0f };
		rotation = { 0.0f, 0.0f, 0.0f };
		return *this;
	}
	Matrix<RelativeUnit, 4, 4> TransformMatrix::get() const {
		return math::identity<float, 4>() * math::translate(translation[0], translation[1], translation[2]) * math::rotate(rotation[0], rotation[1], rotation[2]) * math::scale(scaler[0], scaler[1], scaler[2]);
	}
	bool TransformMatrix::collides2D(const TransformMatrix & other) const {
		const Vector<RelativeUnit, 4> thisAABB{ (translation[0] / 2.0f) + 0.5f, (translation[1] / 2.0f) + 0.5f, scaler[0], scaler[1] };
		const Vector<RelativeUnit, 4> otherAABB{ (other.translation[0] / 2.0f) + 0.5f, (other.translation[1] / 2.0f) + 0.5f, other.scaler[0], other.scaler[1] };

		if (thisAABB.x() < otherAABB.x() + otherAABB.z() &&
			thisAABB.x() + thisAABB.z() > otherAABB.x() &&
			thisAABB.y() < otherAABB.y() + otherAABB.w() &&
			thisAABB.w() + thisAABB.y() > otherAABB.y()) {
			return true;
		}

		return false;
	}

	bool TransformMatrix::operator==(const TransformMatrix & other) const {
		return other.translation == translation && other.rotation == rotation && other.scaler == scaler;
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
