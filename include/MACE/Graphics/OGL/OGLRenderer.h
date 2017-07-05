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
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Utility/Preprocessor.h>
#include <map>

namespace mc {
	namespace gfx {
		namespace ogl {
			class OGL33Renderer;

			class OGL33Model: public ModelImpl, private ogl::VertexArray {
				friend class OGL33Renderer;
			public:
				void init() override;
				void destroy() override;
			};

			class OGL33Texture: public TextureImpl, private ogl::Texture2D {
				friend class OGL33Renderer;
			public:
				void init() override;
				void destroy() override;

				void bind() const override;
				void bind(const Index location) const override;
				void unbind() const override;

				bool isCreated() const override;

				void setMinFilter(const gfx::Texture::ResizeFilter filter) override;
				void setMagFilter(const gfx::Texture::ResizeFilter filter) override;

				void setUnpackStorageHint(const gfx::Texture::PixelStorage hint, const int value) override;
				void setPackStorageHint(const gfx::Texture::PixelStorage hint, const int value) override;

				void setWrapS(const Texture::WrapMode wrap) override;
				void setWrapT(const Texture::WrapMode wrap) override;

				void setData(const void* data, const Size width, const Size height, const Texture::Type type = Texture::Type::FLOAT, const Texture::Format format = Texture::Format::RGB, const Texture::InternalFormat internalFormat = Texture::InternalFormat::RGB, const Index mipmap = 0) override;

				void getImage(const Texture::Format format, const Texture::Type type, void* data) const override;

				void setSwizzle(const Texture::SwizzleMode mode, const Texture::SwizzleMode arg) override;
			};

			class OGL33Painter: public PainterImpl {
				friend class OGL33Renderer;
			public:
				OGL33Painter(const GraphicsEntity* const entity);

				void init() override;
				void destroy() override;
			protected:
				void loadSettings() override;
				void draw(const Painter::Brush brush, const Painter::RenderType type) override;
			private:
				OGL33Renderer* renderer;
			};

			class OGL33Renderer: public Renderer {
				friend class OGL33Painter;
			public:
				OGL33Renderer();
				~OGL33Renderer() noexcept override = default;

				void onResize(const Size width, const Size height) override;
				void onInit(const Size originalWidth, const Size originalHeight) override;
				void onSetUp(os::WindowModule* win) override;
				void onTearDown(os::WindowModule* win) override;
				void onDestroy() override;
				void onQueue(GraphicsEntity* en) override;

				void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) override;

				GraphicsEntity* getEntityAt(const int x, const int y) override;

				std::shared_ptr<PainterImpl> getPainter(const GraphicsEntity * const entity) const override;
				std::shared_ptr<TextureImpl> getTexture() const override;
				std::shared_ptr<ModelImpl> getModel() const override;

				const Preprocessor& getSSLPreprocessor();
			private:
				Preprocessor sslPreprocessor;

				ogl::FrameBuffer frameBuffer;
				ogl::RenderBuffer depthBuffer;

				ogl::Texture2D sceneTexture, idTexture;

				std::map<Byte, std::shared_ptr<ogl::OGL33Texture>> textureImpls;

				Color clearColor;

				void generateFramebuffer(const Size& width, const Size& height);

				//for Painting

				struct RenderProtocol {
					ogl::ShaderProgram program;
					ogl::VertexArray vao;
				};

				std::map<std::pair<Painter::Brush, Painter::RenderType>, std::unique_ptr<OGL33Renderer::RenderProtocol>> protocols;
				ogl::UniformBuffer entityUniforms = ogl::UniformBuffer();
				ogl::UniformBuffer painterUniforms = ogl::UniformBuffer();

				std::string processShader(const std::string& shader);

				void loadEntityUniforms(const GraphicsEntity * const entity);
				void loadPainterUniforms(const TransformMatrix& transform, const Color& col, const Color& secondaryCol, const Vector<float, 4>& data);

				OGL33Renderer::RenderProtocol& getProtocol(const GraphicsEntity* const entity, const std::pair<Painter::Brush, Painter::RenderType> settings);
				ogl::ShaderProgram getShadersForSettings(const std::pair<Painter::Brush, Painter::RenderType>& settings);
				ogl::VertexArray getVAOForSettings(const std::pair<Painter::Brush, Painter::RenderType>& settings);
			};
		}//ogl
	}//gfx
}//mc

#endif
