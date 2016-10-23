/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Texture.h>
#include <MACE/Graphics/GLUtil.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace mc {
namespace gfx {

void Texture::generateTexture()
{
	glGenTextures(1, &id);

	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	unbind();

	checkGLError();
}

Texture::Texture()
{
	id = 0;
}

Texture::Texture(Index id)
{
	this->id = id;
}

Texture::Texture(const char * file)
{
	generateTexture();
	bind();
	loadFile(file);
	unbind();
}


Texture::Texture(const GLvoid * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel)
{
	generateTexture();
	bind();
	setData(data, width, height, type, format, internalFormat, mipmapLevel);
	unbind();
}

void Texture::setData(const void * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel)
{
	glTexImage2D(target, mipmapLevel, internalFormat, width, height, 0, format, type, data);

	checkGLError();
}

void Texture::loadFile(const char * file)
{
	int width, height, componentSize;

	Byte* image = stbi_load(file, &width, &height, &componentSize, STBI_rgb_alpha);

	setData(image, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);

	stbi_image_free(image);
}

void Texture::setTarget(GLenum target)
{
	this->target = target;
}

GLenum Texture::getTarget()
{
	return target;
}

int Texture::getID() const
{
	return id;
}

void Texture::bind() const
{
	glBindTexture(target, id);
}

void Texture::unbind() const
{
	glBindTexture(target, 0);
}

Color & Texture::getPaint()
{
	return paint;
}

const Color & Texture::getPaint() const
{
	return paint;
}

void Texture::setPaint(const Color & c)
{
	paint = c;
}

float Texture::getOpacity()
{
	return opacity;
}

const float Texture::getOpacity() const
{
	return opacity;
}

void Texture::setOpacity(const float f)
{
	opacity = f;
}

}
}