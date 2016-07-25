#include <MC-System/Utility/Transform.h>
#include <MC-System/Utility/Math.h>
#include <cmath>
/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
namespace mc {
Matrix4f math::rotate(const float x, const float y, const float z)
{
	return rotate(identity<float, 4>(),x,y,z);
}
Matrix4f math::rotate(const Matrix4f m, const float x, const float y, const float z)
{
	return rotateX(m,x)*rotateY(m,y)*rotateZ(m,z);
}
Matrix4f math::rotateX(const float x)
{
	return rotateX(identity<float,4>(), x);
}
Matrix4f math::rotateX(const Matrix4f m, const float x)
{
	Matrix4f out = m;
	out[1][1] = cos(x);
	out[2][2] = cos(x);
	out[1][2] = sin(x);
	out[2][1] = -sin(x);
	return out;
}
Matrix4f math::rotateY(const float y)
{
	return rotateY(identity<float, 4>(),y);
}
Matrix4f math::rotateY(const Matrix4f m, const float y)
{
	Matrix4f out = m;
	out[0][0] = cos(y);
	out[0][2] = -sin(y);
	out[2][0] = sin(y);
	out[2][2] = cos(y);
	return out;
}
Matrix4f math::rotateZ(const float z)
{
	return rotateZ(identity<float, 4>(), z);
}
Matrix4f math::rotateZ(const Matrix4f m, const float z)
{
	Matrix4f out = m;
	out[0][0] = cos(z);
	out[0][1] = sin(z);
	out[1][0] = -sin(z);
	out[1][1] = cos(z);
	return out;
}
Matrix4f math::scale(const float x, const float y, const float z) {
	return scale(identity<float, 4>(), x, y, z);
}

Matrix4f math::scale(const Matrix4f m, const float x, const float y, const float z)
{
	Matrix4f out = m;
	out[0][0] = x;
	out[1][1] = y;
	out[2][2] = z;
	return out;
}
Matrix4f math::translate( const float x, const float y, const float z)
{
	return translate(identity<float, 4>(), x, y, z);
}
Matrix4f math::translate(const Matrix4f in,const float x, const float y, const float z)
{
	Matrix4f m = in;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	return m;
}
Matrix4f math::projection(const float FOV, const float NEAR_PLANE, const float FAR_PLANE, const float aspectRatio)
{
	const float y_scale = (float)((1.0f / tan(math::toRadians(FOV / 2.0f))) * aspectRatio);
	const float x_scale = y_scale / aspectRatio;
	const float frustum_length = FAR_PLANE - NEAR_PLANE;

	Matrix4f projectionMatrix = Matrix4f();
	projectionMatrix[0][0] = x_scale;
	projectionMatrix[1][1] = y_scale;
	projectionMatrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
	projectionMatrix[2][3] = -1;
	projectionMatrix[3][2] = -((2 * NEAR_PLANE * FAR_PLANE) / frustum_length);
	projectionMatrix[3][3] = 0;

	return projectionMatrix;
}
Transformation::Transformation()
{
	translation = {0,0,0};
	rotation = { 0,0,0 };
	scaler = { 1,1,1 };


}
Transformation & Transformation::translate(const float x, const float y, const float z)
{
	translation[0] += x;
	translation[1] += y;
	translation[2] += z;
	return *this;
}
Transformation & Transformation::rotate(const float x, const float y, const float z)
{
	rotation[0] += x;
	rotation[1] += y;
	rotation[2] += z;
	return *this;
}
Transformation & Transformation::scale(const float x, const float y, const float z)
{
	scaler[0] += x;
	scaler[1] += y;
	scaler[2] += z;
	return *this;
}
Matrix4f Transformation::get()
{
	return math::translate(translation[0], translation[1], translation[2])*math::rotate(rotation[0], rotation[1], rotation[2])*math::scale(scaler[0], scaler[1], scaler[2]);
}
}
