/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/GraphicsConstants.h>

namespace mc {
	namespace gfx {


		Entity2D::Entity2D() : GraphicsEntity() {}

		void Image::customInit() {}

		void Image::customUpdate() {}

		void Image::customRender() {}

		void Image::customDestroy() {}

		void RenderProtocol<Entity2D>::resize(const Size, const Size) {}

		void RenderProtocol<Entity2D>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
		#	include <MACE/Graphics/Shaders/entity2D.v.glsl>
			};
			const char* fragmentShader2D = {
		#	include <MACE/Graphics/Shaders/entity2D.f.glsl>
			};

			const float squareVertices[12] = {
				-0.5f,-0.5f,0.0f,
				-0.5f,0.5f,0.0f,
				0.5f,0.5f,0.0f,
				0.5f,-0.5f,0.0f
			};

			const float squareTextureCoordinates[8] = {
				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,0.0f,
				1.0f,1.0f,
			};

			const unsigned int squareIndices[6] = {
				0,1,3,
				1,2,3
			};

			square.init();

			//vao loading
			square.loadVertices(4, squareVertices, 15, 3);
			square.storeDataInAttributeList(4, squareTextureCoordinates, 1, 2);
			
			square.loadIndices(6,squareIndices);

			//shader stuff
			shaders2D.createVertex(ssl::processShader(vertexShader2D, GL_VERTEX_SHADER));
			shaders2D.createFragment(ssl::processShader(fragmentShader2D, GL_FRAGMENT_SHADER));

			shaders2D.init();

			ssl::bindShaderProgram(shaders2D);
		}//init

		void RenderProtocol<Entity2D>::setUp(os::WindowModule*, RenderQueue*) {};

		void RenderProtocol<Entity2D>::render(os::WindowModule*, GraphicsEntity* e) {
			square.bind();
			shaders2D.bind();

			ssl::bindEntity(e, shaders2D);

			square.draw();

			checkGLError();
		}//render

		void RenderProtocol<Entity2D>::tearDown(os::WindowModule*, RenderQueue*) {}

		void RenderProtocol<Entity2D>::destroy() {
			shaders2D.destroy();
			square.destroy();
		}//destroy

	}//gfx
}//mc
