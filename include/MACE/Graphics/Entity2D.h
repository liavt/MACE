/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#ifndef MACE_GRAPHICS_ENTITY2D_H
#define MACE_GRAPHICS_ENTITY2D_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL.h>

namespace mc {
	namespace gfx {

		class Entity2D: public GraphicsEntity {
		public:
			Entity2D();
		protected:

		};//Entity2D

		class Image;

		/**
		@internal
		@opengl
		*/
		template<>
		class RenderProtocol<Image>: public RenderImpl {
		public:
			void resize(const Size width, const Size height) override;

			void init(const Size originalWidth, const Size originalHeight) override;

			void setUp(os::WindowModule* win, RenderQueue* queue) override;

			void render(os::WindowModule* win, GraphicsEntity* entity) override;

			void tearDown(os::WindowModule* win, RenderQueue* queue) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};

		int getImageProtocol();

		/**
		@todo figure out a way to make the protocol globally accessible
		*/
		class Image: public Entity2D {
			friend class RenderProtocol<Image>;
		public:
			Image() noexcept;
			Image(const ogl::Texture& tex);
			Image(const Color& col);
			~Image() = default;

			/**
			@dirty
			*/
			void setTexture(ogl::Texture& tex);
			/**
			@dirty
			*/
			ogl::Texture& getTexture();
			const ogl::Texture& getTexture() const;

			bool operator==(const Image& other) const;
			bool operator!=(const Image& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender() override final;
			void onDestroy() override final;
		private:
			ogl::Texture texture;
		};
	}//gfx
}//mc

#endif