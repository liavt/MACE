#pragma once
#include <MC-System/Constants.h>
#include <MC-System/Utility/Color.h>
#include <GL/glew.h>


namespace mc {
namespace gfx {

class VBO {
public:
	VBO();
	~VBO();

	template<typename DataType,Size DataSize>
	void setData(const std::array<DataType,DataSize>& data, const Size componentSize = 1, const GLenum bufferType = GL_ARRAY_BUFFER, const GLenum readType = GL_STATIC_DRAW) {
		this->componentSize = componentSize;
		this->dataSize = DataSize;

		glBindBuffer(bufferType, bufferID);
		glBufferData(bufferType, sizeof(DataType)*(componentSize * DataSize), data.data(), readType);

		checkGLError();//
	}

	void setAttributeLayout(const Index attributeNumber,const GLenum dataType= GL_FLOAT,const bool normalized = GL_FALSE, const unsigned int stride=0,const GLvoid* pointer=0);

	bool isAttributeBuffer() const;

	int getAttributeID() const;
	Size getComponentSize() const;
	Size getDataSize() const;
	Index getBufferID() const;
private:
	int attributeID=-1;
	Index bufferID;
	Size componentSize;
	Size dataSize;
};

class VAO {
public:
	Index vaoID;
	Size vertexNumber, indiceNumber;

	~VAO();

	void bind();
	void unbind();

	void draw(GLenum type=GL_TRIANGLES);

	void loadVertices(const Size& verticeSize, const GLfloat vertices[]);
	void loadTextureCoordinates(const Size& verticeSize, const GLfloat vertices[]);
	void loadIndices(const Size& indiceNum, const GLuint indices[]);

	void storeDataInAttributeList(const GLfloat data[], const Index& attributeNumber, const Size& attributeSize, const Size& verticeSize);
private:
	Index createID();
};

class Texture {
public:

	Texture();
	Texture(Index id);
	Texture(const char* file);
	Texture(const GLvoid * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

	void setData(const GLvoid * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

	void loadFile(const char* file);

	void setTarget(GLenum target);
	GLenum getTarget();

	int getID() const;

	void bind() const;
	void unbind() const;

	Color& getPaint();
	const Color& getPaint() const;
	void setPaint(Color& c);

	float getOpacity();
	const float getOpacity() const;
	void setOpacity(float f);
private:
	Index id = 0;
	GLenum target = GL_TEXTURE_2D;

	Color paint = Color(1.0f, 1.0f, 1.0f, 1.0f);

	float opacity = 1.0f;

	void generateTexture();
};
}//gfx
}//mc