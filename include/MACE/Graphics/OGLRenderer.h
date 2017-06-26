/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_OGLRENDERER_H
#define MACE__GRAPHICS_OGLRENDERER_H

#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Utility/Preprocessor.h>
#include <map>

namespace mc {
	namespace gfx {
		class GLRenderer;

		class GLPainter: public PainterImpl {
		public:
			GLPainter(const GraphicsEntity* const entity);

			void init() override;
			void destroy() override;
		protected:
			void loadSettings(TransformMatrix transform, Color prim, Color second) override;
			void draw(const Painter::Brush brush, const Painter::RenderType type) override;
		private:
			GLRenderer* renderer;
		};

		class GLRenderer: public Renderer {
			friend class GLPainter;
		public:
			GLRenderer();
			~GLRenderer() noexcept override = default;

			void onResize(const Size width, const Size height) override;
			void onInit(const Size originalWidth, const Size originalHeight) override;
			void onSetUp(os::WindowModule* win) override;
			void onTearDown(os::WindowModule* win) override;
			void onDestroy() override;
			void onQueue(GraphicsEntity* en) override;

			void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

			GraphicsEntity* getEntityAt(const int x, const int y) override;

			std::shared_ptr<PainterImpl> getPainter(const GraphicsEntity * const entity) const override;

			const Preprocessor& getSSLPreprocessor();
		private:
			Preprocessor sslPreprocessor;

			ogl::FrameBuffer frameBuffer;
			ogl::RenderBuffer depthBuffer;

			ogl::Texture2D sceneTexture, idTexture;


			Color clearColor;

			void generateFramebuffer(const Size& width, const Size& height);

			struct RenderSettings {
				Painter::Brush brush;
				Painter::RenderType type;

				//required for use as a key in std::map
				bool operator<(const RenderSettings& other) const {
					return brush < other.brush && type < other.type;
				}
			};

			struct RenderProtocol {
				ogl::ShaderProgram program;
				ogl::VertexArray vao;
			};

			std::map<GLRenderer::RenderSettings, GLRenderer::RenderProtocol> protocols = std::map<GLRenderer::RenderSettings, GLRenderer::RenderProtocol>();
			ogl::UniformBuffer entityUniforms = ogl::UniformBuffer();
			ogl::UniformBuffer painterUniforms = ogl::UniformBuffer();

			std::string processShader(const std::string& shader);

			void loadEntityUniforms(const GraphicsEntity * const entity);
			void loadPainterUniforms(TransformMatrix transform, Color prim, Color second);

			GLRenderer::RenderProtocol& getProtocol(const GraphicsEntity* const entity, const RenderSettings settings);
			ogl::ShaderProgram getShadersForSettings(const RenderSettings settings);
			ogl::VertexArray getVAOForSettings(const RenderSettings settings);
		};
	}//gfx
}//mc

#endif
