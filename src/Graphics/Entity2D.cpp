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
		namespace {
			int IMAGE_PROTOCOL = -1;

			ogl::Texture whiteTexture;
		}//anon namespace

		Entity2D::Entity2D() : GraphicsEntity() {}

		Image::Image() noexcept {}

		Image::Image(const ogl::Texture & tex) : Image() {
			texture = tex;
		}

		Image::Image(const Color & col) : Image(whiteTexture){
			if( !texture.isCreated() ) {
				texture.init();

				float data[] = { 1,1,1,1 };

				texture.setData(data, 1, 1, GL_FLOAT, GL_RGBA);

				texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			setPaint(col);
		}

		void Image::onInit() {
			if( !texture.isCreated() ) {
				texture.destroy();
			}

			if( IMAGE_PROTOCOL < 0 ) {
				IMAGE_PROTOCOL = Renderer::registerProtocol<Image>();
			}
		}

		void Image::onUpdate() {}

		void Image::onRender() {
			Renderer::queue(this, IMAGE_PROTOCOL);
		}

		void Image::onDestroy() {
			if( texture.isCreated() ) {
				texture.destroy();
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
			return Entity2D::operator==(other) && texture == other.texture;
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

			entity->texture.bind();

			renderer.draw(e);
		}//render

		void RenderProtocol<Image>::tearDown(os::WindowModule*, RenderQueue*) {}

		void RenderProtocol<Image>::destroy() {
			renderer.destroy();
		}//destroy
#undef MACE_TEXTURE_DATA_BUFFER_SIZE
#undef MACE_TEXTURE_DATA_BUFFER_LOCATION
		int getImageProtocol() {
			return IMAGE_PROTOCOL;
		}//getImageProtocol
	}//gfx
}//mc
