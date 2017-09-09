/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGL33CONTEXT_H
#define MACE__GRAPHICS_OGL_OGL33CONTEXT_H

#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>

namespace mc {
	namespace gfx {
		namespace ogl {
			class OGL33Model: public ModelImpl, private ogl::VertexArray {
				friend class OGL33Renderer;
			public:
				void init() override;
				void destroy() override;

				void bind() const override;
				void unbind() const override;

				void draw() const override;

				void loadTextureCoordinates(const Size dataSize, const float* data) override;
				void loadVertices(const Size verticeSize, const float* vertices) override;
				void loadIndices(const Size indiceNum, const unsigned int* indiceData) override;

				bool isCreated() const override;
			};

			class OGL33Texture: public TextureImpl, private ogl::Texture2D {
				friend class OGL33Renderer;
			public:
				OGL33Texture(const TextureDesc& desc);
				~OGL33Texture() override;

				void bind() const override;
				void bind(const unsigned int location) const override;
				void unbind() const override;

				bool isCreated() const override;

				void setUnpackStorageHint(const gfx::Enums::PixelStorage hint, const int value) override;
				void setPackStorageHint(const gfx::Enums::PixelStorage hint, const int value) override;

				void setData(const void* data, const Index mipmap = 0) override;

				void readPixels(void* data) const override;
			};

			class OGL33Context: public gfx::GraphicsContext {
			public:
				OGL33Context(gfx::WindowModule* win);
				OGL33Context(const OGL33Context& other) = delete;
				~OGL33Context() = default;

				Renderer* getRenderer() const override;
				std::shared_ptr<ModelImpl> createModelImpl() const override;
				std::shared_ptr<TextureImpl> createTextureImpl(const TextureDesc& desc) const override;
			protected:
				void onInit(gfx::WindowModule* win) override;
				void onRender(gfx::WindowModule* win) override;
				void onDestroy(gfx::WindowModule* win) override;
			private:
				std::unique_ptr<Renderer> renderer;
			};
		}//ogl
	}//gfx
}//mc

#endif//MACE__GRAPHICS_OGL_OGLCONTEXT_H