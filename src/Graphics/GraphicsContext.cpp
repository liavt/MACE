/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/GraphicsContext.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <MACE/System/Exceptions.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/Renderer.h>
#include <GL/glew.h>

namespace mc {
namespace gfx{

OpenGLContext::OpenGLContext() : Entity(),GraphicsContext()
{

}
void OpenGLContext::update() {
//	SDL_GL_MakeCurrent(window->getSDLWindow(), context);

	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	Entity::update();
}

void OpenGLContext::setUpWindow(win::Window * win)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void OpenGLContext::init(win::Window * win)
{

	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	glfwMakeContextCurrent(win->getGLFWWindow());

	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		throw mc::InitializationError("GLEW failed to initialize with result " + std::to_string(result));
	}

	if (GL_VERSION_1_1)
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

	if (!GL_VERSION_3_0)
	{
		throw InitializationError("OpenGL 3+ is not available!");
	}else if(!GL_VERSION_3_3){
        std::cout<<"OpenGL 3.3 not found, falling back to OpenGL 3.0.";
	}

	checkGLError();

	if (vsync)glfwSwapInterval(1);
	else glfwSwapInterval(0);


	auto framebufferResize = [](GLFWwindow* win, int width, int height) {
		Renderer::resize(width, height);
	};
	glfwSetFramebufferSizeCallback(win->getGLFWWindow(), framebufferResize);

	int width = 0, height = 0;

	glfwGetFramebufferSize(win->getGLFWWindow(), &width, &height);

	Renderer::init(width, height);

	Entity::init();


}

void OpenGLContext::render(win::Window* win) {
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Entity::render();

	Renderer::renderFrame(win);

	glfwSwapBuffers(win->getGLFWWindow());
}

void OpenGLContext::destroy(win::Window* win) {
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);

	Entity::destroy();

	Renderer::destroy();

}

void OpenGLContext::setVSync(const bool & sync)
{
	vsync = sync;
}

void OpenGLContext::customUpdate()
{
}

void OpenGLContext::customRender()
{
}

void OpenGLContext::customDestroy()
{
}

void OpenGLContext::customInit()
{
}

}//gfx
}//mc
