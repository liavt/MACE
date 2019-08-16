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
#include <MACE/Graphics/OGL/Dispatchable.h>
#include <MACE/Graphics/OGL/FreetypeFont.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <queue>
#include <list>
#include <mutex>

namespace mc {
	namespace internal {
		namespace ogl {
			class Painter;

			class Context: public gfx::GraphicsContextComponent {
			public:
				using ProtocolHash = unsigned short;

				struct RenderProtocol {
					ogl::ShaderProgram program;

					Enum sourceBlend = GL_SRC_ALPHA, destBlend = GL_ONE_MINUS_SRC_ALPHA;

					bool multitarget = true;

					bool created = false;
				};

				Context(gfx::WindowModule* win);
				Context(const Context& other) = delete;
				~Context() = default;

				gfx::ComponentPtr<gfx::Renderer> createRenderTarget() override;

				std::shared_ptr<gfx::ModelImpl> createModelImpl() override;
				std::shared_ptr<gfx::TextureImpl> createTextureImpl(const gfx::TextureDesc& desc) override;
				std::shared_ptr<gfx::FontImpl> createFontImpl(const gfx::FontDesc& desc) override;

				void dispatch(const DispatchFunction dispatch);

				void bindProtocol(Painter* painter, const std::pair<gfx::Painter::Brush, gfx::Painter::RenderFeatures> settings);

				void setTarget(const gfx::FrameBufferTarget& target);
			protected:
				void onInit(gfx::WindowModule* win) override;
				void onRender(gfx::WindowModule* win) override;
				void onDestroy(gfx::WindowModule* win) override;

				void setUp(gfx::WindowModule* win) override;
			private:
				std::queue <DispatchFunction, std::list<DispatchFunction>> dispatchQueue{};
				std::mutex dispatchMutex;

				std::unordered_map<ProtocolHash, RenderProtocol> protocols{};

				ProtocolHash currentProtocol = 0;

				//TODO forward declare this insead of including FreetypeFont.h
				MACE__INTERNAL_NS::fty::FreetypeLibrary freetype;

				gfx::FrameBufferTarget currentTarget = gfx::FrameBufferTarget::COLOR;

				void processDispatchQueue();

				void bindCurrentTarget();
			};
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLCONTEXT_H