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
			};

			class OGL33Texture: public TextureImpl, private ogl::Texture2D {
				friend class OGL33Renderer;
			public:
				void init() override;
				void destroy() override;

				void bind() const override;
				void bind(const Index location) const override;
				void unbind() const override;

				bool isCreated() const override;

				void setMinFilter(const gfx::Texture::ResizeFilter filter) override;
				void setMagFilter(const gfx::Texture::ResizeFilter filter) override;

				void setUnpackStorageHint(const gfx::Texture::PixelStorage hint, const int value) override;
				void setPackStorageHint(const gfx::Texture::PixelStorage hint, const int value) override;

				void setWrapS(const Texture::WrapMode wrap) override;
				void setWrapT(const Texture::WrapMode wrap) override;

				void setData(const void* data, const Size width, const Size height, const Texture::Type type = Texture::Type::FLOAT, const Texture::Format format = Texture::Format::RGB, const Texture::InternalFormat internalFormat = Texture::InternalFormat::RGB, const Index mipmap = 0) override;

				void getImage(const Texture::Format format, const Texture::Type type, void* data) const override;

				void setSwizzle(const Texture::SwizzleMode mode, const Texture::SwizzleMode arg) override;
			};

			class OGL33Context: public gfx::GraphicsContext {
			public:
				OGL33Context(os::WindowModule* win);
				OGL33Context(const OGL33Context& other) = delete;
				~OGL33Context() = default;

				std::shared_ptr<Renderer> getRenderer() const override;
				std::shared_ptr<ModelImpl> createModelImpl() const override;
				std::shared_ptr<TextureImpl> createTextureImpl() const override;
			protected:
				void onInit(os::WindowModule* win) override;
				void onRender(os::WindowModule* win) override;
				void onDestroy(os::WindowModule* win) override;
			private:
				std::shared_ptr<Renderer> renderer;
			};
		}//ogl
	}//gfx
}//mc

#endif//MACE__GRAPHICS_OGL_OGLCONTEXT_H