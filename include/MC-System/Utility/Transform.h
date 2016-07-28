/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <MC-System/Constants.h>
#include <MC-System/Utility/Vector.h>

namespace mc {
	namespace math {
		Matrix4f rotate(const float x, const float y, const float z);
		Matrix4f rotate(const Matrix4f m, const float x, const float y, const float z);

		Matrix4f rotateX(const float x);
		Matrix4f rotateX(const Matrix4f m, const float x);

		Matrix4f rotateY(const float y);
		Matrix4f rotateY(const Matrix4f m, const float y);

		Matrix4f rotateZ(const float z);
		Matrix4f rotateZ(const Matrix4f m, const float z);

		Matrix4f scale(const float x, const float y, const float z);
		Matrix4f scale(const Matrix4f m, const float x, const float y, const float z);

		Matrix4f translate(const float x, const float y, const float z);
		Matrix4f translate(const Matrix4f m, const float x, const float y, const float z);

		Matrix4f projection(const float FOV, const float NEAR_PLANE, const float FAR_PLANE, const float aspectRatio);
	}
	struct Transformation{
		Transformation();

		Transformation& translate(const float x, const float y, const float z);
		Transformation& rotate(const float x, const float y, const float z);
		Transformation& scale(const float x, const float y, const float z);

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
	private:
		Vector3f translation;
		Vector3f rotation;
		Vector3f scaler;
	};
}