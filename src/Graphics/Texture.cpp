/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Texture.h>
#include <MACE/Graphics/Context.h>

namespace mc {
	namespace gfx {
#define MACE__VERIFY_TEXTURE_INIT() MACE_ASSERT(texture != nullptr, "This Texture has not had init() called yet")

		Texture::Texture() : texture(nullptr), hue(0.0f, 0.0f, 0.0f, 0.0f) {}

		Texture::Texture(GraphicsContextComponent* context, const TextureDesc& d) : Texture() {
			init(context, d);
		}

		Texture::Texture(const std::shared_ptr<TextureImpl> tex, const Color& col) : texture(tex), hue(col) {}

		Texture::Texture(const Texture& tex) : texture(tex.texture), hue(tex.hue), transform(tex.transform) {}

		Texture::Texture(const Texture& tex, const Color& col) : texture(tex.texture), transform(tex.transform), hue(col) {}

		Texture::Texture(GraphicsContextComponent* context, const Color& col) : Texture(context->getSolidColor(), col) {}

		void Texture::init(GraphicsContextComponent* context, const TextureDesc& desc) {
			if (desc.width == 0) {
				MACE__THROW(OutOfBounds, "Width of a Texture cannot be zero");
			} else if (desc.height == 0) {
				MACE__THROW(OutOfBounds, "Height of a Texture cannot be zero");
			} else if (desc.magFilter == TextureDesc::Filter::MIPMAP_LINEAR || desc.magFilter == TextureDesc::Filter::MIPMAP_NEAREST) {
				MACE__THROW(InvalidType, "Texture magnification filter can't be mipmapped");
			}

			//the old texture will be deallocated, and its destructor will be called and decrement ref count
			texture = context->createTextureImpl(desc);
		}

		void Texture::destroy() {
			texture.reset();
		}

		bool Texture::isCreated() const noexcept {
			return texture != nullptr;
		}

		const TextureDesc& Texture::getDesc() const {
			MACE__VERIFY_TEXTURE_INIT();

			return texture->desc;
		}

		Pixels Texture::getWidth() {
			return texture == nullptr ? 0 : texture->desc.width;
		}

		const Pixels Texture::getWidth() const {
			return texture == nullptr ? 0 : texture->desc.width;
		}

		Pixels Texture::getHeight() {
			return texture == nullptr ? 0 : texture->desc.height;
		}

		const Pixels Texture::getHeight() const {
			return texture == nullptr ? 0 : texture->desc.height;
		}

		Vector<Pixels, 2> mc::gfx::Texture::getDimensions() {
			return {getWidth(), getHeight()};
		}

		const Vector<Pixels, 2> mc::gfx::Texture::getDimensions() const {
			return {getWidth(), getHeight()};
		}


		Color& Texture::getHue() {
			return hue;
		}

		const Color& Texture::getHue() const {
			return hue;
		}

		void Texture::setHue(const Color& col) {
			hue = col;
		}

		Vector<RelativeUnit, 4> & Texture::getTransform() {
			return transform;
		}

		const Vector<RelativeUnit, 4> & Texture::getTransform() const {
			return transform;
		}

		void Texture::setTransform(const Vector<RelativeUnit, 4> & trans) {
			transform = trans;
		}

		void mc::gfx::Texture::setData(const void* data, const int x, const int y, const Pixels width, const Pixels height, const int mipmapLevel, const PixelStorageHints hints) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setData(data, x, y, width, height, mipmapLevel, hints);
		}

		void Texture::setData(const void* data, const int mipmap, const PixelStorageHints hints) {
			setData(data, 0, 0, texture->desc.width, texture->desc.height, mipmap, hints);
		}

		void Texture::readPixels(void* data, const PixelStorageHints hints) const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->readPixels(data, hints);
		}

		void Texture::bindTextureSlot(const TextureSlot slot) const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->bindTextureSlot(slot);
		}


		bool Texture::operator==(const Texture& other) const {
			return transform == other.transform && hue == other.hue && texture == other.texture;
		}

		bool Texture::operator!=(const Texture& other) const {
			return !operator==(other);
		}

		TextureImpl::TextureImpl(const TextureDesc& t) : desc(t) {}

		TextureDesc::TextureDesc(const unsigned int w, const unsigned int h, const Format form) : format(form), width(w), height(h) {}
	}//gfx
}//mc
