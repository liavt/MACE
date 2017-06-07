/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_RENDERER_H
#define MACE_GRAPHICS_RENDERER_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Graphics/Window.h>
#include <deque>


namespace mc {
	//forward defining to prevent inclusion of header file
	class Preprocessor;

	namespace gfx {

		//if the container we use is ever going to be changed, we typedef
		using RenderQueue = std::deque<std::pair<Index, GraphicsEntity*>>;

		//forward define for friends
		class Renderer;

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
		};

		//we declare RenderImpl which RenderProtocol extends. WE can't store a pointer to RenderProtocol (because its templated), but we can point to RenderImpl
		/**
		@internal
		*/
		class RenderImpl {
			friend class Renderer;
			//ssl::init needs to be friends to access the index variable
			friend void ssl::init(const Size&, const Size&);
		public:
			RenderImpl() noexcept = default;
			virtual ~RenderImpl() noexcept = default;

			/**
			@internal
			@opengl
			*/
			virtual void resize(const Size, const Size) {};

			/**
			@internal
			@opengl
			*/
			virtual void init(const Size, const Size) = 0;
			/**
			@internal
			@opengl
			*/
			virtual void initEntity(GraphicsEntity*) {};

			/**
			@internal
			@opengl
			*/
			virtual void setUp(os::WindowModule*, RenderQueue*) {};
			/**
			@internal
			@opengl
			*/
			virtual void renderEntity(os::WindowModule*, GraphicsEntity* entity) = 0;
			/**
			@internal
			@opengl
			*/
			virtual void tearDown(os::WindowModule*, RenderQueue*) {};

			/**
			@internal
			@opengl
			*/
			virtual void cleanEntity(GraphicsEntity*) {};

			/**
			@internal
			@opengl
			*/
			virtual void destroy() = 0;
			/**
			@internal
			@opengl
			*/
			virtual void destroyEntity(GraphicsEntity*) {};
		};

		/**
		@internal
		@opengl
		*/
		template<typename T>
		class RenderProtocol: public RenderImpl {
		public:
			using RenderImpl::RenderImpl;

			void resize(const Size, const Size) override {};

			void init(const Size, const Size) override {}
			void initEntity(GraphicsEntity*) override {};

			void setUp(os::WindowModule*, RenderQueue*) override {};
			void renderEntity(os::WindowModule*, GraphicsEntity*) override {};
			void tearDown(os::WindowModule*, RenderQueue*) override {};

			void cleanEntity(GraphicsEntity*) override {};

			void destroy() override {};
			void destroyEntity(GraphicsEntity*) override {};
		};

		Renderer* getRenderer();
		void setRenderer(Renderer* r);

		/**
		@todo add function to change how many samples msaa uses
		*/
		class Renderer{
			//needs to access registerProtocol
			friend void ssl::init(const Size&, const Size&);
			friend void ssl::bindEntity(const GraphicsEntity* en, ogl::ShaderProgram& prog);
			friend void ssl::checkInput(os::WindowModule* win);
		public:

			/**
			@internal
			*/
			void flagResize();
			/**
			@internal
			@opengl
			*/
			virtual void resize(const Size width, const Size height);

			/**
			@internal
			@opengl
			*/
			virtual void init(const Size originalWidth, const Size originalHeight);

			/**
			@internal
			@opengl
			*/
			virtual void initEntity(GraphicsEntity* en, const Index protocol);

			/**
			@internal
			@opengl
			*/
			virtual void setUp(os::WindowModule* win);

			void queue(GraphicsEntity* e, const Index protocol);

			Size numberOfProtocols();

			/**
			@internal
			@opengl
			*/
			virtual void tearDown(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			virtual void renderFrame(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			virtual void checkInput(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			virtual void cleanEntity(GraphicsEntity* en, const Index protocol);

			/**
			@internal
			@opengl
			*/
			virtual void destroy();
			/**
			@internal
			@opengl
			*/
			virtual void destroyEntity(GraphicsEntity* en, const Index protocol);

			/**
			@internal
			@opengl
			*/
			virtual void clearBuffers();

			/**
			@opengl
			*/
			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f);
			/**
			@opengl
			*/
			void setRefreshColor(const Color& c);

			template<typename T>
			Index registerProtocol() {
				//in destroy(), this memory is deleted
				RenderImpl* protocol = new RenderProtocol<T>();
				if( protocol == nullptr ) {
					throw InvalidTypeError("Error creating RenderProtocol because template does not exist for it");
				}
				protocol->init(getOriginalWidth(), getOriginalHeight());
				pushProtocol(protocol);
				return numberOfProtocols() - 1;
			}

			Size getOriginalWidth() const;
			Size getOriginalHeight() const;

			Size getWidth() const;
			Size getHeight() const;

			Size getSamples() const;

			Vector<float, 2> getWindowRatios() const;

			RenderQueue getRenderQueue() const;

			Index getEntityIndex() const;

			bool isResized() const;
		protected:
			void pushEntity(Index protocol, GraphicsEntity*  entity);
			void pushProtocol(RenderImpl* protocol);

			virtual void generateFramebuffer(const Size& width, const Size& height);

			RenderQueue renderQueue = RenderQueue();
			std::vector<RenderImpl*> protocols = std::vector<RenderImpl*>();

			Size originalWidth = 0;
			Size originalHeight = 0;

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;
		private:
			//this variable is used for both ssl and Renderer. Each iteration through the queue, this is incremented. It is then passed to the shader, and the shader returns which entity was hovered over
			Index entityIndex = 0;
		};//Renderer

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
