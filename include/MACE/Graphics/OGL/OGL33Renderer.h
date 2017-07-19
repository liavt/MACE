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
				OGL33Painter(OGL33Renderer* const renderer, const GraphicsEntity* const entity);

				void init() override;
				void destroy() override;
			protected:
				void loadSettings(const Painter::State& state) override;
				void draw(const Enums::Brush brush, const Enums::RenderType type) override;
			private:
				OGL33Renderer* const renderer;
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

				void onResize(const Size width, const Size height) override;
				void onInit(const Size originalWidth, const Size originalHeight) override;
				void onSetUp(gfx::WindowModule* win) override;
				void onTearDown(gfx::WindowModule* win) override;
				void onDestroy() override;
				void onQueue(GraphicsEntity* en) override;

				void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

				GraphicsEntity* getEntityAt(const int x, const int y) override;

				std::shared_ptr<PainterImpl> createPainterImpl(const GraphicsEntity * const entity) override;

				const Preprocessor& getSSLPreprocessor();
			private:
				Preprocessor sslPreprocessor;

				ogl::FrameBuffer frameBuffer;
				ogl::RenderBuffer depthBuffer;

				ogl::Texture2D sceneTexture, idTexture;

				Color clearColor;

				void generateFramebuffer(const Size& width, const Size& height);

				//for Painting

				struct RenderProtocol {
					ogl::ShaderProgram program;
					ogl::VertexArray vao;
				};

				std::map<std::pair<Enums::Brush, Enums::RenderType>, std::unique_ptr<OGL33Renderer::RenderProtocol>> protocols;
				ogl::UniformBuffer entityUniforms = ogl::UniformBuffer();
				ogl::UniformBuffer painterUniforms = ogl::UniformBuffer();

				std::string processShader(const std::string& shader);

				void loadEntityUniforms(const GraphicsEntity * const entity);
				void loadPainterUniforms(const TransformMatrix& transform, const Color& col, const Color& secondaryCol, const Vector<float, 4>& data);

				OGL33Renderer::RenderProtocol& getProtocol(const GraphicsEntity* const entity, const std::pair<Enums::Brush, Enums::RenderType> settings);
				ogl::ShaderProgram getShadersForSettings(const std::pair<Enums::Brush, Enums::RenderType>& settings);
				ogl::VertexArray getVAOForSettings(const std::pair<Enums::Brush, Enums::RenderType>& settings);
			};
		}//ogl
	}//gfx
}//mc

#endif
