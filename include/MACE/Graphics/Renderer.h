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
#include <MACE/Core/Error.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Utility/Vector.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/Color.h>

#include <deque>
#include <stack>

namespace mc {
	namespace gfx {
		//if the container we use is ever going to be changed, we typedef
		using RenderQueue = std::deque<GraphicsEntity*>;

		//forward define for friend declaration in later classes
		class Renderer;
		class PainterImpl;
		class Texture;

		class Painter: public Initializable {
		public:
			//painter stuff
			enum class Brush: Byte {
				TEXTURE = 0, COLOR = 1, MASK = 2, MASKED_BLEND = 3,
			};

			enum class RenderType: Byte {
				QUAD = 0
			};

			struct State {
				TransformMatrix transformation;

				Color primaryColor, secondaryColor;

				Vector<float, 4> data;

				bool operator==(const State& other) const;
				bool operator!=(const State& other) const;
			};

			enum class DirtyFlag: Byte {
				PRIMARY_COLOR = 0,
				SECONDARY_COLOR = 1,
				TRANSLATION = 2,
				ROTATION = 3,
				SCALE = 4
			};

			Painter(GraphicsEntity* const en);

			void init() override;
			void destroy() override;

			std::shared_ptr<PainterImpl> getImplementation();
			const std::shared_ptr<PainterImpl> getImplementation() const;

			std::shared_ptr<PainterImpl> operator*();
			const std::shared_ptr<PainterImpl> operator*() const;

			std::shared_ptr<PainterImpl> operator->();
			const std::shared_ptr<PainterImpl> operator->() const;
		private:
			std::shared_ptr<PainterImpl> impl = nullptr;
		};

		class PainterImpl: public Initializable {
			friend class Renderer;
		public:
			virtual ~PainterImpl() noexcept = default;

			virtual void init() override = 0;
			virtual void destroy() override = 0;

			void fillRect(const float x = 0.0f, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f);
			void fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size);
			void fillRect(const Vector<float, 4>& dim);

			void drawImage(const Texture& img);

			void maskImage(const Texture& img, const Texture& mask);

			void blendImagesMasked(const Texture& foreground, const Texture& background, const Texture& mask, const float minimumThreshold = 0.0f, const float maximumThreshold = 1.0f);

			const GraphicsEntity* const getEntity() const;

			void setColor(const Color& col);
			Color& getColor();
			const Color& getColor() const;

			void setSecondaryColor(const Color& col);
			Color& getSecondaryColor();
			const Color& getSecondaryColor() const;

			void setData(const Vector<float, 4>& col);
			Vector<float, 4>& getData();
			const Vector<float, 4>& getData() const;

			void resetColor();

			void setTransformation(const TransformMatrix& trans);
			TransformMatrix& getTransformation();
			const TransformMatrix& getTransformation() const;

			void translate(const Vector<float, 3>& vec);
			void translate(const float x, const float y, const float z = 0.0f);

			void rotate(const Vector<float, 3>& vec);
			void rotate(const float x, const float y, const float z);

			void scale(const Vector<float, 3>& vec);
			void scale(const float x, const float y, const float z = 0.0f);

			void resetTransform();

			void push();
			void pop();

			virtual void reset();

			virtual bool operator==(const PainterImpl& other) const;
			bool operator!=(const PainterImpl& other) const;
		protected:
			PainterImpl(const GraphicsEntity* const en);

			virtual void loadSettings() = 0;
			virtual void draw(const Painter::Brush brush, const Painter::RenderType type) = 0;

			const GraphicsEntity* const entity;

			Painter::State state;
		private:
			//for pushing/popping the state
			std::stack<Painter::State> stateStack;
		};

		/**
		@todo add function to change how many samples msaa uses
		@todo add renderers for directx, cpu, offscreen, opengl es, opengl 1.1/2.1
		*/
		class Renderer {
			friend class Painter;
			friend class GraphicsContext;
		public:
			virtual ~Renderer() = default;

			virtual GraphicsEntity* getEntityAt(const int x, const int y) = 0;

			/**
			@opengl
			*/
			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) = 0;

			virtual void onResize(const Size width, const Size height) = 0;
			virtual void onInit(const Size originalWidth, const Size originalHeight) = 0;
			virtual void onSetUp(gfx::WindowModule* win) = 0;
			virtual void onTearDown(gfx::WindowModule* win) = 0;
			virtual void onDestroy() = 0;
			virtual void onQueue(GraphicsEntity* en) = 0;

			/**
			@internal
			@opengl
			*/
			void resize(const Size width, const Size height);

			/**
			@internal
			@opengl
			*/
			void init(gfx::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void setUp(gfx::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void tearDown(gfx::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void checkInput(gfx::WindowModule* win);


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

			Size getWidth() const;
			Size getHeight() const;

			Size getSamples() const;

			Vector<float, 2> getWindowRatios() const;

			RenderQueue getRenderQueue() const;

			bool isResized() const;

			GraphicsContext* getContext();
			const GraphicsContext* getContext() const;
		protected:
			//not declared const because some of the functions require modification to an intneral buffer of impls
			virtual std::shared_ptr<PainterImpl> createPainterImpl(const GraphicsEntity * const entity) = 0;

			Index pushEntity(GraphicsEntity*  entity);

			RenderQueue renderQueue = RenderQueue();

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;
			
			GraphicsContext* context;
		};//Renderer
	}//gfx
}//mc

#endif
