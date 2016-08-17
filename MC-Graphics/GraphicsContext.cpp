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
#include <MC-System/Exceptions.h>
#include <MC-Graphics/GLUtil.h>
#include <MC-Graphics/Renderer.h>
#include <GL/glew.h>

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
}

void OpenGLContext::init(win::Window * win)
{

	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwMakeContextCurrent(win->getGLFWWindow());

	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		throw mc::InitializationError("GLEW failed to initialize with result " + std::to_string(result));
	}

	if (GLEW_VERSION_1_1)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		std::cout << "OpenGL has been created succesfully!" << std::endl;
		std::cout << "Version: " << std::endl << "	" << glGetString(GL_VERSION) << std::endl;
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

	if (vsync)glfwSwapInterval(1);
	else glfwSwapInterval(0);

	Renderer::init();

	initChildren();


}

void OpenGLContext::render(win::Window* win) {
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer::prepare();

	renderChildren();

	glfwSwapBuffers(win->getGLFWWindow());
}

void OpenGLContext::destroy(win::Window* win) {
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	destroyChildren();

	Renderer::destroy();

}

void OpenGLContext::resize(win::Window * win)
{
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	int width, height;

	glfwGetWindowSize(win->getGLFWWindow(), &width, &height);

	Renderer::resize(width,height);
}

void OpenGLContext::setVSync(const bool & sync)
{
	vsync = sync;
}

}//gfx
}//mc