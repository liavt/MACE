/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGLCONTEXT_H
#define MACE__GRAPHICS_OGL_OGLCONTEXT_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/Texture.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/OGL/FreetypeFont.h>
#include <queue>
#include <list>
#include <functional>
#include <mutex>

namespace mc {
	namespace internal {
		namespace ogl {
			class Context;

			using DispatchFunction = std::function<void()>;

			class MACE_NOVTABLE Dispatchable {
			protected:
				std::shared_ptr<Context> context;

				Dispatchable(std::shared_ptr<Context> context);

				void dispatch(const DispatchFunction func) const;
			};

			class Model: public gfx::ModelImpl, private ogl::VertexArray, private Dispatchable {
			public:
				Model(std::shared_ptr<Context> context);
				~Model();

				void bind() const;

				void draw() const override;

				/*
				@bug these functions will create a NEW buffer each time you call them, instead of reusing them
				*/
				void loadTextureCoordinates(const unsigned int dataSize, const float* data) override;
				void loadVertices(const unsigned int verticeSize, const float* vertices) override;
				void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) override;
			};

			class Texture: public gfx::TextureImpl, private ogl::Texture2D, private Dispatchable {
			public:
				Texture(std::shared_ptr<Context> context, const gfx::TextureDesc& desc);
				~Texture() override;

				void setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const gfx::PixelStorageHints hints) override;

				void readPixels(void* data, const gfx::PixelStorageHints hints) const override;

				void bindTextureSlot(const gfx::TextureSlot slot) const override;
			};

			class Renderer;

			class Context: public gfx::GraphicsContextComponent {
			public:
				Context(gfx::WindowModule* win);
				Context(const Context& other) = delete;
				~Context() = default;

				gfx::ComponentPtr<gfx::Renderer> createRenderTarget() override;

				std::shared_ptr<gfx::ModelImpl> createModelImpl() override;
				std::shared_ptr<gfx::TextureImpl> createTextureImpl(const gfx::TextureDesc& desc) override;
				std::shared_ptr<gfx::FontImpl> createFontImpl(const gfx::FontDesc& desc) override;

				void dispatch(const DispatchFunction dispatch);
			protected:
				void onInit(gfx::WindowModule* win) override;
				void onRender(gfx::WindowModule* win) override;
				void onDestroy(gfx::WindowModule* win) override;
			private:
				std::queue <DispatchFunction, std::list<DispatchFunction>> dispatchQueue{};
				std::mutex dispatchMutex;

				//TODO forward declare this insead of including FreetypeFont.h
				MACE__INTERNAL_NS::fty::FreetypeLibrary freetype;

				void processDispatchQueue();
			};
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLCONTEXT_H