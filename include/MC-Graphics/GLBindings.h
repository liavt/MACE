#pragma once
#include <MC-System/Constants.h>
#include <MC-System/Utility/Color.h>
#include <GL/glew.h>

namespace mc{
	namespace gfx{
		class RawModel {
		public:
			Index vaoID;
			Size vertexNumber;

			void bind();
			void unbind();

			void destroy();

			void load(const Size& verticeSize, const GLfloat vertices[]);


			//RawModel(Index vaoID);
		private:
			Index createVAO();
			void storeDataInAttributeList(const Index& attributeNumber, const Size& verticeSize, const GLfloat data[]);
		};

		class ShaderProgram {
			Index id = -1;
			Index fragId = -1, vertId = -1;

			Index createShader(const char code[], const GLenum& type);

			void createProgram();
		public:
			ShaderProgram();
			~ShaderProgram();

			void init();
			void bind();
			void unbind();

			void createFragment(const char shader[]);
			void createVertex(const char shader[]);

			void createUniform(const char name[]);
			void setUniform(const char name[], mc::Color value);

		};
	}
}