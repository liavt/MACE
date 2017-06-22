/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_RENDERER_H
#define MACE__GRAPHICS_RENDERER_H

#include <MACE/Core/Interfaces.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Window.h>
#include <deque>


namespace mc {
	namespace gfx {

		//if the container we use is ever going to be changed, we typedef
		using RenderQueue = std::deque<GraphicsEntity*>;

		//forward define for friend declaration in later classes
		class Renderer;

		class Painter : public Initializable{
			friend class Renderer;
		public:
			virtual ~Painter() noexcept = default;

			GraphicsEntity* getEntity();
			const GraphicsEntity* getEntity() const;

			virtual void drawImage(const ColorAttachment& img, const float x = 0.0f, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f) = 0;

			virtual bool operator==(const Painter& other) const;
			bool operator!=(const Painter& other) const;
		protected:
			Painter(GraphicsEntity* const en);

			GraphicsEntity* const entity;
		};

		Renderer* getRenderer();
		/**
		@param r New Renderer. Must be dynamically allocated as `delete` will be called with it.
		*/
		void setRenderer(Renderer* r);

		/**
		@todo add function to change how many samples msaa uses
		*/
		class Renderer {

		public:
			virtual ~Renderer() = default;

			virtual GraphicsEntity* getEntityAt(const int x, const int y) = 0;

			/**
			@opengl
			*/
			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) = 0;

			virtual void onResize(const Size width, const Size height) = 0;
			virtual void onInit(const Size originalWidth, const Size originalHeight) = 0;
			virtual void onSetUp(os::WindowModule* win) = 0;
			virtual void onTearDown(os::WindowModule* win) = 0;
			virtual void onDestroy() = 0;
			virtual void onQueue(GraphicsEntity* en) = 0;

			virtual std::unique_ptr<Painter> getPainter(GraphicsEntity * const entity) const = 0;

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

			Index queue(GraphicsEntity* e);

			void remove(const Index id);

			/**
			@opengl
			*/
			void setRefreshColor(const Color& c);

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
			Index pushEntity(GraphicsEntity*  entity);

			RenderQueue renderQueue = RenderQueue();

			Size originalWidth = 0;
			Size originalHeight = 0;

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;
		};//Renderer
	}//gfx
}//mc

#endif
