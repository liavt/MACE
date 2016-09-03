/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <MACE/System/Constants.h>
#include <MACE/System/Utility/Matrix.h>

namespace mc {
	namespace math {
		/**
		Creates a rotation matrix, which when multiplied by a `Vector4f`, rotates it.
		@param x Rotation in radians around the X axis
		@param y Rotation in radians around the Y axis
		@param z Rotation in radians around the Z axis
		@return A rotation `Matrix4f`
		*/
		Matrix4f rotate(const float x, const float y, const float z);
		Matrix4f rotate(const Vector3f& v);
		Matrix4f rotate(const Matrix4f& m,const Vector3f& v);
		Matrix4f rotate(const Matrix4f& m, const float x, const float y, const float z);

		Matrix4f rotateX(const float x);
		Matrix4f rotateX(const Matrix4f& m, const float x);

		Matrix4f rotateY(const float y);
		Matrix4f rotateY(const Matrix4f& m, const float y);

		Matrix4f rotateZ(const float z);
		Matrix4f rotateZ(const Matrix4f& m, const float z);

		Matrix4f scale(const float x, const float y, const float z);
		Matrix4f scale(const Matrix4f& m, const float x, const float y, const float z);

		Matrix4f translate(const float x, const float y, const float z);
		Matrix4f translate(const Matrix4f& m, const float x, const float y, const float z);

		Matrix4f projection(const float FOV, const float NEAR_PLANE, const float FAR_PLANE, const float aspectRatio);
		Matrix4f ortho(const float left, const float right, const float bottom, const float top, const float near, const float far);
	}
	struct TransformMatrix{
		TransformMatrix();

		TransformMatrix& translate(const float x, const float y, const float z);
		TransformMatrix& rotate(const float x, const float y, const float z);
		TransformMatrix& scale(const float x, const float y, const float z);
		TransformMatrix& reset();

		Matrix4f get() const;

		Vector3f& getRotation();
		Vector3f& getTranslation();
		Vector3f& getScale();

		const Vector3f& getRotation() const;
		const Vector3f& getTranslation() const;
		const Vector3f& getScale() const;

		void setRotation(Vector3f& newVector);
		void setTranslation(Vector3f& newVector);
		void setScale(Vector3f& newVector);

		bool operator==(const TransformMatrix& other) const;
		bool operator!=(const TransformMatrix& other) const;

		Vector3f translation;
		Vector3f rotation;
		Vector3f scaler;
	};
}