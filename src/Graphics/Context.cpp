/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Components.h>

#ifdef MACE_GCC
//stb_image raises this warning and can be safely ignored
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(MACE_MSVC)
#	pragma warning( push ) 
//these are all warnings that STB_IMAGE activates which dont really matter
#	pragma warning( disable: 4244 4100 4456 26451 6001 4296) 
#endif

#define STB_IMAGE_IMPLEMENTATION
#ifdef MACE_DEBUG
//	this macro makes more descriptive error messages
#	define STBI_FAILURE_USERMSG
#endif//MACE_DEBUG

#define STBI_ASSERT(x) MACE_ASSERT(x, "stb_image assert")

#include <stb_image.h>

#ifdef MACE_GCC
#	pragma GCC diagnostic pop
#elif defined(MACE_MSVC)
#	pragma warning( pop )
#endif

//for debug purposes
#include <iostream>

namespace mc {
	namespace gfx {
#define MACE__VERIFY_TEXTURE_INIT() MACE_ASSERT(texture != nullptr, "This Texture has not had init() called yet")
#define MACE__VERIFY_MODEL_INIT() MACE_ASSERT(model != nullptr, "This Model has not had init() called yet")

#define MACE__RESOURCE_PREFIX "MC/"

		//these are the names for cached resources in the GraphicsContextComponent
#define MACE__RESOURCE_SOLIDCOLOR MACE__RESOURCE_PREFIX "SolidColor"
#define MACE__RESOURCE_GRADIENT MACE__RESOURCE_PREFIX "Gradient"
		//how many pixels in the gradient
#define MACE__RESOURCE_GRADIENT_HEIGHT 128
#define MACE__RESOURCE_QUAD MACE__RESOURCE_PREFIX "Quad"

		bool ModelImpl::operator==(const ModelImpl & other) const {
			return primitiveType == other.primitiveType;
		}

		bool ModelImpl::operator!=(const ModelImpl& other) const {
			return !operator==(other);
		}

		Model::Model() : model(nullptr) {}

		Model::Model(const std::shared_ptr<ModelImpl> mod) : model(mod) {}

		Model::Model(const Model& other) : model(other.model) {}

		void Model::init() {
			model = gfx::getCurrentWindow()->getContext()->createModelImpl();
		}

		void Model::destroy() {
			model.reset();
		}

		void Model::createTextureCoordinates(const unsigned int dataSize, const float* data) {
			MACE__VERIFY_MODEL_INIT();

			model->loadTextureCoordinates(dataSize, data);
		}

		void Model::createVertices(const unsigned int verticeSize, const float* vertices, const PrimitiveType& prim) {
			MACE__VERIFY_MODEL_INIT();

			model->primitiveType = prim;
			model->loadVertices(verticeSize, vertices);

		}

		void Model::createIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
			MACE__VERIFY_MODEL_INIT();

			model->loadIndices(indiceNum, indiceData);
		}

		PrimitiveType Model::getPrimitiveType() {
			return model->primitiveType;
		}

		const PrimitiveType Model::getPrimitiveType() const {
			return model->primitiveType;
		}

		void Model::draw() const {
			MACE__VERIFY_MODEL_INIT();

			model->draw();
		}

		bool Model::isCreated() const {
			return model != nullptr;
		}

		bool Model::operator==(const Model& other) const {
			return model == other.model;
		}

		bool Model::operator!=(const Model& other) const {
			return !operator==(other);
		}

		Texture GraphicsContextComponent::createTextureFromColor(const Color& col, const Pixels width, const Pixels height) {
			TextureDesc desc = TextureDesc(width, height, TextureDesc::Format::RGBA);
			desc.minFilter = TextureDesc::Filter::NEAREST;
			desc.magFilter = TextureDesc::Filter::NEAREST;
			desc.type = TextureDesc::Type::UNSIGNED_INT;
			desc.internalFormat = TextureDesc::InternalFormat::RGBA8;

			Texture texture = Texture(this, desc);

			const auto data = std::vector<unsigned int>(static_cast<Size>(width) * static_cast<Size>(height), col.toUnsignedInt());
			texture.setData(&data[0]);

			return texture;
		}

		Texture GraphicsContextComponent::createTextureFromFile(const std::string& file, const ImageFormat format, const TextureDesc::Wrap wrap) {
			return createTextureFromFile(file.c_str(), format, wrap);
		}

		Texture GraphicsContextComponent::createTextureFromFile(const CString file, const ImageFormat imgFormat, const TextureDesc::Wrap wrap) {
			Texture tex = Texture();

			int width, height, actualComponents;
			auto image = std::unique_ptr<Byte, decltype(&stbi_image_free)>(stbi_load(file, &width, &height, &actualComponents, static_cast<int>(imgFormat)), stbi_image_free);

			if (image == nullptr || width == 0 || height == 0 || actualComponents == 0) {
				MACE__THROW(gfx::BadImage, "Unable to read image: " + std::string(file) + '\n' + stbi_failure_reason());
			}

			/*if DONT_CARE, the outputComponents is equal to the amount of components in image,
			otherwise equal to amount of requestedComponents
			*/
			const int outputComponents = (imgFormat == ImageFormat::DONT_CARE
										  ? actualComponents : static_cast<int>(imgFormat));

			TextureDesc desc = TextureDesc(width, height);
			if (outputComponents == 1) {
				if (imgFormat == ImageFormat::LUMINANCE) {
					desc.format = TextureDesc::Format::LUMINANCE;
				} else if (imgFormat == ImageFormat::INTENSITY) {
					desc.format = TextureDesc::Format::INTENSITY;
				} else {
					desc.format = TextureDesc::Format::RED;
				}
				desc.internalFormat = TextureDesc::InternalFormat::R8;
			} else if (outputComponents == 2) {
				if (imgFormat == ImageFormat::LUMINANCE_ALPHA) {
					desc.format = TextureDesc::Format::LUMINANCE_ALPHA;
				} else {
					desc.format = TextureDesc::Format::RG;
				}
				desc.internalFormat = TextureDesc::InternalFormat::RG8;
			} else if (outputComponents == 3) {
				desc.format = TextureDesc::Format::RGB;
				desc.internalFormat = TextureDesc::InternalFormat::RGB8;
			} else if (outputComponents == 4) {
				desc.format = TextureDesc::Format::RGBA;
				desc.internalFormat = TextureDesc::InternalFormat::RGBA8;
			} else MACE_UNLIKELY{
				MACE__THROW(gfx::BadImage, "Internal Error: createFromFile: outputComponents is not 1-4");
			}
			desc.type = TextureDesc::Type::UNSIGNED_BYTE;
			desc.wrapS = wrap;
			desc.wrapT = wrap;
			desc.minFilter = TextureDesc::Filter::MIPMAP_LINEAR;
			desc.magFilter = TextureDesc::Filter::NEAREST;
			tex.init(this, desc);

			tex.setData(image.get());

			return tex;
		}

		Texture GraphicsContextComponent::createTextureFromMemory(const unsigned char* c, const Size size) {
			Texture texture = Texture();
			int width, height, componentSize;

			auto image = std::unique_ptr<Byte, decltype(&stbi_image_free)>(stbi_load_from_memory(c, static_cast<int>(size), &width, &height, &componentSize, STBI_rgb_alpha), &stbi_image_free);

			if (image == nullptr || width == 0 || height == 0 || componentSize == 0) {
				MACE__THROW(gfx::BadImage, "Unable to read image from memory: " + std::string(stbi_failure_reason()));
			}

			TextureDesc desc = TextureDesc(width, height, TextureDesc::Format::RGBA);
			desc.type = TextureDesc::Type::UNSIGNED_BYTE;
			desc.internalFormat = TextureDesc::InternalFormat::RGBA8;
			desc.minFilter = TextureDesc::Filter::MIPMAP_LINEAR;
			desc.magFilter = TextureDesc::Filter::NEAREST;
			texture.init(this, desc);

			texture.setData(image.get());

			return texture;
		}

		Model GraphicsContextComponent::getQuad() {
			return parent->getOrCreateComponent<CacheComponent<Model>>()->getOrCreate(MACE__RESOURCE_QUAD, []() {
				Model model = Model();
				model.init();

				MACE_CONSTEXPR const float squareTextureCoordinates[8] = {
					0.0f,1.0f,
					0.0f,0.0f,
					1.0f,0.0f,
					1.0f,1.0f,
				};

				MACE_CONSTEXPR const unsigned int squareIndices[6] = {
					0,1,3,
					1,2,3
				};

				MACE_CONSTEXPR const float squareVertices[12] = {
					-1.0f,-1.0f,0.0f,
					-1.0f,1.0f,0.0f,
					1.0f,1.0f,0.0f,
					1.0f,-1.0f,0.0f
				};

				model.createVertices(squareVertices, PrimitiveType::TRIANGLES);
				model.createIndices(squareIndices);
				model.createTextureCoordinates(squareTextureCoordinates);

				return model;
			});
		}

		Texture GraphicsContextComponent::getSolidColor(const Color& col) {
			return Texture(parent->getOrCreateComponent<CacheComponent<Texture>>()->getOrCreate(MACE__RESOURCE_SOLIDCOLOR, [this]() {
				TextureDesc desc = TextureDesc(1, 1, TextureDesc::Format::LUMINANCE);
				desc.minFilter = TextureDesc::Filter::NEAREST;
				desc.magFilter = TextureDesc::Filter::NEAREST;
				desc.type = TextureDesc::Type::FLOAT;
				desc.internalFormat = TextureDesc::InternalFormat::R8;

				Texture texture = Texture(this, desc);

				MACE_CONSTEXPR const float data[] = {1.0f};
				texture.setData(data);

				return texture;
			}), col);
		}

		Texture GraphicsContextComponent::getGradient() {
			auto val = parent->getOrCreateComponent<CacheComponent<Texture>>();
			val->has("HEE");
			return parent->getOrCreateComponent<CacheComponent<Texture>>()->getOrCreate(MACE__RESOURCE_GRADIENT, [this]() {
				TextureDesc desc = TextureDesc(1, MACE__RESOURCE_GRADIENT_HEIGHT);
				desc.format = TextureDesc::Format::LUMINANCE;
				desc.type = TextureDesc::Type::FLOAT;
				desc.internalFormat = TextureDesc::InternalFormat::R8;
				desc.minFilter = TextureDesc::Filter::LINEAR;
				desc.magFilter = TextureDesc::Filter::NEAREST;
				Texture texture = Texture(this, desc);

				float data[MACE__RESOURCE_GRADIENT_HEIGHT];
				for (unsigned int i = 0; i < MACE__RESOURCE_GRADIENT_HEIGHT; ++i) {
					//the darker part is on the bottom
					data[i] = static_cast<float>(MACE__RESOURCE_GRADIENT_HEIGHT - i) / static_cast<float>(MACE__RESOURCE_GRADIENT_HEIGHT);
				}
				texture.setData(data);

				return texture;
			});
		}

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

		gfx::WindowModule* GraphicsContextComponent::getWindow() {
			return window;
		}

		const gfx::WindowModule* GraphicsContextComponent::getWindow() const {
			return window;
		}

		GraphicsContextComponent::GraphicsContextComponent(gfx::WindowModule* win) :window(win) {
			MACE_ASSERT(window != nullptr, "WindowModule inputted to GraphicsContextComponent is nullptr");
		}

		void GraphicsContextComponent::init() {
			onInit(window);
			getRenderer()->init(window);
		}

		void GraphicsContextComponent::render() {
			onRender(window);
		}

		void GraphicsContextComponent::destroy() {
			getRenderer()->destroy();
			onDestroy(window);
			window = nullptr;
		}

		TextureImpl::TextureImpl(const TextureDesc& t) : desc(t) {}

		TextureDesc::TextureDesc(const unsigned int w, const unsigned int h, const Format form) : format(form), width(w), height(h) {}
	}//gfx
}//mc
