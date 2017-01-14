/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_TRANSFORM_H
#define MACE_UTILITY_TRANSFORM_H

#include <MACE/Core/Constants.h>
#include <MACE/Utility/Matrix.h>

namespace mc {
	namespace math {
		/**
		Creates a rotation matrix, which when multiplied by a `Vector4f`, rotates it.
		<p>
		The base `Matrix4f` is an identity matrix
		@param x Rotation in radians around the X axis
		@param y Rotation in radians around the Y axis
		@param z Rotation in radians around the Z axis
		@return A rotation `Matrix4f`
		*/
		Matrix<float,4, 4> rotate(const float x, const float y, const float z);
		/**
		Creates a rotation matrix, which when multiplied by a `Vector4f`, rotates it.
		<p>
		The base `Matrix4f` is an identity matrix
		@param v A quaternion representing a rotation
		@return A rotated `Matrix4f`
		*/
		Matrix<float, 4, 4> rotate(const Vector<float, 3>& v);
		/**
		Rotates an existing `Matrix4f`
		@param v A quaternion representing a rotation
		@param m `Matrix4f` base to rotate
		@return A rotated `Matrix4f`
		*/
		Matrix<float, 4, 4> rotate(const Matrix<float, 4, 4>& m, const Vector<float, 3>& v);
		/**
		Rotates an existing `Matrix4f`
		@param m A `Matrix4f` to rotate
		@param x Rotation in radians around the X axis
		@param y Rotation in radians around the Y axis
		@param z Rotation in radians around the Z axis
		@return A rotated `Matrix4f`
		*/
		Matrix<float, 4, 4> rotate(const Matrix<float, 4, 4>& m, const float x, const float y, const float z);

		/**
		Creates a scaling matrix, that when multiplied by a vector, scales the X, Y, and Z values.
		<p>
		The base `Matrix4f` is an identity matrix
		@param x How much to scale the X coordinate
		@param y How much to scale the Y coordinate
		@param z How much to scale the Z coordinate
		@return A `Matrix4f` that is scaled based on the supplied values
		*/
		Matrix<float, 4, 4> scale(const float x, const float y, const float z);
		/**
		Scales an existing transformation matrix.
		@param m The base `Matrix4f` to get scaled
		@param x How much to scale the X coordinate
		@param y How much to scale the Y coordinate
		@param z How much to scale the Z coordinate
		@return A `Matrix4f` that is scaled based on the supplied values
		*/
		Matrix<float, 4, 4> scale(const Matrix<float, 4, 4>& m, const float x, const float y, const float z);

		/**
		Creates a translation matrix, that when multiplied by a vector, translates the X, Y, and Z values.
		<p>
		The base `Matrix4f` is an identity matrix
		@param x How much to translate the X coordinate
		@param y How much to translate the Y coordinate
		@param z How much to translate the Z coordinate
		@return A `Matrix4f` that is translated based on the supplied values
		*/
		Matrix<float, 4, 4> translate(const float x, const float y, const float z);
		/**
		Translates an existing transformation matrix.
		@param m The base `Matrix4f` to get translated
		@param x How much to translate the X coordinate
		@param y How much to translate the Y coordinate
		@param z How much to translate the Z coordinate
		@return A `Matrix4f` that is translated based on the supplied values
		*/
		Matrix<float, 4, 4> translate(const Matrix<float, 4, 4>& m, const float x, const float y, const float z);

		/**
		Generates a projection matrix based on values. Each time the window changes size, you need to regenerate your projection matrix
		<p>
		Projection matrices are used in 3D graphics. Vectors multiplied by a projection matrix are transformed to "look" 3-dimensional.
		@param FOV The field of vision, in degrees
		@param NEAR_PLANE A `float` representing the near clipping plane. Z values lower than this are not visible to the viewer
		@param FAR_PLANE A `float` representing the far clipping plane. Z values higher than this are not visible to the viewer
		@param aspectRatio The aspect ratio of the screen.
		@return A projection matrix made from the specified settings
		@see ortho(const float, const float, const float, const float, const float)
		*/
		Matrix<float, 4, 4> projection(const float FOV, const float NEAR_PLANE, const float FAR_PLANE, const float aspectRatio);
		/**
		Generates an orthographic projection matrix. As opposed to a standard projection matrix, vectors in an orthographic projection don't become smaller the farther away they are. When combined with a rotational matrix, it can create an isometric view, which is used extensively in games like Simcity. Isometric views are also used heavily in 3D modeling software to represent the dimensions of an object.In a 2D environment, an orthographic projection matrix can be used to scale the screen.
		@param left The left plane of the orthographic projection
		@param right The right plane of the orthographic projection
		@param bottom The bottom plane of the orthographic projection
		@param top The top plane of the orthographic projection
		@param near The near plane of the orthographic projection
		@param far The far plane of the orthographic projection
		@return An ortographic projection matrix generated from the values.
		@see projection(const float, const float, const float, const float)
		*/
		Matrix<float, 4, 4> ortho(const float left, const float right, const float bottom, const float top, const float near, const float far);

	}//math

		/**
		Matrix-based class that allows for the easy creation and management of transformation matrices. Transformation matrices can scale, rotate, and translate any vector.
		<p>
		This should not be confused with the `Matrix` class, which allows for the arbitrary storage of data in a matrix-like fashion
		*/
	struct TransformMatrix {
		/**
		Default constructor. Generates a `TransformMatrix` as an identity matrix.
		@see reset()
		*/
		TransformMatrix();

		/**
		Translates this `TransformMatrix` in any 3 directions.
		@param x How much to translate in the X plane.
		@param y How much to translate in the Y plane.
		@param z How much to translate in the Z plane.
		@return Itself for chaining
		@see translate(const Matrix4f&, const float, const float, const float)
		*/
		TransformMatrix& translate(const float x, const float y, const float z);
		/**
		Rotates this `TransformMatrix` in any 3 directions.
		@param x How much to rotate in the X plane.
		@param y How much to rotate in the Y plane.
		@param z How much to rotate in the Z plane.
		@return Itself for chaining
		@see rotate(const Matrix4f&, const float, const float, const float)
		*/
		TransformMatrix& rotate(const float x, const float y, const float z);
		/**
		Scales this `TransformMatrix` in any 3 directions.
		@param x How much to scale in the X plane.
		@param y How much to scale in the Y plane.
		@param z How much to scale in the Z plane.
		@return Itself for chaining
		@see scale(const Matrix4f&, const float, const float, const float)
		*/
		TransformMatrix& scale(const float x, const float y, const float z);
		/**
		Deletes any transformations on this `TransformMatrix`, effectively making it an identity matrix.
		@return Itself for chaining
		*/
		TransformMatrix& reset();

		/**
		Converts this `TransformMatrix` into a `Matrix4f` based on the stored transformations.
		@return The transformation matrix represented by the values stored via `rotate()`, `scale()`, and `translate()`
		*/
		Matrix<float, 4, 4> get() const;

		/**
		Checks if the values represented by 2 `TransformMatrix` are the same
		@param other What to compare to
		@return Whether `this` and `other` are equal
		*/
		bool operator==(const TransformMatrix& other) const;
		/**
		Checks if the values represented by 2 `TransformMatrix` are not the same
		@param other What to compare to
		@return Whether `this` and `other` are different
		*/
		bool operator!=(const TransformMatrix& other) const;
		/**
		Checks whether a `TransformMatrix` is larger than another
		@param other What to compare to
		@return whether `this` is larger than `other`, based on `translation`, `rotation`, and `scaler`
		*/
		bool operator>(const TransformMatrix& other) const;
		/**
		Checks whether a `TransformMatrix` is larger than or equal to another
		@param other What to compare to
		@return whether `this` is larger than or equal to `other`, based on `translation`, `rotation`, and `scaler`
		*/
		bool operator>=(const TransformMatrix& other) const;
		/**
		Checks whether a `TransformMatrix` is smaller than another
		@param other What to compare to
		@return whether `this` is smaller than `other`, based on `translation`, `rotation`, and `scaler`
		*/
		bool operator<(const TransformMatrix& other) const;
		/**
		Checks whether a `TransformMatrix` is small than or equal to another
		@param other What to compare to
		@return whether `this` is smaller than or equal to `other`, based on `translation`, `rotation`, and `scaler`
		*/
		bool operator<=(const TransformMatrix& other) const;

		/**
		Represents this `TransformMatrix`'s translation
		@see translate(const float, const float, const float)
		@see get()
		*/
		Vector<float, 3> translation;
		/**
		Represents this `TransformMatrix`'s rotation
		@see rotate(const float, const float, const float)
		@see get()
		*/
		Vector<float, 3> rotation;
		/**
		Represents this `TransformMatrix`'s scale
		@see scale(const float, const float, const float)
		@see get()
		*/
		Vector<float, 3> scaler;
	};//TransformMatrix
}//mc

#endif