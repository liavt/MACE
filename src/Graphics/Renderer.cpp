/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/OGL.h>

#ifdef MACE_GNU
//stb_image raises this warning and can be safely ignored
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(MACE_MSVC)
#	pragma warning( push ) 
//these are all warnings that STB_IMAGE activates which dont really matter
#	pragma warning( disable: 4244 4100 4456) 
#endif

#define STB_IMAGE_IMPLEMENTATION
#ifdef MACE_DEBUG
//	this macro makes more descriptive error messages
#	define STBI_FAILURE_USERMSG
#endif//MACE_DEBUG

#include <stb_image.h>

#ifdef MACE_GNU
#	pragma GCC diagnostic pop
#elif defined(MACE_MSVC)
#	pragma warning( pop )
#endif

//for testing purposes
#include <iostream>

namespace mc {
	namespace gfx {
		namespace {
			std::unique_ptr<Renderer> renderer = std::unique_ptr<Renderer>(nullptr);

			//when you request for a solid color Texture, it will use the same texture to save memory
			std::shared_ptr<TextureImpl> solidColor;
		}

		Texture::Texture() : texture(nullptr), paint(0.0f, 0.0f, 0.0f, 0.0f) {}

		Texture::Texture(const std::shared_ptr<TextureImpl> tex, const Color& col) : texture(tex), paint(col) {}

		Texture::Texture(const Texture & tex, const Color & col) : texture(tex.texture), paint(col) {}

		Texture::Texture(const char * file) : Texture() {
			init();
			load(file);
		}

		Texture::Texture(const std::string & file) : Texture(file.c_str()) {}

		Texture::Texture(const Color& col) : Texture(solidColor, col) {
			if (solidColor.get() == nullptr) {
				solidColor = getRenderer()->getTexture();
			}
			if (!solidColor->isCreated()) {
				solidColor->init();

				solidColor->resetPixelStorage();

				const float data[] = { 1,1,1,1 };
				solidColor->setData(data, 1, 1, gfx::Texture::Type::FLOAT, gfx::Texture::Format::RGBA, gfx::Texture::InternalFormat::RGBA, 0);

				solidColor->setMinFilter(Texture::ResizeFilter::NEAREST);
				solidColor->setMagFilter(Texture::ResizeFilter::NEAREST);

				texture = solidColor;
			}
		}

		void Texture::init() {
			if (texture == nullptr) {
				texture = getRenderer()->getTexture();
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
			texture->resetPixelStorage();

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

			ogl::checkGLError(__LINE__, __FILE__, "Error loading texture from file");
		}

		void Texture::load(const std::string & file) {
			load(file.c_str());
		}

		void Texture::load(const Color & c) {
			texture->resetPixelStorage();

			texture->setData(&c, 1, 1, gfx::Texture::Type::FLOAT, gfx::Texture::Format::RGBA, gfx::Texture::InternalFormat::RGBA, 0);

			texture->setMinFilter(Texture::ResizeFilter::NEAREST);
			texture->setMagFilter(Texture::ResizeFilter::NEAREST);

			ogl::checkGLError(__LINE__, __FILE__, "Error loading texture from color");
		}

		void Texture::load(const unsigned char * c, const Size size) {
			texture->resetPixelStorage();

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

			ogl::checkGLError(__LINE__, __FILE__, "Error loading texture from memory");
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
			texture->resetPixelStorage();
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

		bool Texture::operator==(const Texture& other) const {
			return paint == other.paint && texture == other.texture;
		}

		bool Texture::operator!=(const Texture& other) const {
			return !operator==(other);
		}

		Renderer * getRenderer() {
			return renderer.get();
		}

		void setRenderer(Renderer * r) {
			renderer.reset(r);
		}

		void Renderer::init(const Size width, const Size height) {
			originalWidth = width;
			originalHeight = height;

			onInit(width, height);
		}

		void Renderer::setUp(os::WindowModule* win) {
			if (resized) {
				int width, height;

				glfwGetFramebufferSize(win->getGLFWWindow(), &width, &height);

				resize(width, height);

				resized = false;
			}

			onSetUp(win);
		}//setUp

		Index Renderer::queue(GraphicsEntity * e) {
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}

			onQueue(e);

			//it adds 1 so 0 (NULL) represents a non-initalized value
			return pushEntity(e) + 1;
		}//queue

		void Renderer::remove(const Index id) {
			renderQueue[id - 1] = nullptr;
		}

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(const Size width, const Size height) {
			onResize(width, height);

			resized = false;
		}//resize

		void Renderer::tearDown(os::WindowModule* win) {
			onTearDown(win);
		}//tearDown

		void Renderer::checkInput(os::WindowModule*) {
			GraphicsEntity* hovered = getEntityAt(os::Input::getMouseX(), os::Input::getMouseY());

			if (hovered != nullptr) {
				hovered->hover();
			}

		}//checkInput

		void Renderer::destroy() {
			onDestroy();
		}//destroy()

		void Renderer::setRefreshColor(const Color & c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Size Renderer::getOriginalWidth() const {
			return originalWidth;
		}//getOriginalWidth()

		Size Renderer::getOriginalHeight() const {
			return originalHeight;
		}//getOriginalHeight()

		Size Renderer::getWidth() const {
			return static_cast<Size>(static_cast<float>(Renderer::getOriginalWidth()) * windowRatios[0]);
		}

		Size Renderer::getHeight() const {
			return static_cast<Size>(static_cast<float>(Renderer::getOriginalHeight()) * windowRatios[1]);
		}

		Size Renderer::getSamples() const {
			return samples;
		}//getSamples()

		Vector<float, 2> Renderer::getWindowRatios() const {
			return windowRatios;
		}

		RenderQueue Renderer::getRenderQueue() const {
			return renderQueue;
		}

		bool Renderer::isResized() const {
			return resized;
		}

		Index Renderer::pushEntity(GraphicsEntity * entity) {
			for (Index i = 0; i < renderQueue.size(); ++i) {
				if (renderQueue[i] == nullptr) {
					renderQueue[i] = entity;
					return i;
				}
			}
			renderQueue.push_back(entity);
			return renderQueue.size() - 1;
		}//pushEntity(protocol, entity)

		PainterImpl::PainterImpl(const GraphicsEntity * const en) : entity(en) {
			if (en == nullptr) {
				MACE__THROW(NullPointer, "Input to Painter was nullptr");
			}
		}

		void PainterImpl::maskImage(const Texture & img, const Texture & mask, const float minimum, const float maximum, const float x, const float y, const float w, const float h) {
			push();
			translate(x, y);
			scale(w, h);
			setColor(img.getPaint());
			setData({ minimum, maximum, 0.0f, 0.0f });
			loadSettings();
			img.bind(0);
			mask.bind(1);
			draw(Painter::Brush::MASK, Painter::RenderType::QUAD);
			pop();
		}

		const GraphicsEntity * const PainterImpl::getEntity() const {
			return entity;
		}

		void PainterImpl::fillRect(const float x, const float y, const float w, const float h) {
			push();
			translate(x, y);
			scale(w, h);
			loadSettings();
			draw(Painter::Brush::COLOR, Painter::RenderType::QUAD);
			pop();
		}

		void PainterImpl::fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			fillRect(pos.x(), pos.y(), size.x(), size.y());
		}

		void PainterImpl::fillRect(const Vector<float, 4>& dim) {
			fillRect(dim.x(), dim.y(), dim.z(), dim.w());
		}

		void PainterImpl::drawImage(const Texture & img, const float x, const float y, const float w, const float h) {
			push();
			translate(x, y);
			scale(w, h);
			setColor(img.getPaint());
			loadSettings();
			img.bind(0);
			draw(Painter::Brush::TEXTURE, Painter::RenderType::QUAD);
			pop();
		}

		void PainterImpl::drawImage(const Texture & img, const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			drawImage(img, pos.x(), pos.y(), size.x(), size.y());
		}

		void PainterImpl::drawImage(const Texture & img, const Vector<float, 4>& dim) {
			drawImage(img, dim.x(), dim.y(), dim.z(), dim.w());
		}

		void PainterImpl::setColor(const Color & col) {
			state.color = col;
		}

		Color & PainterImpl::getColor() {
			return state.color;
		}

		const Color & PainterImpl::getColor() const {
			return state.color;
		}

		void PainterImpl::setData(const Vector<float, 4> & col) {
			state.data = col;
		}

		Vector<float, 4> & PainterImpl::getData() {
			return state.data;
		}

		const Vector<float, 4> & PainterImpl::getData() const {
			return state.data;
		}

		void PainterImpl::resetColor() {
			state.color = Colors::INVISIBLE;
			state.data = { 0.0f, 0.0f, 0.0f, 0.0f };
		}

		void PainterImpl::setTransformation(const TransformMatrix & trans) {
			state.transformation = trans;
		}

		TransformMatrix & PainterImpl::getTransformation() {
			return state.transformation;
		}

		const TransformMatrix & PainterImpl::getTransformation() const {
			return state.transformation;
		}

		void PainterImpl::translate(const Vector<float, 3>& vec) {
			translate(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::translate(const float x, const float y, const float z) {
			state.transformation.translate(x, y, z);
		}

		void PainterImpl::rotate(const Vector<float, 3>& vec) {
			rotate(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::rotate(const float x, const float y, const float z) {
			state.transformation.rotate(x, y, z);
		}

		void PainterImpl::scale(const Vector<float, 3>& vec) {
			scale(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::scale(const float x, const float y, const float z) {
			state.transformation.scale(x, y, z);
		}

		void PainterImpl::resetTransform() {
			state.transformation.reset();
		}

		void PainterImpl::push() {
			stateStack.push(state);
		}

		void PainterImpl::pop() {
			state = stateStack.top();
			stateStack.pop();
		}

		void PainterImpl::reset() {
			resetColor();
			resetTransform();
		}

		bool PainterImpl::operator==(const PainterImpl & other) const {
			return entity == other.entity && state == other.state;
		}

		bool PainterImpl::operator!=(const PainterImpl & other) const {
			return !operator==(other);
		}

		Painter::Painter(GraphicsEntity * const en) {
			impl = getRenderer()->getPainter(en);
			if (impl.get() == nullptr) {
				MACE__THROW(NullPointer, "Renderer returned a nullptr to a Painter");
			}
		}

		void Painter::init() {
			impl->init();
		}

		void Painter::destroy() {
			impl->destroy();
		}

		std::shared_ptr<PainterImpl> Painter::getImplementation() {
			return impl;
		}

		const std::shared_ptr<PainterImpl> Painter::getImplementation() const {
			return impl;
		}

		std::shared_ptr<PainterImpl> Painter::operator*() {
			return impl;
		}

		const std::shared_ptr<PainterImpl> Painter::operator*() const {
			return impl;
		}

		std::shared_ptr<PainterImpl> Painter::operator->() {
			return impl;
		}

		const std::shared_ptr<PainterImpl> Painter::operator->() const {
			return impl;
		}

		bool Painter::State::operator==(const State & other) const {
			return transformation == other.transformation && color == other.color && data == other.data;
		}

		bool Painter::State::operator!=(const State & other) const {
			return !operator==(other);
		}
	}//gfx
}//mc
