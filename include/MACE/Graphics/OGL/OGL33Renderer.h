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
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Utility/Preprocessor.h>
#include <map>

namespace mc {
	namespace gfx {
		namespace ogl {
			class OGL33Renderer;

			class OGL33Painter: public PainterImpl {
				friend class OGL33Renderer;
			public:
				OGL33Painter(OGL33Renderer* const renderer, Painter* const p);

				void init() override;
				void destroy() override;

				void begin() override;
				void end() override;

				void clean() override;
			protected:
				void loadSettings(const Painter::State& state, const std::uint_least16_t& dirtyFlags) override;
				void draw(const Model& m, const Enums::Brush brush, const Enums::RenderType type) override;
			private:
				OGL33Renderer* const renderer;

				UniformBuffer painterData;
				UniformBuffer entityData;
			};

			/**
			@todo Remove discard from mc_frag.glsl
			@todo get rid of global constant IncludeString
			*/
			class OGL33Renderer: public Renderer {
				friend class OGL33Painter;
			public:
				OGL33Renderer();
				~OGL33Renderer() noexcept override = default;

				void onResize(gfx::WindowModule* win, const Size width, const Size height) override;
				void onInit(gfx::WindowModule* win) override;
				void onSetUp(gfx::WindowModule* win) override;
				void onTearDown(gfx::WindowModule* win) override;
				void onDestroy() override;
				void onQueue(GraphicsEntity* en) override;

				void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

				GraphicsEntity* getEntityAt(const int x, const int y) override;

				std::shared_ptr<PainterImpl> createPainterImpl(Painter* const p) override;

				const Preprocessor& getShaderPreprocessor();
			private:
				//for shaders

				const IncludeString vertexLibrary = IncludeString({
#	include <MACE/Graphics/OGL/Shaders/mc_vertex.glsl>
				}, "mc_vertex");
				const IncludeString fragmentLibrary = IncludeString({
#	include <MACE/Graphics/OGL/Shaders/mc_frag.glsl>
				}, "mc_frag");
				const IncludeString positionLibrary = IncludeString({
#	include <MACE/Graphics/OGL/Shaders/mc_position.glsl>
				}, "mc_position");
				const IncludeString entityLibrary = IncludeString({
#	include <MACE/Graphics/OGL/Shaders/mc_entity.glsl>
				}, "mc_entity");
				const IncludeString coreLibrary = IncludeString({
#	include <MACE/Graphics/OGL/Shaders/mc_core.glsl>
				}, "mc_core");

				Preprocessor shaderPreprocessor = Preprocessor("");

				//for the framebuffer

				ogl::FrameBuffer frameBuffer{};
				ogl::RenderBuffer depthBuffer{};

				ogl::Texture2D sceneTexture{}, idTexture{};

				Color clearColor = Colors::BLACK;

				void generateFramebuffer(const Size& width, const Size& height);

				//for the Painter

				struct RenderProtocol {
					ogl::ShaderProgram program;
				};

				std::map<std::pair<Enums::Brush, Enums::RenderType>, OGL33Renderer::RenderProtocol> protocols{};

				std::string processShader(const std::string& shader);

				OGL33Renderer::RenderProtocol& getProtocol(OGL33Painter* painter, const std::pair<Enums::Brush, Enums::RenderType> settings);
				ogl::ShaderProgram createShadersForSettings(const std::pair<Enums::Brush, Enums::RenderType>& settings);
			};
		}//ogl
	}//gfx
}//mc

#endif
