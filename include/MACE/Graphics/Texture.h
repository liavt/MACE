#pragma once
#include <MACE/System/Constants.h>
#include <MACE/System/Utility/Color.h>
#include <GL/glew.h>

namespace mc {
namespace gfx {

class Texture {
public:

	Texture();
	Texture(Index id);
	Texture(const char* file);
	Texture(const GLvoid * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

	void setData(const void * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

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


}//gfx
}//mc