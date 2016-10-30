/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <MACE/System/Constants.h>
#include <MACE/System/Utility/Color.h>
#include <GL/glew.h>

namespace mc {
namespace gfx {

class Texture {
public:

	Texture();
	Texture(Index id);
	Texture(const char* file);
	Texture(const void * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

	void setData(const void * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

	void loadFile(const char* file);

	void setTarget(const GLenum target);
	GLenum getTarget();

	int getID() const;

	void bind() const;
	void unbind() const;

	Color& getPaint();
	const Color& getPaint() const;
	void setPaint(const Color& c);

	float getOpacity();
	const float getOpacity() const;
	void setOpacity(const float f);

	void setParameter(const GLenum& name, const GLint& value);
private:
	Index id = 0;
	GLenum target = GL_TEXTURE_2D;

	Color paint = Color(1.0f, 1.0f, 1.0f, 1.0f);

	float opacity = 1.0f;

	void generateTexture();
};//Texture


}//gfx
}//mc