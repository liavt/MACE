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
#include <MACE/Graphics/Window.h>
#include <deque>


namespace mc {
	namespace gfx {

		//if the container we use is ever going to be changed, we typedef
		using RenderQueue = std::deque<std::pair<Index, GraphicsEntity*>>;

		//forward define for friend declaration in later classes
		class Renderer;

		class Painter {
			friend class Renderer;
		public:
			virtual ~Painter() noexcept = default;

			GraphicsEntity* getEntity();
			const GraphicsEntity* getEntity() const;

			virtual bool operator==(const Painter& other) const;
			bool operator!=(const Painter& other) const;
		protected:
			Painter(GraphicsEntity* const en);

			GraphicsEntity* const entity;
		};

		//we declare RenderImpl which RenderProtocol extends. WE can't store a pointer to RenderProtocol (because its templated), but we can point to RenderImpl
		/**
		@internal
		*/
		class RenderImpl {
			friend class Renderer;
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
			RenderProtocol() {
				//cheaty way to always fail a static_assert without including <type_traits> for std::false_type
				//NOTE: for the uninformed, static_assert(false) is ill formed
				static_assert(sizeof(T) != sizeof(T), "No RenderProtocol specialization exists for this GraphicsEntity type!");
			}
		private:
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
		class Renderer {

		public:
			virtual ~Renderer() = default;

			/**
			@opengl
			*/
			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) = 0;

			virtual void onResize(const Size width, const Size height) = 0;
			virtual void onInit(const Size originalWidth, const Size originalHeight) = 0;
			virtual void onSetUp(os::WindowModule* win) = 0;
			virtual void onTearDown(os::WindowModule* win) = 0;
			virtual void onDestroy() = 0;
			virtual void onInputCheck(os::WindowModule* win) = 0;

			/**
			@internal
			@opengl
			*/
			void resize(const Size width, const Size height);

			/**
			@internal
			@opengl
			*/
			void init(const Size originalWidth, const Size originalHeight);

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
			void renderFrame(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void checkInput(os::WindowModule* win);


			/**
			@internal
			@opengl
			*/
			void destroy();

			/**
			@internal
			*/
			void flagResize();

			/**
			@internal
			@opengl
			*/
			void initEntity(GraphicsEntity* en, const Index protocol);

			/**
			@internal
			@opengl
			*/
			void cleanEntity(GraphicsEntity* en, const Index protocol);

			/**
			@internal
			@opengl
			*/
			void destroyEntity(GraphicsEntity* en, const Index protocol);

			void queue(GraphicsEntity* e, const Index protocol);

			Size numberOfProtocols();

			/**
			@internal
			@opengl
			*/
			void clearBuffers();

			/**
			@opengl
			*/
			void setRefreshColor(const Color& c);

			template<typename T>
			Index registerProtocol() {
				//in destroy(), this memory is deleted
				RenderImpl* protocol = new RenderProtocol<T>();
				if (protocol == nullptr) {
					MACE__THROW(InvalidType, "Error creating RenderProtocol because template does not exist for it");
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

			RenderQueue renderQueue = RenderQueue();

			Size originalWidth = 0;
			Size originalHeight = 0;

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;
		private:
			//this variable is used for both ssl and Renderer. Each iteration through the queue, this is incremented. It is then passed to the shader, and the shader returns which entity was hovered over
			Index entityIndex = 0;

			std::vector<RenderImpl*> protocols = std::vector<RenderImpl*>();
		};//Renderer
	}//gfx
}//mc

#endif
