#pragma once

#include <MC-Graphics/GraphicsEntity.h>
#include <MC-Graphics/Shaders.h>

#define MACE_ENTITY2D_UNIFORM_VALUES \
	MACE_ENTITY2D_UNIFORM_ENTRY(opacity,float) \
	MACE_ENTITY2D_UNIFORM_ENTRY(paint,Color) \
	MACE_ENTITY2D_UNIFORM_ENTRY(scale,Vector3f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(rotation, Matrix4f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(translation, Vector3f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(inheritedScale,Vector3f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(inheritedRotation,Matrix4f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(inheritedTranslation,Vector3f) \


namespace mc {
namespace gfx {

class Renderer {
public:
	static void init();

	static void prepare();

	static void resize(const Size width, const Size height);

	static void draw(Entity2D* e);

	static void destroy();

	static void setRefreshColor(const float r, const float g, const float b, const float a=1.0f);
	static void setRefreshColor(const Color& c);

private:
	const static char* vertexShader2D;
	const static char* fragmentShader2D;

	const static GLfloat squareVertices[12];

	const static GLfloat squareTextureCoordinates[8];

	const static GLuint squareIndices[6];

#define MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
	static type a##CurrentlyBound;

	MACE_ENTITY2D_UNIFORM_VALUES
#undef	MACE_ENTITY2D_UNIFORM_ENTRY

	static ShaderProgram shaders2D;
	static VAO square;
};//Renderer

}//gfx
}//mc

#ifndef MACE_ENTITY2D_EXPOSE_X_MACRO
#undef MACE_ENTITY2D_UNIFORM_VALUES
#endif