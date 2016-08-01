/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MC-System/System.h>
#include <MC-Window/WindowModule.h>
#include <MC-Graphics/Entity.h>
#include <array>
#include <GL/glew.h>

namespace mc {

	/**
	Namespace containing classes used for Graphics in MACE.
	*/
	namespace gfx{

		void checkGLError();
		void throwShaderError(const Index& shaderId,const GLenum& type);

		struct RawModel {
			Index vaoID;
			Size vertexNumber;

			//RawModel(Index vaoID);
		};

		class ShaderProgram {
			Index id;
			Index fragId, vertId;

			Index createShader( const std::string& code, const GLenum& type);

		public:
			ShaderProgram();
			~ShaderProgram();

			void init();
			void bind();
			void unbind();

			void createFragment(const std::string& shader);
			void createVertex(const std::string& shader);

		};

		class Loader {
			//prevent initialization
			Loader();

			static Index createVAO();

			static void storeDataInAttributeList(const Index& attributeNumber, const Size& verticeSize, const GLfloat data[]);

		public:

			static void bindVAO(const Index& id);
			static void unbindVAO();

			static RawModel load(const Size& verticeSize, const GLfloat vertices[]);

			static void destroy();
		};

		class Renderer {
			//prevent intializations
			Renderer();
		public:
			static void init();

			static void render(const Entity& entity);

			static void destroy();
		};

		class OpenGLContext : public mc::gfx::Container, public mc::win::GraphicsContext {
			SDL_GLContext context;
		public:
			OpenGLContext();

			void update();

			void init(win::Window* win);
			void render(win::Window* win);
			void destroy(win::Window* win);
		};
	}
}

