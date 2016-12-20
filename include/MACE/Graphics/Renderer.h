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
			void checkInput();

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
			virtual void resize(const Size width, const Size height) = 0;

			/**
			@internal
			@opengl
			*/
			virtual void init(const Size originalWidth, const Size originalHeight) = 0;
			/**
			@internal
			@opengl
			*/
			virtual void setUp(os::WindowModule* win, RenderQueue* queue) = 0;
			/**
			@internal
			@opengl
			*/
			virtual void render(os::WindowModule* win, GraphicsEntity* entity) = 0;
			/**
			@internal
			@opengl
			*/
			virtual void tearDown(os::WindowModule* win, RenderQueue* queue) = 0;
			/**
			@internal
			@opengl
			*/
			virtual void destroy() = 0;
		private:
			static int index;
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
			void setUp(os::WindowModule*, RenderQueue*) override {};
			void render(os::WindowModule*, GraphicsEntity*) override {};
			void tearDown(os::WindowModule*, RenderQueue*) override {};
			void destroy() override {};
		};


		class Renderer final{
			//needs to access registerProtocol
			friend void ssl::init(const Size&, const Size&);
		public:
			/**
			@internal
			@opengl
			*/
			static void resize(const Size width, const Size hieght);

			/**
			@internal
			@opengl
			*/
			static void init(const Size originalWidth, const Size originalHeight);

			/**
			@internal
			@opengl
			*/
			static void setUp(os::WindowModule* win);

			template<typename T>
			static void queue(T* e) {
				if( e == nullptr || e == NULL )throw NullPointerException("Input pointer to an entity must not be null in queue()");
				if( RenderProtocol<T>::index == -1 )registerProtocol<T>();
				pushEntity(RenderProtocol<T>::index, e);
			};

			static Size numberOfProtocols();

			/**
			@internal
			@opengl
			*/
			static void tearDown(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			static void renderFrame(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			static void checkInput();

			/**
			@internal
			@opengl
			*/
			static void destroy();

			/**
			@internal
			@opengl
			*/
			static void clearBuffers();

			/**
			@opengl
			*/
			static void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f);
			/**
			@opengl
			*/
			static void setRefreshColor(const Color& c);

			static Size getOriginalWidth();
			static Size getOriginalHeight();
		private:
			Renderer() = delete;
			~Renderer() = delete;

			static void pushEntity(Index protocol, GraphicsEntity*  entity);
			static void pushProtocol(RenderImpl* protocol);

			template<typename T>
			static void registerProtocol() {
				//in destroy(), this memory is deleted
				RenderImpl* protocol = new RenderProtocol<T>();
				protocol->init(getOriginalWidth(), getOriginalHeight());
				pushProtocol(protocol);
				RenderProtocol<T>::index = numberOfProtocols() - 1;
			}
		};//Renderer

	}//gfx
}//mc

#endif