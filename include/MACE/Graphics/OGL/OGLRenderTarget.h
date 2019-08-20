/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGLRENDERTARGET_H
#define MACE__GRAPHICS_OGL_OGLRENDERTARGET_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Graphics/RenderTarget.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/OGL/Dispatchable.h>
#include <MACE/Graphics/OGL/OGLContext.h>
#include <unordered_map>

namespace mc {
	namespace internal {
		namespace ogl {
			class Painter;


			class Renderer: public gfx::Renderer, private Dispatchable {
				friend class Painter;
			public:
				Renderer(std::shared_ptr<Context> context);
				~Renderer() noexcept override = default;

				void onInit() override;
				void onDestroy() override;
				void onQueue(gfx::Entity* en) override;

				void preRender();
				void postRender(gfx::WindowModule* win);

				void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

				void getEntitiesAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, gfx::EntityID* arr) const override;
				void getPixelsAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, Color* arr, const gfx::FrameBufferTarget target) const override;

				std::shared_ptr<gfx::PainterImpl> createPainterImpl() override;

			private:
				ogl::FrameBuffer frameBuffer{};
				ogl::RenderBuffer sceneBuffer{}, idBuffer{}, dataBuffer{}, depthStencilBuffer{};

				Color clearColor = Colors::BLACK;

				void generateFramebuffer(const Pixels width, const Pixels height);
			};

			class Painter: public gfx::PainterImpl, private Dispatchable {
				friend class Context;
			public:
				Painter(std::shared_ptr<Context> context, Renderer* const renderer);

				void init() override;
				void destroy() override;

				void begin() override;
				void end() override;

				void setTarget(const gfx::FrameBufferTarget& target) override;

				void clean() override;
			protected:
				void loadSettings(const gfx::Painter::State& state) override;
				void draw(const gfx::Model& m, const gfx::Painter::Brush brush) override;
			private:
				Renderer* const renderer;

				struct {
					UniformBuffer entityData;
					UniformBuffer painterData;
				} uniformBuffers;

				gfx::Metrics savedMetrics;
				gfx::Painter::State savedState;

				void createEntityData();
				void createPainterData();
			};
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLTARGET_H
