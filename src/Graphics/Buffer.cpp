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

}
}