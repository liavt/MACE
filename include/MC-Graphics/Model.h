#pragma once
#include <MC-System/Constants.h>
#include <MC-System/Utility/Color.h>
#include <GL/glew.h>


namespace mc {
namespace gfx {

class RawModel {
	Index createVAO();
public:
	Index vaoID;
	Size vertexNumber, indiceNumber;

	~RawModel();

	void bind();
	void unbind();

	void draw(GLenum type=GL_TRIANGLES, Index offset=0);

	void destroy();

	void loadVertices(const Size& verticeSize, const GLfloat vertices[]);
	void loadTextureCoordinates(const Size& verticeSize, const GLfloat vertices[]);
	void loadIndices(const Size& indiceNum, const GLuint indices[]);

	void storeDataInAttributeList(const GLfloat data[], const Index& attributeNumber, const Size& attributeSize, const Size& verticeSize);

};

class Texture {
	Index id = 0;
	GLenum target = GL_TEXTURE_2D;

	Color paint = Color(1.0f,1.0f,1.0f,1.0f);

	float paintStrength=0.5f;

	void generateTexture();
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

	float& getPaintStrength();
	const float& getPaintStrength() const;
	void setPaintStrength(float& f);
};
}//gfx
}//mc