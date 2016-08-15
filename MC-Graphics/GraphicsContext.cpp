/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Graphics/GraphicsContext.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <GL/GLEW.h>
#include <MC-System/Exceptions.h>
#include <MC-Graphics/GLUtil.h>
#include <MC-Graphics/Renderer.h>

namespace mc {
namespace gfx{

OpenGLContext::OpenGLContext() : Container(),GraphicsContext()
{

}
void OpenGLContext::update() {
//	SDL_GL_MakeCurrent(window->getSDLWindow(), context);

	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	updateChildren();

	SDL_Delay(10);
}

void OpenGLContext::init(win::Window * win)
{

	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	context = SDL_GL_CreateContext(win->getSDLWindow());
	SDL_GL_MakeCurrent(win->getSDLWindow(),context);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		throw mc::InitializationError("GLEW failed to initialize with result "+std::to_string(result));
	}

	if (GLEW_VERSION_1_1)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		std::cout << "OpenGL has been created succesfully!" << std::endl;
		std::cout << "Version: "<<std::endl<<"	"<<glGetString(GL_VERSION) << std::endl;
		std::cout << "Vendor: " << std::endl << "	" << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << std::endl << "	" << glGetString(GL_RENDERER) << std::endl;
		std::cout << "Shader version: " << std::endl << "	" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}
	else
	{
		throw InitializationError("Error retrieving GLEW version!");
	}

	if (!GLEW_VERSION_3_1)
	{
		throw InitializationError("OpenGL 3 is not available!");
	}

	checkGLError();

	result = SDL_GL_SetSwapInterval(vsync);
	if (result == -1)std::cerr << SDL_GetError();

	Renderer::init();

	initChildren();


}

void OpenGLContext::render(win::Window* win) {
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer::prepare();

	renderChildren();

	SDL_GL_SwapWindow(win->getSDLWindow());
}

void OpenGLContext::destroy(win::Window* win) {
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	destroyChildren();

	Renderer::destroy();

	SDL_GL_DeleteContext(context);
}

void OpenGLContext::resize(win::Window * win)
{
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	int width, height;

	SDL_GetWindowSize(win->getSDLWindow(), &width, &height);

	Renderer::resize(width,height);
}

void OpenGLContext::setVSync(const bool & sync)
{
	vsync = sync;
}

}//gfx
}//mc