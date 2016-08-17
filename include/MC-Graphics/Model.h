#pragma once
#include <MC-System/Constants.h>
#include <MC-System/Utility/Color.h>
#include <GL/GLEW.h>


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
};//VBO

class Texture {
public:

	Texture();
	Texture(Index id);
	Texture(const char* file);
	Texture(const GLvoid * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

	void setData(const GLvoid * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

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
private:
	Index id = 0;
	GLenum target = GL_TEXTURE_2D;

	Color paint = Color(1.0f, 1.0f, 1.0f, 1.0f);

	float opacity = 1.0f;

	void generateTexture();
};//Texture

class UBO {
public:
	~UBO();

	void bind() const;
	void unbind() const;

	void setData(const GLsizeiptr dataSize, const GLvoid* data, const GLenum drawType = GL_DYNAMIC_DRAW) const;

	Index getID() const;

	GLvoid* map() const;
	void unmap() const;

	void init();

	void bindBufferBase(const Index location);

	void bindUniform(const Index programID, const Index uniformID) const;

private:
	Index id;
	Index location;
};//UBO

}//gfx
}//mc