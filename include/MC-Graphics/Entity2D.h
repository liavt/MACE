#pragma once

#include <MC-Graphics/Model.h>
#include <MC-Graphics/Entity.h>
#include <MC-Graphics/Shaders.h>
#include <MC-Graphics/GLUtil.h>

#define _MACE_ENTITY2D_UNIFORM_VALUES \
	_MACE_ENTITY2D_UNIFORM_ENTRY(opacity,float) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(paint,Color) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(scale,Vector3f) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(rotation, Matrix4f) \
	_MACE_ENTITY2D_UNIFORM_ENTRY(translation, Vector3f)


namespace mc {
namespace gfx {

	class Entity2D : public GraphicsEntity {
	public:
		Entity2D();
	protected:
	};

	class Renderer2D {
	public:
		Renderer2D();

		void init();

		void draw(Entity2D* e);

		void destroy();
	private:
		const static char* vertexShader2D;
		const static char* fragmentShader2D;

		const static GLfloat squareVertices[12];

		const static GLfloat squareTextureCoordinates[8];

		const static GLuint squareIndices[6];

		Index boundModel = 0;

		int boundTexture = -1, boundProgram = -1;

#define _MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
		type a##CurrentlyBound = type##();

		_MACE_ENTITY2D_UNIFORM_VALUES
#undef	_MACE_ENTITY2D_UNIFORM_ENTRY

		ShaderProgram shaders2D=ShaderProgram();
		RawModel square=RawModel();

		std::unordered_map<std::string, bool> lastUniforms = std::unordered_map<std::string, bool>();

	};

	template<>
	struct Renderer<Entity2D> {

		static void draw(Entity2D* e);

		static const Renderer2D getInstance();
	private:
		static bool isInit;

		static Renderer2D instance;
	};

	inline void Renderer<Entity2D>::draw(Entity2D * e)
	{
		if (!isInit) {
			isInit = true;
			instance.init();
		}

		instance.draw(e);
	}

	inline const Renderer2D Renderer<Entity2D>::getInstance()
	{
		return instance;
	}

}
}

#ifndef _MACE_ENTITY2D_EXPOSE_X_MACRO
#undef _UNIFORM_VALUES
#endif