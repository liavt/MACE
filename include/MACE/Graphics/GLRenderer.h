/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_GLRENDERER_H
#define MACE_GRAPHICS_GLRENDERER_H

#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL.h>

namespace mc {
	//forward defining to prevent inclusion of header file
	class Preprocessor;

	namespace gfx {
		/**
		Holds resources for the Standard Shader Library (SSL.) SSL makes it easy to interact with MACE entities from shaders through special header files.
		@internal
		*/
		namespace ssl {
			/**
			@internal
			@opengl
			*/
			void init(const Size& originalWidth, const Size& originalHeight);
			/**
			@internal
			@opengl
			*/
			void setUp(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void tearDown(os::WindowModule* win);
			/**
			@internal
			@opengl
			*/
			void destroy();
			/**
			@internal
			@opengl
			*/
			void resize(const Size& width, const Size& height);

			/**
			@internal
			@opengl
			*/
			void checkInput(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void bindBuffer(ogl::UniformBuffer& buf);
			/**
			@internal
			@opengl
			*/
			void fillBuffer(GraphicsEntity* en);

			/**
			@internal
			@opengl
			*/
			void bindEntity(const GraphicsEntity* en, ogl::ShaderProgram& prog);
			/**
			@internal
			@opengl
			*/
			void bindShaderProgram(ogl::ShaderProgram& prog);

			/**
			@internal
			@opengl
			*/
			void setRefreshColor(const float r, const float g, const float b, const float a);

			/**
			@internal
			*/
			std::string processShader(const std::string& shader, const GLenum& type);
			/**
			@internal
			*/
			const mc::Preprocessor& getSSLPreprocessor();
		}

		class GLRenderer: public Renderer {
			//needs to access registerProtocol
			friend void ssl::init(const Size&, const Size&);
			friend void ssl::bindEntity(const GraphicsEntity* en, ogl::ShaderProgram& prog);
			friend void ssl::checkInput(os::WindowModule* win);
		public:
			~GLRenderer() noexcept = default;

			void onResize(const Size width, const Size height) override;
			void onInit(const Size originalWidth, const Size originalHeight) override;
			void onSetUp(os::WindowModule* win) override;
			void onTearDown(os::WindowModule* win) override;
			void onDestroy() override;
			void onInputCheck(os::WindowModule* win) override;

			void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;
		private:
			void generateFramebuffer(const Size& width, const Size& height);
		};

		class SimpleQuadRenderer {
		public:
			SimpleQuadRenderer(const bool ssl = true);

			/**
			@opengl
			*/
			void init(const char* vertexShader, const char* fragShader);
			/**
			@copydoc SimpleQuadRenderer::init(const char*, const char*)
			*/
			void init(const char* vertexShader, const std::string& fragShader);
			/**
			@copydoc SimpleQuadRenderer::init(const char*, const char*)
			*/
			void init(const std::string& vertexShader, const char* fragShader);
			/**
			@copydoc SimpleQuadRenderer::init(const char*, const char*)
			*/
			void init(const std::string& vertexShader, const std::string& fragShader);
			/**
			@opengl
			*/
			void destroy();

			/**
			@opengl
			*/
			void bind() const;

			/**
			@opengl
			*/
			void draw(const GraphicsEntity* en = nullptr);

			void setShader(const ogl::ShaderProgram& shader);
			ogl::ShaderProgram& getShader();
			const ogl::ShaderProgram& getShader() const;

			void setVertexArray(const ogl::VertexArray& vertices);
			ogl::VertexArray& getVertexArray();
			const ogl::VertexArray& getVertexArray() const;

			bool operator==(const SimpleQuadRenderer& other) const;
			bool operator!=(const SimpleQuadRenderer& other) const;
		private:
			ogl::ShaderProgram shaders2D = ogl::ShaderProgram();
			ogl::VertexArray square = ogl::VertexArray();

			const bool useSSL;
		};
	}//gfx
}//mc

#endif
