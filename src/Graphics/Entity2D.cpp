/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/GLUtil.h>

namespace mc {
namespace gfx {


Entity2D::Entity2D() : GraphicsEntity()
{
}

void Image::customInit()
{
}

void Image::customUpdate()
{
}

void Image::customRender()
{
	Renderer::queue<Entity2D>(this);
}

void Image::customDestroy()
{
}

void RenderProtocol<Entity2D>::resize(const Size width, const Size height) {
}

void RenderProtocol<Entity2D>::init(const Size originalWidth, const Size originalHeight) {

	//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
	//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
	const char* vertexShader2D = {
#	include <MACE/Graphics/Shaders/entity2D.v.glsl>
	};
	const char* fragmentShader2D = {
#	include <MACE/Graphics/Shaders/entity2D.f.glsl>
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

	//vao loading
	square.loadVertices(4, squareVertices, 15, 3);
	square.storeDataInAttributeList(4, squareTextureCoordinates, 1, 2);
	square.loadIndices(6, squareIndices);

	//shader stuff
	shaders2D.createVertex(ssl::processShader(vertexShader2D,GL_VERTEX_SHADER));
	shaders2D.createFragment(ssl::processShader(fragmentShader2D,GL_FRAGMENT_SHADER));

	shaders2D.init();

	ssl::bindShaderProgram(shaders2D);
}

void RenderProtocol<Entity2D>::setUp(win::Window* win, RenderQueue* queue)
{
};

void RenderProtocol<Entity2D>::render(win::Window* win, void* e) {
	Entity2D * entity = reinterpret_cast<Entity2D*>(e);

	square.bind();
	shaders2D.bind();

	ssl::bindEntity(entity);

	square.draw();

	checkGLError();
}

void RenderProtocol<Entity2D>::tearDown(win::Window* win, RenderQueue* queue) {}

void RenderProtocol<Entity2D>::destroy() {
	shaders2D.destroy();
}

}//gfx
}//mc
