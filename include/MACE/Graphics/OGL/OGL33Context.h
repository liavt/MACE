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
	namespace internal {
		namespace ogl33 {

			class OGL33Model: public gfx::ModelImpl, private ogl33::VertexArray {
				friend class OGL33Renderer;
			public:
				OGL33Model();
				~OGL33Model();

				void bind() const;

				void draw() const override;

				/*
				@bug these functions will create a NEW buffer each time you call them, instead of reusing them
				*/
				void loadTextureCoordinates(const unsigned int dataSize, const float* data) override;
				void loadVertices(const unsigned int verticeSize, const float* vertices) override;
				void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) override;
			};

			class OGL33Texture: public gfx::TextureImpl, private ogl33::Texture2D {
			public:
				OGL33Texture(const gfx::TextureDesc& desc);
				~OGL33Texture() override;

				void setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const gfx::PixelStorageHints hints) override;

				void readPixels(void* data, const gfx::PixelStorageHints hints) const override;

				void bindTextureSlot(const gfx::TextureSlot slot) const override;
			};

			class OGL33Context: public gfx::GraphicsContextComponent {
			public:
				OGL33Context(gfx::WindowModule* win);
				OGL33Context(const OGL33Context& other) = delete;
				~OGL33Context() = default;

				gfx::Renderer* getRenderer() const override;

				std::shared_ptr<gfx::ModelImpl> createModelImpl() override;
				std::shared_ptr<gfx::TextureImpl> createTextureImpl(const gfx::TextureDesc& desc) override;
				std::shared_ptr<gfx::FontImpl> createFontImpl(const gfx::FontDesc& desc) override;
			protected:
				void onInit(gfx::WindowModule* win) override;
				void onRender(gfx::WindowModule* win) override;
				void onDestroy(gfx::WindowModule* win) override;
			private:
				std::unique_ptr<gfx::Renderer> renderer;
				//TODO forward declare this insead of including FreetypeFont.h
				MACE__INTERNAL_NS::fty::FreetypeLibrary freetype;
			};
		}//ogl33
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLCONTEXT_H