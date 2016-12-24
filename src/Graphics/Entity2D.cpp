/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity2D.h>
//for memcpy
#include <cstring>

namespace mc {
	namespace gfx {


		int Image::protocol = -1;

		Entity2D::Entity2D() : GraphicsEntity() {}

		Image::Image() noexcept {}

		Image::Image(const ogl::Texture & tex) {
			texture = tex;
		}

		void Image::setTextureData(const ogl::UniformBuffer & buf) {
			if( textureData != buf ) {
				makeDirty();
				textureData = buf;
			}
		}

		ogl::UniformBuffer & Image::getTextureData() {
			makeDirty();

			return textureData;
		}

		const ogl::UniformBuffer & Image::getTextureData() const {
			return textureData;
		}

#define MACE_TEXTURE_DATA_BUFFER_SIZE 5
#define MACE_TEXTURE_DATA_BUFFER_LOCATION 0

		void Image::onInit() {
			textureData.init();

			textureData.setLocation(MACE_TEXTURE_DATA_BUFFER_LOCATION);

			textureData.bind();
			//tell it how much data to allocate
			textureData.setData(sizeof(float)*MACE_TEXTURE_DATA_BUFFER_SIZE, nullptr);
			textureData.unbind();

			if( Image::protocol < 0 ) {
				Image::protocol = Renderer::registerProtocol<Image>();
			}
		}

		void Image::onUpdate() {}

		void Image::onRender() {
			Renderer::queue(this, Image::protocol);
		}

		void Image::onDestroy() {
			textureData.destroy();
		}


		void Image::onClean() {
			const Color& paint = texture.getPaint();
			const float& opacity = texture.getOpacity();

			textureData.bind();
			//holy crap thats a lot of flags. this is the fastest way to map the buffer. the difference is MASSIVE. try it.
			float* mappedTextureData = static_cast<float*>(textureData.mapRange(0, sizeof(float)*MACE_TEXTURE_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
			std::memcpy(mappedTextureData, &paint, sizeof(paint));
			mappedTextureData += 4;
			std::memcpy(mappedTextureData, &opacity, sizeof(opacity));
			textureData.unmap();
		}

		Color & Image::getPaint() {
			makeDirty();

			return texture.getPaint();
		}

		const Color & Image::getPaint() const {
			return texture.getPaint();
		}

		void Image::setPaint(const Color & c) {
			if( texture.getPaint() != c ) {
				makeDirty();

				texture.setPaint(c);
			}
		}

		float Image::getOpacity() {
			makeDirty();

			return texture.getOpacity();
		}

		const float Image::getOpacity() const {
			return texture.getOpacity();
		}

		void Image::setOpacity(const float f) {
			if( texture.getOpacity() != f ) {
				makeDirty();

				texture.setOpacity(f);
			}
		}

		void Image::setTexture(ogl::Texture & tex) {
			if( tex != texture ) {
				makeDirty();

				texture = tex;
			}
		}

		ogl::Texture & Image::getTexture() {
			makeDirty();

			return texture;
		}

		const ogl::Texture & Image::getTexture() const {
			return texture;
		}

		bool Image::operator==(const Image & other) const {
			return Entity2D::operator==(other) && texture == other.texture&&textureData == other.textureData;
		}

		bool Image::operator!=(const Image & other) const {
			return !operator==(other);
		}

		void RenderProtocol<Image>::resize(const Size, const Size) {}

		void RenderProtocol<Image>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
		#	include <MACE/Graphics/Shaders/image.v.glsl>
			};
			const char* fragmentShader2D = {
		#	include <MACE/Graphics/Shaders/image.f.glsl>
			};

			renderer.init(vertexShader2D, fragmentShader2D);
		}//init

		void RenderProtocol<Image>::setUp(os::WindowModule*, RenderQueue*) {};

		void RenderProtocol<Image>::render(os::WindowModule*, GraphicsEntity* e) {
			Image* entity = dynamic_cast<Image*>(e);
			if( entity == nullptr ) {
				throw NullPointerException("You must provide an Image for RenderProtocol<Image>");
			}

			entity->getTexture().bind();

			ogl::UniformBuffer& textureData = entity->textureData;
			textureData.bind();
			textureData.bindToUniformBlock(renderer.getShader().getID(), "textureData");
			textureData.bindForRender();

			renderer.draw(e);
		}//render

		void RenderProtocol<Image>::tearDown(os::WindowModule*, RenderQueue*) {}

		void RenderProtocol<Image>::destroy() {
			renderer.destroy();
		}//destroy
#undef MACE_TEXTURE_DATA_BUFFER_SIZE
#undef MACE_TEXTURE_DATA_BUFFER_LOCATION
	}//gfx
}//mc
