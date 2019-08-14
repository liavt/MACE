/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_CONTEXT_H
#define MACE__GRAPHICS_CONTEXT_H

#include <MACE/Core/Constants.h>
#include <MACE/Graphics/Texture.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/RenderTarget.h>

#include <memory>
#include <string>
#include <functional>

namespace mc {
	namespace gfx {
		class WindowModule;

		class Renderer;

		class FontImpl;
		struct FontDesc;

		class ModelImpl;
		struct ModelDesc;

		class MACE_NOVTABLE GraphicsContextComponent: public gfx::Component, public std::enable_shared_from_this<GraphicsContextComponent> {
			friend class Texture;
			friend class Model;
			friend class Font;
		public:
			using TextureCreateCallback = std::function<Texture()>;
			using ModelCreateCallback = std::function<Model()>;

			GraphicsContextComponent(gfx::WindowModule* win) MACE_EXPECTS(win != nullptr);
			//prevent copying
			GraphicsContextComponent(const GraphicsContextComponent& other) = delete;
			virtual ~GraphicsContextComponent() noexcept = default;

			void init() final;
			void render() final;
			void destroy() final;

			gfx::WindowModule* getWindow();
			const gfx::WindowModule* getWindow() const;

			virtual ComponentPtr<Renderer> createRenderTarget() = 0;

			/**
			@rendercontext
			*/
			Texture createTextureFromColor(const Color& col, const Pixels width = 1, const Pixels height = 1);
			/**
			@rendercontext
			*/
			Texture createTextureFromFile(const std::string& file, const ImageFormat format = ImageFormat::DONT_CARE, const TextureDesc::Wrap wrap = TextureDesc::Wrap::CLAMP);
			/**
			@rendercontext
			*/
			Texture createTextureFromFile(const CString file, const ImageFormat format = ImageFormat::DONT_CARE, const TextureDesc::Wrap wrap = TextureDesc::Wrap::CLAMP);
			/**
			@rendercontext
			*/
			Texture createTextureFromMemory(const unsigned char* c, const Size size);
			/**
			@copydoc createFromMemory(const unsigned char*, const int)
			*/
			template<const Size N>
			inline Texture createTextureFromMemory(const unsigned char c[N]) {
				return createTextureFromMemory(c, static_cast<int>(N));
			}
			/**
			@rendercontext
			*/
			Texture getSolidColor(const Color& color = Colors::WHITE);
			/**
			- Vertical gradient
			- Darker part on bottom
			- Height is 100

			@rendercontext
			*/
			Texture getGradient();

			Model getQuad();

			template<typename T>
			float convertPixelsToRelativeXCoordinates(T px) const {
				return static_cast<float>(px) / window->getLaunchConfig().width;
			}

			template<typename T>
			float convertPixelsToRelativeYCoordinates(T px) const {
				return static_cast<float>(px) / window->getLaunchConfig().height;
			}
		protected:
			gfx::WindowModule* window;

			/*
			Reasoning behind returning a std::shared_ptr and not a std::unique_ptr...

			Each Model and Texture class needs to have it's own Impl.
			Each Impl acts as a pointer to a resource, whether it be in GPU memory
			or whatever. However, Model and Texture needs the ability to be moved,
			like so:

			Model model = Model();
			Model newModel = m;

			Both Model objects will share the same resource, as it is simply a
			pointer to the actual data. However, with std::unique_ptr, this move
			semantic because near impossible. Because multiple Model objects
			may have to own the same pointer to data, they have to use std::shared_ptr
			*/
			MACE_NODISCARD virtual std::shared_ptr<ModelImpl> createModelImpl() = 0;
			MACE_NODISCARD virtual std::shared_ptr<TextureImpl> createTextureImpl(const TextureDesc& desc) = 0;
			MACE_NODISCARD virtual std::shared_ptr<FontImpl> createFontImpl(const FontDesc& desc) = 0;

			virtual void onInit(gfx::WindowModule* win) = 0;
			virtual void onRender(gfx::WindowModule* win) = 0;
			virtual void onDestroy(gfx::WindowModule* win) = 0;
		};
	}
}//mc

#endif//MACE__GRAPHICS_CONTEXT_H
