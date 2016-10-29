/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#include <MACE/System/Constants.h>
#include <GL/glew.h>

namespace mc {
namespace gfx {

class VAO {
public:
	Index vaoID;
	Size vertexNumber, indiceNumber;

	~VAO();

	void bind();
	void unbind();

	void draw(const GLenum type=GL_TRIANGLES);

	void loadVertices(const Size&& verticeSize, const GLfloat vertices[], const Index&& location = 15, const Size&& attributeSize = 3);
	void loadIndices(const Size&& indiceNum, const GLuint indices[], const GLenum&& drawType = GL_DYNAMIC_DRAW);

	void storeDataInAttributeList(const Size&& dataSize, const GLvoid* data, const Index&& location = 0, const Size&& attributeSize = 3);
private:
	Index createID();
};//VAO

}//gfx
}//mc