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

	void loadVertices(const Size& verticeSize, const GLfloat vertices[]);
	void loadTextureCoordinates(const Size& verticeSize, const GLfloat vertices[]);
	void loadIndices(const Size& indiceNum, const GLuint indices[]);

	void storeDataInAttributeList(const GLfloat data[], const Index& attributeNumber, const Size& attributeSize, const Size& verticeSize);
private:
	Index createID();
};//VAO

}//gfx
}//mc