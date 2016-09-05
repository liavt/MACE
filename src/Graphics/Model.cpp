#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/GraphicsConstants.h>

namespace mc {
namespace gfx {


VAO::~VAO()
{
}

void VAO::bind()
{
	glBindVertexArray(vaoID);
}
void VAO::unbind()
{
	glBindVertexArray(0);
}
void VAO::draw(GLenum type)
{
	glDrawElements(type, indiceNumber, GL_UNSIGNED_INT,0);
}
void VAO::loadVertices(const Size & verticeSize, const GLfloat vertices[])
{
	vaoID = createID();
	vertexNumber = verticeSize;


	bind();
	storeDataInAttributeList(vertices, 0, 3, vertexNumber);
	unbind();
}

void VAO::loadTextureCoordinates(const Size & verticeSize, const GLfloat vertices[])
{
	bind();
	storeDataInAttributeList(vertices, 1, 2, vertexNumber);
	unbind();
}

void VAO::loadIndices(const Size & indiceNum, const GLuint indices[])
{
	bind();
	Index indicesID;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &indicesID);//
								// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//
														// Give our vertices to OpenGL.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(indiceNum), indices, GL_DYNAMIC_DRAW);//

																								//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//

	gfx::checkGLError();//

	this->indiceNumber = indiceNum;

	unbind();
}
Index VAO::createID()
{
	Index id;
	glGenVertexArrays(1, &id);
	checkGLError();
	return id;
}
void VAO::storeDataInAttributeList(const GLfloat data[], const Index & attributeNumber, const Size& attributeSize, const Size & verticeSize)
{
	Index vboID;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vboID);//
							// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboID);//
											// Give our data to opengl
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(attributeSize * verticeSize), data, GL_DYNAMIC_DRAW);//
	gfx::checkGLError();//

	glVertexAttribPointer(attributeNumber, attributeSize, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attributeNumber);


	//	glBindBuffer(GL_ARRAY_BUFFER, vboID);//
	checkGLError();
}



}//gfx
}//mc