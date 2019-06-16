/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGL33RENDERER_H
#define MACE__GRAPHICS_OGL_OGL33RENDERER_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL/OGL33.h>
#include <unordered_map>

namespace mc {
	namespace gfx {
		namespace ogl33 {
			class OGL33Painter;


			class OGL33Renderer: public Renderer {
				friend class OGL33Painter;
			public:
				using ProtocolHash = unsigned short;

				struct RenderProtocol {
					ogl33::ShaderProgram program;

					Enum sourceBlend = GL_SRC_ALPHA, destBlend = GL_ONE_MINUS_SRC_ALPHA;

					bool multitarget = true;

					bool created = false;
				};

				OGL33Renderer();
				~OGL33Renderer() noexcept override = default;

				void onResize(gfx::WindowModule* win, const Pixels width, const Pixels height) override;
				void onInit(gfx::WindowModule* win) override;
				void onSetUp(gfx::WindowModule* win) override;
				void onTearDown(gfx::WindowModule* win) override;
				void onDestroy() override;
				void onQueue(GraphicsEntity* en) override;

				void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

				void getEntitiesAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, EntityID* arr) const override;
				void getPixelsAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, Color* arr, const FrameBufferTarget target) const override;

				std::shared_ptr<PainterImpl> createPainterImpl() override;

			private:
				ogl33::FrameBuffer frameBuffer{};
				ogl33::RenderBuffer sceneBuffer{}, idBuffer{}, dataBuffer{}, depthStencilBuffer{};

				Color clearColor = Colors::BLACK;

				std::unordered_map<ProtocolHash, OGL33Renderer::RenderProtocol> protocols{};

				ProtocolHash currentProtocol = 0;

				FrameBufferTarget currentTarget = FrameBufferTarget::COLOR;

				void generateFramebuffer(const Pixels width, const Pixels height);

				void bindProtocol(OGL33Painter* painter, const std::pair<Painter::Brush, Painter::RenderFeatures> settings);

				void setTarget(const FrameBufferTarget& target);

				void bindCurrentTarget();
			};

			class OGL33Painter: public PainterImpl {
				friend class OGL33Renderer;
			public:
				OGL33Painter(OGL33Renderer* const renderer);

				void init() override;
				void destroy() override;

				void begin() override;
				void end() override;

				void setTarget(const FrameBufferTarget& target) override;

				void clean() override;
			protected:
				void loadSettings(const Painter::State& state) override;
				void draw(const Model& m, const Painter::Brush brush) override;
			private:
				OGL33Renderer* const renderer;

				struct {
					UniformBuffer entityData;
					UniformBuffer painterData;
				} uniformBuffers;

				Metrics savedMetrics;
				Painter::State savedState;

				void createEntityData();
				void createPainterData();
			};
		}//ogl33
	}//gfx
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGL33RENDERER_H
