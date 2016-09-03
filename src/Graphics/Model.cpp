#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/GraphicsConstants.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

void Texture::generateTexture()
{
	glGenTextures(1, &id);

	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	unbind();

	checkGLError();
}

Texture::Texture()
{
	id = 0;
}

Texture::Texture(Index id)
{
	this->id = id;
}

Texture::Texture(const char * file)
{
	generateTexture();
	bind();
	loadFile(file);
	unbind();
}

Texture::Texture(const GLvoid * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel)
{
	generateTexture();
	bind();
	setData(data, width, height, type, format, internalFormat, mipmapLevel);
	unbind();
}

void Texture::setData(const GLvoid * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel)
{
	glTexImage2D(target, mipmapLevel, internalFormat, width, height, 0, format, type, data);

	checkGLError();
}

void Texture::loadFile(const char * file)
{
	int width, height, componentSize;

	Byte* image = stbi_load(file, &width, &height, &componentSize, STBI_rgb_alpha);

	setData(image, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);

	stbi_image_free(image);
}

void Texture::setTarget(GLenum target)
{
	this->target = target;
}

GLenum Texture::getTarget()
{
	return target;
}

int Texture::getID() const
{
	return id;
}

void Texture::bind() const
{
	glBindTexture(target, id);
}

void Texture::unbind() const
{
	glBindTexture(target, 0);
}

Color & Texture::getPaint()
{
	return paint;
}

const Color & Texture::getPaint() const
{
	return paint;
}

void Texture::setPaint(const Color & c)
{
	paint = c;
}

float Texture::getOpacity()
{
	return opacity;
}

const float Texture::getOpacity() const
{
	return opacity;
}

void Texture::setOpacity(const float f)
{
	opacity = f;
}

}//gfx
}//mc