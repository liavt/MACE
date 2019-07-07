/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
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
#	pragma warning( disable: 4244 4100 4456 26451 6001 4296) 
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

		Model& Model::getQuad() {
			auto context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {
				return context->getOrCreateModel(MACE__RESOURCE_QUAD, []() {
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

		Texture Texture::create(const Color& col, const Pixels width, const Pixels height) {
			TextureDesc desc = TextureDesc(width, height, TextureDesc::Format::RGBA);
			desc.minFilter = TextureDesc::Filter::NEAREST;
			desc.magFilter = TextureDesc::Filter::NEAREST;
			desc.type = TextureDesc::Type::UNSIGNED_INT;
			desc.internalFormat = TextureDesc::InternalFormat::RGBA8;

			Texture texture = Texture(desc);

			const Size newSize = static_cast<Size>(width) * static_cast<Size>(height);
			std::vector<unsigned int> data = std::vector<unsigned int>();
			data.reserve(newSize);
			for (Index i = 0; i < newSize; ++i) {
				data[i] = col.toUnsignedInt();
			}
			texture.setData(&data[0]);

			return texture;
		}

		Texture Texture::createFromFile(const std::string& file, const ImageFormat format, const TextureDesc::Wrap wrap) {
			return Texture::createFromFile(file.c_str(), format, wrap);
		}

		Texture Texture::createFromFile(const CString file, const ImageFormat imgFormat, const TextureDesc::Wrap wrap) {
			Texture tex = Texture();

			int width, height, actualComponents;
			Byte* image = stbi_load(file, &width, &height, &actualComponents, static_cast<int>(imgFormat));

			try {
				if (image == nullptr || width == 0 || height == 0 || actualComponents == 0) {
					MACE__THROW(BadImage, "Unable to read image: " + std::string(file) + '\n' + stbi_failure_reason());
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
					MACE__THROW(BadImage, "Internal Error: createFromFile: outputComponents is not 1-4");
				}
				desc.type = TextureDesc::Type::UNSIGNED_BYTE;
				desc.wrapS = wrap;
				desc.wrapT = wrap;
				desc.minFilter = TextureDesc::Filter::MIPMAP_LINEAR;
				desc.magFilter = TextureDesc::Filter::NEAREST;
				tex.init(desc);

				tex.setData(image);
			} catch (const std::exception& e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			return tex;
		}

		Texture Texture::createFromMemory(const unsigned char* c, const Size size) {
			Texture texture = Texture();
			int width, height, componentSize;

			Byte* image = stbi_load_from_memory(c, static_cast<int>(size), &width, &height, &componentSize, STBI_rgb_alpha);

			try {
				if (image == nullptr || width == 0 || height == 0 || componentSize == 0) {
					MACE__THROW(BadImage, "Unable to read image from memory: " + std::string(stbi_failure_reason()));
				}

				TextureDesc desc = TextureDesc(width, height, TextureDesc::Format::RGBA);
				desc.type = TextureDesc::Type::UNSIGNED_BYTE;
				desc.internalFormat = TextureDesc::InternalFormat::RGBA8;
				desc.minFilter = TextureDesc::Filter::MIPMAP_LINEAR;
				desc.magFilter = TextureDesc::Filter::NEAREST;
				texture.init(desc);

				texture.setData(image);
			} catch (const std::exception& e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			return texture;
		}

		Texture& Texture::getSolidColor() {
			auto context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {
				return context->getOrCreateTexture(MACE__RESOURCE_SOLIDCOLOR, []() {
					TextureDesc desc = TextureDesc(1, 1, TextureDesc::Format::LUMINANCE);
					desc.minFilter = TextureDesc::Filter::NEAREST;
					desc.magFilter = TextureDesc::Filter::NEAREST;
					desc.type = TextureDesc::Type::FLOAT;
					desc.internalFormat = TextureDesc::InternalFormat::R8;

					Texture texture = Texture(desc);

					MACE_CONSTEXPR const float data[] = {1.0f};
					texture.setData(data);

					return texture;
				});
			}
		}

		Texture& Texture::getGradient() {
			auto context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {


				return context->getOrCreateTexture(MACE__RESOURCE_GRADIENT, []() {
					TextureDesc desc = TextureDesc(1, MACE__RESOURCE_GRADIENT_HEIGHT);
					desc.format = TextureDesc::Format::LUMINANCE;
					desc.type = TextureDesc::Type::FLOAT;
					desc.internalFormat = TextureDesc::InternalFormat::R8;
					desc.minFilter = TextureDesc::Filter::LINEAR;
					desc.magFilter = TextureDesc::Filter::NEAREST;
					Texture texture = Texture(desc);

					float data[MACE__RESOURCE_GRADIENT_HEIGHT];
					for (unsigned int i = 0; i < MACE__RESOURCE_GRADIENT_HEIGHT; ++i) {
						//the darker part is on the bottom
						data[i] = static_cast<float>(MACE__RESOURCE_GRADIENT_HEIGHT - i) / static_cast<float>(MACE__RESOURCE_GRADIENT_HEIGHT);
					}
					texture.setData(data);

					return texture;
				});
			}
		}

		Texture::Texture() : texture(nullptr), hue(0.0f, 0.0f, 0.0f, 0.0f) {}

		Texture::Texture(const TextureDesc& d) : Texture() {
			init(d);
		}

		Texture::Texture(const std::shared_ptr<TextureImpl> tex, const Color& col) : texture(tex), hue(col) {}

		Texture::Texture(const Texture& tex) : texture(tex.texture), hue(tex.hue), transform(tex.transform) {}

		Texture::Texture(const Texture& tex, const Color& col) : texture(tex.texture), transform(tex.transform), hue(col) {}

		Texture::Texture(const Color& col) : Texture(Texture::getSolidColor(), col) {}

		void Texture::init(const TextureDesc& desc) {
			if (desc.width == 0) {
				MACE__THROW(OutOfBounds, "Width of a Texture cannot be zero");
			} else if (desc.height == 0) {
				MACE__THROW(OutOfBounds, "Height of a Texture cannot be zero");
			}

			//the old texture will be deallocated, and its destructor will be called and decrement ref count
			texture = gfx::getCurrentWindow()->getContext()->createTextureImpl(desc);
		}

		void Texture::destroy() {
			texture.reset();
		}

		bool Texture::isCreated() const {
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

		Texture& GraphicsContextComponent::createTexture(const std::string& name, const Texture& texture) {
			if (hasTexture(name)) {
				MACE__THROW(AlreadyExists, "Texture with name " + name + " has already been created");
			}

			return textures[name] = texture;
		}

		Texture& GraphicsContextComponent::getOrCreateTexture(const std::string& name, const TextureCreateCallback create) {
			if (!hasTexture(name)) {
				return createTexture(name, create());
			} else {
				return getTexture(name);
			}
		}

		Texture& GraphicsContextComponent::getOrCreateTextureFromFile(const std::string& name, const std::string& path) {
			return getOrCreateTexture(name, [&path]() {
				return Texture::createFromFile(path);
			});
		}

		Model& GraphicsContextComponent::createModel(const std::string& name, const Model& mod) {
			if (hasModel(name)) {
				MACE__THROW(AlreadyExists, "Model with name " + name + " has already been created");
			}

			return models[name] = mod;
		}

		Model& GraphicsContextComponent::getOrCreateModel(const std::string& name, const ModelCreateCallback create) {
			if (!hasModel(name)) {
				return createModel(name, create());
			} else {
				return getModel(name);
			}
		}

		bool GraphicsContextComponent::hasTexture(const std::string& name) const {
			//map.count() returns 1 if key exists, 0 otherwise.
			return textures.count(name) != 0;//the verbosity is to suppress warnings of casting from std::size_t to bool
		}

		bool GraphicsContextComponent::hasModel(const std::string& name) const {
			return models.count(name) != 0;
		}

		void GraphicsContextComponent::setTexture(const std::string& name, const Texture& texture) {
			textures[name] = texture;
		}

		Texture& GraphicsContextComponent::getTexture(const std::string& name) {
			return textures.at(name);
		}

		const Texture& GraphicsContextComponent::getTexture(const std::string& name) const {
			return textures.at(name);
		}

		void GraphicsContextComponent::setModel(const std::string& name, const Model& model) {
			models[name] = model;
		}

		Model& GraphicsContextComponent::getModel(const std::string& name) {
			return models.at(name);
		}

		const Model& GraphicsContextComponent::getModel(const std::string& name) const {
			return models.at(name);
		}

		std::map<std::string, Texture>& GraphicsContextComponent::getTextures() {
			return textures;
		}

		const std::map<std::string, Texture>& GraphicsContextComponent::getTextures() const {
			return textures;
		}

		std::map<std::string, Model>& GraphicsContextComponent::getModels() {
			return models;
		}

		const std::map<std::string, Model>& GraphicsContextComponent::getModels() const {
			return models;
		}

		GraphicsContextComponent::GraphicsContextComponent(gfx::WindowModule* win) :window(win) {
			MACE_ASSERT(window != nullptr, "WindowModule inputted to GraphicsContextComponent is nullptr");
		}

		void GraphicsContextComponent::init() {
			onInit(window);
			getRenderer()->context = this;
			getRenderer()->init(window);
		}

		void GraphicsContextComponent::render() {
			onRender(window);
		}

		void GraphicsContextComponent::destroy() {
			for (auto& x : textures) {
				if (x.second.isCreated()) {
					x.second.destroy();
				}
			}

			for (auto& x : models) {
				if (x.second.isCreated()) {
					x.second.destroy();
				}
			}

			getRenderer()->destroy();
			onDestroy(window);
			window = nullptr;
		}

		TextureImpl::TextureImpl(const TextureDesc& t) : desc(t) {}

		TextureDesc::TextureDesc(const unsigned int w, const unsigned int h, const Format form) : format(form), width(w), height(h) {}
	}//gfx
}//mc
