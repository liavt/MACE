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
#include <MACE/Graphics/Shaders.h>
#include <MACE/Graphics/Window.h>
#include <deque>


namespace mc {
	//forward defining to prevent inclusion of header file
	class Preprocessor;

	namespace gfx {

		//if the container we use is ever going to be changed, we typedef 
		using RenderQueue = std::deque<std::pair<Index, Entity*>>;

		//forward define for friends
		class Renderer;

		/**
		Holds resources for the Standard Shader Library (SSL.) SSL makes it easy to interact with MACE entities from shaders through special header files.
		*/
		namespace ssl {
			void init(const Size& originalWidth, const Size& originalHeight);
			void setUp(Window* win);
			void tearDown(Window* win);
			void destroy();
			void resize(const Size& width, const Size& height);

			void bindEntity(const GraphicsEntity* en);
			void bindShaderProgram(ShaderProgram& prog);

			std::string processShader(const std::string& shader, const GLenum& type);
			const mc::Preprocessor& getSSLPreprocessor();
		};

		//we declare RenderImpl which RenderProtocol extends. WE can't store a pointer to RenderProtocol (because its templated), but we can point to RenderImpl
		class RenderImpl {
			friend class Renderer;
			//ssl::init needs to be friends to access the index variable
			friend void ssl::init(const Size&, const Size&);
		public:
			RenderImpl();
			virtual ~RenderImpl() = default;

			virtual void resize(const Size width, const Size height) = 0;

			virtual void init(const Size originalWidth, const Size originalHeight) = 0;
			virtual void setUp(Window* win, RenderQueue* queue) = 0;
			virtual void render(Window* win, Entity* entity) = 0;
			virtual void tearDown(Window* win, RenderQueue* queue) = 0;
			virtual void destroy() = 0;
		private:
			static int index;
		};

		template<typename T>
		class RenderProtocol: public RenderImpl {
		public:
			using RenderImpl::RenderImpl;

			void resize(const Size width, const Size height) {};

			void init(const Size originalWidth, const Size originalHeight) {}
			void setUp(Window* win, RenderQueue* queue) {};
			void render(Window* win, Entity* entity) {};
			void tearDown(Window* win, RenderQueue* queue) {};
			void destroy() {};
		};

		class Renderer {
			//needs to access registerProtocol
			friend void ssl::init(const Size&, const Size&);
		public:

			static void resize(const Size width, const Size hieght);

			static void init(const Size originalWidth, const Size originalHeight);

			static void setUp(Window* win);

			template<typename T>
			static void queue(T* e) {
				if( e == nullptr || e == NULL )throw NullPointerException("Input pointer to an entity must not be null in queue()");
				if( RenderProtocol<T>::index == -1 )registerProtocol<T>();
				pushEntity(RenderProtocol<T>::index, e);
			};

			static Size numberOfProtocols();

			static void tearDown(Window* win);

			static void renderFrame(Window* win);

			static void destroy();

			static void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f);
			static void setRefreshColor(const Color& c);

			static Size getOriginalWidth();
			static Size getOriginalHeight();
		private:
			Renderer() = delete;
			~Renderer() = delete;

			static void pushEntity(Index protocol, Entity*  entity);
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