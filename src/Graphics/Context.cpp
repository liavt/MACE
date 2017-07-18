/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Renderer.h>

#ifdef MACE_GCC
//stb_image raises this warning and can be safely ignored
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(MACE_MSVC)
#	pragma warning( push ) 
//these are all warnings that STB_IMAGE activates which dont really matter
#	pragma warning( disable: 4244 4100 4456 ) 
#endif

#define STB_IMAGE_IMPLEMENTATION
#ifdef MACE_DEBUG
//	this macro makes more descriptive error messages
#	define STBI_FAILURE_USERMSG
#endif//MACE_DEBUG

#include <stb_image.h>

#ifdef MACE_GCC
#	pragma GCC diagnostic pop
#elif defined(MACE_MSVC)
#	pragma warning( pop )
#endif

namespace mc {
	namespace gfx {
#define MACE__RESOURCE_SOLIDCOLOR "MACE/SolidColor"

		Texture::Texture() : texture(nullptr), paint(0.0f, 0.0f, 0.0f, 0.0f) {}

		Texture::Texture(const std::shared_ptr<TextureImpl> tex, const Color& col) : texture(tex), paint(col) {}

		Texture::Texture(const Texture & tex, const Color & col) : texture(tex.texture), paint(col) {}

		Texture::Texture(const char * file) : Texture() {
			init();
			load(file);
		}

		Texture::Texture(const std::string & file) : Texture(file.c_str()) {}

		Texture::Texture(const Color& col) : Texture(nullptr, col) {
			std::shared_ptr<GraphicsContext> context = os::getCurrentWindow()->getContext();
			//context is guarenteed to never be nullptr, and if currentwindow is nullptr, an exception will be thrown from os::getCurrentWindow()

			if (!context->hasTexture(MACE__RESOURCE_SOLIDCOLOR)) {
				texture = context->createTextureImpl();

				texture->init();

				resetPixelStorage();

				MACE_CONSTEXPR const float data[] = { 1 };
				texture->setData(data, 1, 1, gfx::Texture::Type::FLOAT, gfx::Texture::Format::RED, gfx::Texture::InternalFormat::RED, 0);

				texture->setSwizzle(gfx::Texture::SwizzleMode::G, gfx::Texture::SwizzleMode::R);
				texture->setSwizzle(gfx::Texture::SwizzleMode::B, gfx::Texture::SwizzleMode::R);
				texture->setSwizzle(gfx::Texture::SwizzleMode::A, gfx::Texture::SwizzleMode::R);

				texture->setMinFilter(Texture::ResizeFilter::NEAREST);
				texture->setMagFilter(Texture::ResizeFilter::NEAREST);

				context->createTexture(MACE__RESOURCE_SOLIDCOLOR, texture);
			} else {
				texture = context->getTexture(MACE__RESOURCE_SOLIDCOLOR).texture;
			}
		}

		void Texture::init() {
			if (texture == nullptr) {
				texture = os::getCurrentWindow()->getContext()->createTextureImpl();
			}
			texture->init();
		}

		void Texture::destroy() {
			texture->destroy();
		}

		bool Texture::isCreated() const {
			if (texture == nullptr) {
				return false;
			}

			return texture->isCreated();
		}

		void Texture::load(const char * file) {
			resetPixelStorage();

			int width, height, componentSize;

			Byte* image = stbi_load(file, &width, &height, &componentSize, STBI_rgb_alpha);

			try {
				if (image == nullptr || width == 0 || height == 0 || componentSize == 0) {
					stbi_image_free(image);
					MACE__THROW(BadImage, "Unable to read image: " + std::string(file) + '\n' + stbi_failure_reason());
				}

				texture->setData(image, width, height, gfx::Texture::Type::UNSIGNED_BYTE, gfx::Texture::Format::RGBA, gfx::Texture::InternalFormat::RGBA, 0);
			} catch (const std::exception& e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			texture->setMinFilter(Texture::ResizeFilter::MIPMAP_LINEAR);
			texture->setMagFilter(Texture::ResizeFilter::NEAREST);
		}

		void Texture::load(const std::string & file) {
			load(file.c_str());
		}

		void Texture::load(const Color & c) {
			resetPixelStorage();

			texture->setData(&c, 1, 1, gfx::Texture::Type::FLOAT, gfx::Texture::Format::RGBA, gfx::Texture::InternalFormat::RGBA, 0);

			texture->setMinFilter(Texture::ResizeFilter::NEAREST);
			texture->setMagFilter(Texture::ResizeFilter::NEAREST);
		}

		void Texture::load(const unsigned char * c, const Size size) {
			resetPixelStorage();

			int width, height, componentSize;

			Byte* image = stbi_load_from_memory(c, size, &width, &height, &componentSize, STBI_rgb_alpha);

			try {
				if (image == nullptr || width == 0 || height == 0 || componentSize == 0) {
					stbi_image_free(image);
					MACE__THROW(BadImage, "Unable to read image from memory: " + std::string(stbi_failure_reason()));
				}

				texture->setData(image, width, height, gfx::Texture::Type::UNSIGNED_BYTE, gfx::Texture::Format::RGBA, gfx::Texture::InternalFormat::RGBA, 0);
			} catch (const std::exception& e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			texture->setMinFilter(Texture::ResizeFilter::MIPMAP_LINEAR);
			texture->setMagFilter(Texture::ResizeFilter::NEAREST);
		}

		Color& Texture::getPaint() {
			return paint;
		}

		const Color& Texture::getPaint() const {
			return paint;
		}

		void Texture::setPaint(const Color& col) {
			paint = col;
		}

		void Texture::bind() const {
			texture->bind();
		}

		void Texture::bind(const Index location) const {
			texture->bind(location);
		}

		void Texture::unbind() const {
			texture->unbind();
		}

		void Texture::resetPixelStorage() {
			setPackStorageHint(Texture::PixelStorage::ALIGNMENT, 4);
			setUnpackStorageHint(Texture::PixelStorage::ALIGNMENT, 4);
			setPackStorageHint(Texture::PixelStorage::ROW_LENGTH, 0);
			setUnpackStorageHint(Texture::PixelStorage::ROW_LENGTH, 0);
		}

		void Texture::setMinFilter(const Texture::ResizeFilter filter) {
			texture->setMinFilter(filter);
		}

		void Texture::setMagFilter(const Texture::ResizeFilter filter) {
			texture->setMagFilter(filter);
		}

		void Texture::setData(const void * data, const Size width, const Size height, const Texture::Type type, const Texture::Format format, const Texture::InternalFormat internalFormat, const Index mipmap) {
			texture->setData(data, width, height, type, format, internalFormat, mipmap);
		}

		void Texture::setUnpackStorageHint(const Texture::PixelStorage hint, const int value) {
			texture->setUnpackStorageHint(hint, value);
		}

		void Texture::setPackStorageHint(const Texture::PixelStorage hint, const int value) {
			texture->setPackStorageHint(hint, value);
		}

		void Texture::setWrap(const Texture::WrapMode wrap) {
			setWrapS(wrap);
			setWrapT(wrap);
		}

		void Texture::setWrapS(const Texture::WrapMode wrap) {
			texture->setWrapS(wrap);
		}

		void Texture::setWrapT(const Texture::WrapMode wrap) {
			texture->setWrapT(wrap);
		}

		void Texture::setSwizzle(const Texture::SwizzleMode mode, const Texture::SwizzleMode arg) {
			texture->setSwizzle(mode, arg);
		}

		void Texture::getImage(const Texture::Format format, const Texture::Type type, void * data) const {
			texture->getImage(format, type, data);
		}

		bool Texture::operator==(const Texture& other) const {
			return paint == other.paint && texture == other.texture;
		}

		bool Texture::operator!=(const Texture& other) const {
			return !operator==(other);
		}

		os::WindowModule * GraphicsContext::getWindow() {
			return window;
		}

		const os::WindowModule * GraphicsContext::getWindow() const {
			return window;
		}

		void GraphicsContext::createTexture(const std::string & name, const Texture & texture) {
			if (hasTexture(name)) {
				MACE__THROW(AlreadyExists, "Texture with name " + name + " has already been created");
			}

			textures[name] = texture;
		}

		bool GraphicsContext::hasTexture(const std::string & name) const {
			return textures.count(name);
		}

		void GraphicsContext::setTexture(const std::string & name, const Texture & texture) {
			textures[name] = texture;
		}

		Texture & GraphicsContext::getTexture(const std::string & name) {
			return textures.at(name);
		}

		const Texture & GraphicsContext::getTexture(const std::string & name) const {
			return textures.at(name);
		}

		std::map<std::string, Texture>& GraphicsContext::getTextures() {
			return textures;
		}

		const std::map<std::string, Texture>& GraphicsContext::getTextures() const {
			return textures;
		}
		/*
		std::map<std::string, Model>& GraphicsContext::getModels() {
			return models;
		}

		const std::map<std::string, Model>& GraphicsContext::getModels() const {
			return models;
		}
		*/
		GraphicsContext::GraphicsContext(os::WindowModule * win) :window(win) {
#ifdef MACE_DEBUG
			if (window == nullptr) {
				MACE__THROW(NullPointer, "WindowModule inputted to GraphicsContext is nullptr");
			}
#endif
		}

		void GraphicsContext::init() {
			onInit(window);
			getRenderer()->context = this;
			getRenderer()->init(window);
		}

		void GraphicsContext::render() {
			onRender(window);

			getRenderer()->checkInput(window);
		}

		void GraphicsContext::destroy() {
			getRenderer()->destroy();
			onDestroy(window);
			window = nullptr;
		}
	}//gfx
}//mc