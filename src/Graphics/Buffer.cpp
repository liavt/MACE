/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Buffer.h>

namespace mc {
namespace gfx {

void UBO::setLocation(const Index loc)
{
	this->location = loc;
}

Index UBO::getLocation()
{
	return location;
}

const Index UBO::getLocation() const
{
	return location;
}

void UBO::bindForRender(const Index offset, const GLsizeiptr size) const
{
	if (size<0 && offset == 0) {
		glBindBufferBase(GL_UNIFORM_BUFFER, this->location, id);
	}
	else {
		glBindBufferRange(GL_UNIFORM_BUFFER, this->location, id, offset, size);
	}
}

void UBO::bindToUniformBlock(const Index programID, const char* blockName) const
{
	glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, blockName), location);
}

void FBO::init()
{
	glGenFramebuffers(1,&location);
}

void FBO::destroy()
{
	glDeleteFramebuffers(1,&location);
	location = 0;
}

void FBO::bind(const GLenum& target) const
{
	glBindFramebuffer(target, location);
}

void FBO::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::attachTexture(const GLenum & target, const GLenum & attachment, const GLuint & textureID, const GLint & level)
{
	bind();
	glFramebufferTexture(target, attachment, textureID, level);
}

void FBO::attachTexture1D(const GLenum & target, const GLenum & attachment, const GLenum & texTarget, const GLuint & textureID, const GLint & level)
{
	bind();
	glFramebufferTexture1D(target, attachment, texTarget, textureID, level);
}

void FBO::attachTexture2D(const GLenum & target, const GLenum & attachment, const GLenum & texTarget, const GLuint & textureID, const GLint & level)
{
	bind();
	glFramebufferTexture2D(target, attachment, texTarget, textureID, level);
}

void FBO::attachTextureLayer(const GLenum & target, const GLenum & attachment, const GLuint& texture, const GLint & level, const GLint & layer)
{
	bind();
	glFramebufferTextureLayer(target, attachment, level, texture, layer);
}

void FBO::attachRenderbuffer(const GLenum & target, const GLenum & attachment, const RBO & buffer)
{
	bind();
	glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, buffer.getLocation());
}

void FBO::setParameter(const GLenum & paramName, const GLint & value)
{
	glFramebufferParameteri(location, paramName, value);
}

void FBO::setDrawBuffers(const Size & arrSize, const GLenum * buffers)
{
	glDrawBuffers(arrSize, buffers);
}

GLboolean FBO::isCreated() const
{
	return glIsFramebuffer(location);
}

GLenum FBO::checkStatus(const GLenum & target)
{
	bind();
	return glCheckFramebufferStatus(target);
}

Index FBO::getLocation() const
{
	return location;
}

void RBO::init()
{
	glGenRenderbuffers(1, &location);
}

void RBO::destroy()
{
	glDeleteRenderbuffers(1, &location);
}

void RBO::bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, location);
}

void RBO::unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER,location);
}

void RBO::setStorage(const GLenum & format, const GLsizei & width, const GLsizei & height)
{
	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

void RBO::setStorageMultisampled(const GLsizei & samples, const GLenum & format, const GLsizei & width, const GLsizei & height)
{
	bind();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
}

GLboolean RBO::isCreated() const
{
	return glIsRenderbuffer(location);
}

Index RBO::getLocation() const
{
	return location;
}

}
}