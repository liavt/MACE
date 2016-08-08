#pragma once

#include <MC-Graphics/Model.h>
#include <MC-Graphics/Entity.h>
#include <MC-Graphics/Shaders.h>
#include <MC-Graphics/GLUtil.h>

namespace mc {
namespace gfx {

	class Entity2D : public GraphicsEntity {
	public:
		Entity2D();
		Entity2D(const float& width, const float& height);

		float& getWidth();
		const float& getWidth() const;
		void setWidth(const float& s);

		float& getHeight();
		const float& getHeight() const;
		void setHeight(const float& s);
	protected:
		float width = 0, height = 0;
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

		Index boundTexture=-1, boundModel=-1, boundProgram=-1;

		float boundPaintStrength = -1.0f, boundWidth = -1.0f, boundHeight = -1.0f;

		Color& boundPaint = Color(-1.0f,-1.0f,-1.0f,-1.0f);

		TransformMatrix& boundTransformation = TransformMatrix();

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