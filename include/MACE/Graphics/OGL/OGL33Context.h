/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGL33CONTEXT_H
#define MACE__GRAPHICS_OGL_OGL33CONTEXT_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/OGL/OGL33.h>
#include <MACE/Graphics/OGL/FreetypeFont.h>

namespace mc {
	namespace gfx {
		namespace ogl33 {
			class OGL33Model: public ModelImpl, private ogl33::VertexArray {
				friend class OGL33Renderer;
			public:
				OGL33Model();
				~OGL33Model();

				void bind() const override;
				void unbind() const override;

				void draw() const override;

				void loadTextureCoordinates(const unsigned int dataSize, const float* data) override;
				void loadVertices(const unsigned int verticeSize, const float* vertices) override;
				void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) override;
			};

			class OGL33Texture: public TextureImpl, private ogl33::Texture2D {
			public:
				OGL33Texture(const TextureDesc& desc);
				~OGL33Texture() override;

				void bind() const override;
				void bind(const TextureSlot slot) const override;
				void unbind() const override;

				void setUnpackStorageHint(const gfx::PixelStorage hint, const int value) override;
				void setPackStorageHint(const gfx::PixelStorage hint, const int value) override;

				void setData(const void* data, const int mipmap = 0) override;

				void readPixels(void* data) const override;
			};

			class OGL33Context: public gfx::GraphicsContext {
			public:
				OGL33Context(gfx::WindowModule* win);
				OGL33Context(const OGL33Context& other) = delete;
				~OGL33Context() = default;

				Renderer* getRenderer() const override;

				std::shared_ptr<ModelImpl> createModelImpl() override;
				std::shared_ptr<TextureImpl> createTextureImpl(const TextureDesc& desc) override;
				std::shared_ptr<FontImpl> createFontImpl(const FontDesc& desc) override;
			protected:
				void onInit(gfx::WindowModule* win) override;
				void onRender(gfx::WindowModule* win) override;
				void onDestroy(gfx::WindowModule* win) override;
			private:
				std::unique_ptr<Renderer> renderer;
				//TODO forward declare this insead of including FreetypeFont.h
				fty::FreetypeLibrary freetype;
			};
		}//ogl33
	}//gfx
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLCONTEXT_H