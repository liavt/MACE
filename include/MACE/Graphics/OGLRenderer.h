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

namespace mc {
	namespace gfx {
		class GLPainter: public Painter {
		public:
			GLPainter(GraphicsEntity* const entity);

			void init() override;
			void destroy() override;

			void drawImage(const ColorAttachment& img, const float x = 0.0f, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f) override;
		};

		class GLRenderer: public Renderer {
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

			std::unique_ptr<Painter> getPainter(GraphicsEntity * const entity) const override;
		
			const Preprocessor& getSSLPreprocessor();
		private:
			Preprocessor sslPreprocessor;

			ogl::FrameBuffer frameBuffer;
			ogl::RenderBuffer depthBuffer;

			ogl::Texture2D sceneTexture, idTexture;
			
			Color clearColor;
			
			void generateFramebuffer(const Size& width, const Size& height);
		};
	}//gfx
}//mc

#endif
