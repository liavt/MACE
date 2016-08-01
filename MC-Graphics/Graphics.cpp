/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Graphics/Graphics.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <GL/GLEW.h>
#include <MC-System/Exceptions.h>

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

			lock.unlock();

			SDL_Delay(10);
		}


		void OpenGLContext::init(win::Window * win)
		{
			context = SDL_GL_CreateContext(win->getSDLWindow());
			SDL_GL_MakeCurrent(win->getSDLWindow(),context);

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

			const GLenum result = glewInit();
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

			Renderer::init();

			initChildren();

		}

		void OpenGLContext::render(win::Window* win) {
			glClear(GL_COLOR_BUFFER_BIT);

			renderChildren();

			SDL_GL_SwapWindow(win->getSDLWindow());
		}

		void OpenGLContext::destroy(win::Window* win) {
			destroyChildren();
			Renderer::destroy();
			Loader::destroy();
			SDL_GL_DeleteContext(context);
		}
		void Loader::storeDataInAttributeList(const Index& attributeNumber, const Size& componentSize, const GLfloat data[])
		{
			
			Index vboID;
			glGenBuffers(1, &vboID);

			glBindBuffer(GL_ARRAY_BUFFER, vboID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

			checkGLError();
		}
		Index Loader::createVAO()
		{
			Index id;
			glGenVertexArrays(1, &id);
			checkGLError();
			return id;
		}
		void Loader::bindVAO(const Index& id)
		{
			glBindVertexArray(id);

		}
		RawModel Loader::load(const Size& verticeSize, const GLfloat vertices[])
		{

			Index VertexArrayID, vertexbuffer;

			glGenVertexArrays(1, &VertexArrayID);//
			glBindVertexArray(VertexArrayID);//

											 // Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &vertexbuffer);//
										   // The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);//
														// Give our vertices to OpenGL.
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//
			gfx::checkGLError();//

			glBindVertexArray(0);

			return	{VertexArrayID,3};
			/*
			RawModel out = RawModel();
			out.vaoID = createVAO();
			out.vertexNumber = verticeSize;
			bindVAO(out.vaoID);
			storeDataInAttributeList(0, out.vertexNumber, vertices);
			unbindVAO();
			return	out;*/
		}
		void Loader::unbindVAO()
		{
			glBindVertexArray(0);
		}
		void Loader::destroy()
		{
			/*
			for (Index i : vbos) {
				glDeleteBuffers(i);
			}
			for (Index i : vaos) {
				glDeleteVertexArrays(i);
			}*/

		}
		void Renderer::init()
		{
		}
		void Renderer::destroy()
		{
		}
		Index ShaderProgram::createShader( const std::string& code, const  GLenum& type)
		{
			const Index shaderId = glCreateShader(type);

			checkGLError();

			if (shaderId == 0) {
				throwShaderError(shaderId,type);
			}

			glShaderSource(shaderId, 1,(const GLchar**)&code,0);
			glCompileShader(shaderId);

			GLint result = -1;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
			if (result == 0) {
				throwShaderError(shaderId,type);
			}

			glAttachShader(id, shaderId);

			return shaderId;
		}
		ShaderProgram::ShaderProgram()
		{
			id = glCreateProgram();
			checkGLError();
			if (id == 0) {
				throwShaderError(id, GL_PROGRAM);
			}
		}
		ShaderProgram::~ShaderProgram()
		{
			unbind();
			if (id != 0) {
				if (vertId != 0) {
					glDetachShader(id, vertId);
				}
				if (fragId != 0) {
					glDetachShader(id, fragId);
				}
				glDeleteProgram(id);
			}
		}
		void ShaderProgram::init()
		{
			glLinkProgram(id);

			GLint result = -1;
			glGetProgramiv(id, GL_LINK_STATUS, &result);
			if (result == 0) {
				throwShaderError(id,GL_PROGRAM);
			}

			checkGLError();

			glValidateProgram(id);

			glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
			if (result == 0) {
				throwShaderError(id, GL_PROGRAM);
			}
		}
		void ShaderProgram::bind()
		{
			glUseProgram(id);
		}
		void ShaderProgram::unbind()
		{
			glUseProgram(0);
		}
		void ShaderProgram::createFragment(const std::string & shader)
		{
			fragId = createShader(shader,GL_FRAGMENT_SHADER);
		}
		void ShaderProgram::createVertex(const std::string & shader)
		{
			vertId = createShader(shader, GL_VERTEX_SHADER);
		}
		void checkGLError()
		{
			GLenum result = GL_NO_ERROR;
			while ((result = glGetError()) != GL_NO_ERROR) {
				switch (result) {
				case GL_INVALID_ENUM:
					throw GLError("GL_INVALID_ENUM! An unacceptable value is specified for an enumerated argument!");
					break;
				case GL_INVALID_VALUE:
					throw GLError("GL_INVALID_VALUE! A numeric argument is out of range!");
					break;
				case GL_INVALID_OPERATION:
					throw GLError("GL_INVALID_OPERATION! The specified operation is not allowed in the current state!");
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					throw GLError("GL_INVALID_FRAMEBUFFER_OPERATION! The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE!)");
					break;
				case GL_STACK_OVERFLOW:
					throw GLError("GL_STACK_OVERFLOW! A stack pushing operation cannot be done because it would overflow the limit of that stack's size!");
					break;
				case GL_STACK_UNDERFLOW:
					throw GLError("GL_STACK_UNDERFLOW! A stack popping operation cannot be done because the stack is already at its lowest point.");
					break;
				case GL_OUT_OF_MEMORY:
					throw GLError("GL_OUT_OF_MEMORY! There is not enough memory left to execute the command!");
					break;
				default:
					throw GLError("OpenGL has errored with an error code of " + std::to_string(result));
					break;
				}
			}
		}
		void throwShaderError(const Index& shaderId,const GLenum& type)
		{
			GLchar* log_string = new char[126];
			glGetShaderInfoLog(shaderId, 126, 0, log_string);
			const std::string outMessage = "Error generating shader of type " + std::to_string(type) + " with code " + log_string;
			delete log_string;
			throw GLError(outMessage);
		}
}
}