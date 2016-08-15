#pragma once

#include <MC-Graphics/GraphicsEntity.h>
#include <MC-Graphics/Shaders.h>

#define _MACE_ENTITY2D_UNIFORM_VALUES \
	_MACE_ENTITY2D_UNIFORM_ENTRY(opacity,float) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(paint,Color) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(scale,Vector3f) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(rotation, Matrix4f) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(translation, Vector3f)


namespace mc {
namespace gfx {

class Renderer {
public:
	static void init();

	static void prepare();

	static void resize(const Size width, const Size height);

	static void draw(Entity2D* e);

	static void destroy();
private:
	const static char* vertexShader2D;
	const static char* fragmentShader2D;

	const static GLfloat squareVertices[12];

	const static GLfloat squareTextureCoordinates[8];

	const static GLuint squareIndices[6];

#define _MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
	static type a##CurrentlyBound;

	_MACE_ENTITY2D_UNIFORM_VALUES
#undef	_MACE_ENTITY2D_UNIFORM_ENTRY

	static ShaderProgram shaders2D;
	static VAO square;
};//Renderer

}//gfx
}//mc

#ifndef _MACE_ENTITY2D_EXPOSE_X_MACRO
#undef _MACE_ENTITY2D_UNIFORM_VALUES
#endif