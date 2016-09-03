#pragma once

#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/Renderer.h>

namespace mc {
namespace gfx {

	class Entity2D : public GraphicsEntity {
	public:
		Entity2D();
	protected:
	};//Entity2D

	class Image : public Entity2D {
	protected:
		void customInit();
		void customUpdate();
		void customRender();
		void customDestroy();

	};

	template<>
	class RenderProtocol<Entity2D> : public RenderImpl {
	public:

		void resize(const Size width, const Size height);

		void init();

		void setUp();

		void render(void* entity);

		void tearDown();

		void destroy();
	private:
		//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
		//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
		const char* vertexShader2D = {
#include "shaders/entity2D.vert"
		};
		const char* fragmentShader2D = {
#include "shaders/entity2D.frag"
		};

		const GLfloat squareVertices[12] = {
			-0.5f,-0.5f,0.0f,
			-0.5f,0.5f,0.0f,
			0.5f,0.5f,0.0f,
			0.5f,-0.5f,0.0f
		};

		const GLfloat squareTextureCoordinates[8] = {
			0.0f,1.0f,
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
		};

		const GLuint squareIndices[6] = {
			0,1,3,
			1,2,3
		};

		UBO paintData = UBO();
		UBO entityData = UBO();

		ShaderProgram shaders2D = ShaderProgram();
		VAO square = VAO();
	};

}//gfx
}//mc