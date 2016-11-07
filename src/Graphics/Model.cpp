/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/GraphicsConstants.h>

namespace mc {
namespace gfx {


VertexArray::~VertexArray()
{
}

void VertexArray::bind()
{
	glBindVertexArray(vaoID);
}
void VertexArray::unbind()
{
	glBindVertexArray(0);
}
void VertexArray::draw(GLenum type)
{
	glDrawElements(type, indiceNumber, GL_UNSIGNED_INT,0);
}

void VertexArray::loadVertices(const Size && verticeSize, const GLfloat vertices[], const Index && location, const Size && attributeSize)
{
	vaoID = createID();
	vertexNumber = verticeSize;

	bind();
	storeDataInAttributeList(std::move(vertexNumber), vertices, std::move(location), std::move(attributeSize));
}

void VertexArray::loadIndices(const Size&& indiceNum, const GLuint indices[], const GLenum&& drawType)
{
	bind();
	Index indicesID;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &indicesID);//
								// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//
														// Give our vertices to OpenGL.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(indiceNum), indices, drawType);//

																								//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//

	gfx::checkGLError();//

	this->indiceNumber = indiceNum;

	unbind();
}
Index VertexArray::createID()
{
	Index id;
	glGenVertexArrays(1, &id);
	checkGLError();
	return id;
}
void VertexArray::storeDataInAttributeList(const Size&& dataSize, const GLvoid* data, const Index&& location , const Size&& attributeSize)
{
	bind();

	Index vboID;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vboID);//
							// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboID);//
											// Give our data to opengl
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(attributeSize * dataSize), data, GL_DYNAMIC_DRAW);//
	gfx::checkGLError();//

	glVertexAttribPointer(location, attributeSize, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(location);


	//	glBindBuffer(GL_ARRAY_BUFFER, vboID);//
	checkGLError();
}



}//gfx
}//mc