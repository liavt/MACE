/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGL33RENDERER_H
#define MACE__GRAPHICS_OGL_OGL33RENDERER_H

#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL/OGL33.h>
#include <map>

namespace mc {
	namespace gfx {
		namespace ogl33 {
			class OGL33Painter;

			class OGL33Renderer: public Renderer {
				friend class OGL33Painter;
			public:
				struct RenderProtocol {
					ogl33::ShaderProgram program;

					Enum sourceBlend = GL_SRC_ALPHA, destBlend = GL_ONE_MINUS_SRC_ALPHA;

					bool multitarget = true;

					bool created = false;
				};

				OGL33Renderer();
				~OGL33Renderer() noexcept override = default;

				void onResize(gfx::WindowModule* win, const int width, const int height) override;
				void onInit(gfx::WindowModule* win) override;
				void onSetUp(gfx::WindowModule* win) override;
				void onTearDown(gfx::WindowModule* win) override;
				void onDestroy() override;
				void onQueue(GraphicsEntity* en) override;

				void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

				void getEntitiesAt(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h, EntityID* arr) const override;
				void getPixelsAt(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h, Color* arr, const FrameBufferTarget target) const override;

				std::shared_ptr<PainterImpl> createPainterImpl() override;

			private:
				ogl33::FrameBuffer frameBuffer{};
				ogl33::RenderBuffer sceneBuffer{}, idBuffer{}, dataBuffer{}, depthStencilBuffer{};

				Color clearColor = Colors::BLACK;

				std::unordered_map<unsigned short, OGL33Renderer::RenderProtocol> protocols{};

				unsigned short currentProtocol = 0;

				FrameBufferTarget currentTarget = FrameBufferTarget::COLOR;

				void generateFramebuffer(const int width, const int height);

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

					UniformBuffer* begin() {
						return &entityData;
					}
				} uniformBuffers;

				Metrics savedMetrics;
				Painter::State savedState;

				void createEntityData();
				void createPainterData();
			};
		}//ogl33
	}//gfx
}//mc

#endif
