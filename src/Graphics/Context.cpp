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
#ifdef MACE_DEBUG
#	define MACE__VERIFY_TEXTURE_INIT() do{if(texture == nullptr){ MACE__THROW(InvalidState, "This Texture has not had init() called yet"); }}while(0)
#	define MACE__VERIFY_MODEL_INIT() do{if(model == nullptr){ MACE__THROW(InvalidState, "This Model has not had init() called yet"); }}while(0)
#else
#	define MACE__VERIFY_TEXTURE_INIT()
#	define MACE__VERIFY_MODEL_INIT()
#endif

#define MACE__RESOURCE_PREFIX "MC/"

		//these are the names for cached resources in the GraphicsContext
#define MACE__RESOURCE_SOLIDCOLOR MACE__RESOURCE_PREFIX "SolidColor"
#define MACE__RESOURCE_GRADIENT MACE__RESOURCE_PREFIX "Gradient"
		//how many pixels in the gradient
#define MACE__RESOURCE_GRADIENT_HEIGHT 128
#define MACE__RESOURCE_QUAD MACE__RESOURCE_PREFIX "Quad"

		bool ModelImpl::operator==(const ModelImpl& other) const {
			return primitiveType == other.primitiveType;
		}

		bool ModelImpl::operator!=(const ModelImpl& other) const {
			return !operator==(other);
		}

		Model& Model::getQuad() {
			GraphicsContext* context = gfx::getCurrentWindow()->getContext();
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
			if (model == nullptr) {
				model = gfx::getCurrentWindow()->getContext()->createModelImpl();
			}

			model->init();
		}

		void Model::destroy() {
			MACE__VERIFY_MODEL_INIT();

			model->destroy();
		}

		void Model::bind() const {
			MACE__VERIFY_MODEL_INIT();

			model->bind();
		}

		void Model::unbind() const {
			MACE__VERIFY_MODEL_INIT();

			model->unbind();
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
			MACE__VERIFY_MODEL_INIT();

			return model->isCreated();
		}

		bool Model::operator==(const Model& other) const {
			return model == other.model;
		}

		bool Model::operator!=(const Model& other) const {
			return !operator==(other);
		}

		Texture Texture::create(const Color& col, const unsigned int width, const unsigned int height) {
			TextureDesc desc = TextureDesc(width, height, TextureDesc::Format::RGBA);
			desc.minFilter = TextureDesc::Filter::NEAREST;
			desc.magFilter = TextureDesc::Filter::NEAREST;
			desc.type = TextureDesc::Type::UNSIGNED_INT;
			desc.internalFormat = TextureDesc::InternalFormat::RGBA;

			Texture texture = Texture(desc);

			texture.resetPixelStorage();

			const size_t newSize = static_cast<size_t>(width) * static_cast<size_t>(height);
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

		Texture Texture::createFromFile(const char* file, const ImageFormat imgFormat, const TextureDesc::Wrap wrap) {
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
					desc.internalFormat = TextureDesc::InternalFormat::RED;
				} else if (outputComponents == 2) {
					if (imgFormat == ImageFormat::LUMINANCE_ALPHA) {
						desc.format = TextureDesc::Format::LUMINANCE_ALPHA;
					} else {
						desc.format = TextureDesc::Format::RG;
					}
					desc.internalFormat = TextureDesc::InternalFormat::RG;
				} else if (outputComponents == 3) {
					desc.format = TextureDesc::Format::RGB;
					desc.internalFormat = TextureDesc::InternalFormat::RGB;
				} else if (outputComponents == 4) {
					desc.format = TextureDesc::Format::RGBA;
					desc.internalFormat = TextureDesc::InternalFormat::RGBA;
				} else MACE_UNLIKELY {
					MACE__THROW(BadImage, "Internal Error: createFromFile: outputComponents is not 1-4");
				}
				desc.type = TextureDesc::Type::UNSIGNED_BYTE;
				desc.wrapS = wrap;
				desc.wrapT = wrap;
				desc.minFilter = TextureDesc::Filter::MIPMAP_LINEAR;
				desc.magFilter = TextureDesc::Filter::NEAREST;
				tex.init(desc);

				tex.resetPixelStorage();
				tex.setData(image);
			} catch (const std::exception & e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			return tex;
		}

		Texture Texture::createFromMemory(const unsigned char* c, const int size) {
			Texture texture = Texture();
			int width, height, componentSize;

			Byte* image = stbi_load_from_memory(c, size, &width, &height, &componentSize, STBI_rgb_alpha);

			try {
				if (image == nullptr || width == 0 || height == 0 || componentSize == 0) {
					MACE__THROW(BadImage, "Unable to read image from memory: " + std::string(stbi_failure_reason()));
				}

				TextureDesc desc = TextureDesc(width, height, TextureDesc::Format::RGBA);
				desc.type = TextureDesc::Type::UNSIGNED_BYTE;
				desc.internalFormat = TextureDesc::InternalFormat::RGBA;
				desc.minFilter = TextureDesc::Filter::MIPMAP_LINEAR;
				desc.magFilter = TextureDesc::Filter::NEAREST;
				texture.init(desc);

				texture.resetPixelStorage();

				texture.setData(image);
			} catch (const std::exception & e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			return texture;
		}

		/*
		Texture Texture::createGradient(const GradientDesc & gradDesc) {
			unsigned int colorComponents = 0;
			TextureDesc desc = TextureDesc(gradDesc.width, gradDesc.height);
			switch (gradDesc.colorSetting) {
				case GradientDesc::ColorSetting::R:
					desc.format = TextureDesc::Format::LUMINANCE;
					desc.internalFormat = TextureDesc::InternalFormat::RED;
					colorComponents = 1;
					break;
				case GradientDesc::ColorSetting::RA:
					desc.format = TextureDesc::Format::LUMINANCE_ALPHA;
					desc.internalFormat = TextureDesc::InternalFormat::RG;
					colorComponents = 2;
					break;
				case GradientDesc::ColorSetting::RG:
					desc.format = TextureDesc::Format::RG;
					desc.internalFormat = TextureDesc::InternalFormat::RG;
					colorComponents = 2;
					break;
				case GradientDesc::ColorSetting::RGB:
					desc.format = TextureDesc::Format::RGB;
					desc.internalFormat = TextureDesc::InternalFormat::RGB;
					colorComponents = 3;
					break;
				case GradientDesc::ColorSetting::RGBA:
					MACE_FALLTHROUGH;
				default:
					desc.format = TextureDesc::Format::RGBA;
					desc.internalFormat = TextureDesc::InternalFormat::RGBA;
					colorComponents = 4;
					break;
			}
			desc.type = TextureDesc::Type::FLOAT;
			desc.minFilter = TextureDesc::Filter::LINEAR;
			desc.magFilter = TextureDesc::Filter::NEAREST;
			Texture texture = Texture(desc);

			texture.resetPixelStorage();

			std::vector<float> data;
			data.resize(gradDesc.width * gradDesc.height * colorComponents);
			for (unsigned int x = 0; x < gradDesc.width; ++x) {
				for (unsigned int y = 0; y < gradDesc.height; ++y) {
					const float progressX = static_cast<float>(x) / static_cast<float>(gradDesc.width),
						progressY = static_cast<float>(y) / static_cast<float>(gradDesc.height);
					for (unsigned int i = 0; i < colorComponents; ++i) {
						const float valueLeft = (gradDesc.topLeft[i] * progressY) + (gradDesc.botLeft[i] * (1.0f - progressY));
						const float value = valueLeft;
						data[((x * gradDesc.height) + y) * (i + 1)] = 1.0f;
					}
					std::cout << std::endl;
				}
			}

			texture.setData(data.data());
			return texture;
		}
		*/

		Texture& Texture::getSolidColor() {
			GraphicsContext* context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {
				return context->getOrCreateTexture(MACE__RESOURCE_SOLIDCOLOR, []() {
					TextureDesc desc = TextureDesc(1, 1, TextureDesc::Format::LUMINANCE);
					desc.minFilter = TextureDesc::Filter::NEAREST;
					desc.magFilter = TextureDesc::Filter::NEAREST;
					desc.type = TextureDesc::Type::FLOAT;
					desc.internalFormat = TextureDesc::InternalFormat::RED;

					Texture texture = Texture(desc);

					texture.resetPixelStorage();

					MACE_CONSTEXPR const float data[] = {1.0f};
					texture.setData(data);

					return texture;
				});
			}
		}

		Texture& Texture::getGradient() {
			GraphicsContext* context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {


				return context->getOrCreateTexture(MACE__RESOURCE_GRADIENT, []() {
					TextureDesc desc = TextureDesc(1, MACE__RESOURCE_GRADIENT_HEIGHT);
					desc.format = TextureDesc::Format::LUMINANCE;
					desc.type = TextureDesc::Type::FLOAT;
					desc.internalFormat = TextureDesc::InternalFormat::RED;
					desc.minFilter = TextureDesc::Filter::LINEAR;
					desc.magFilter = TextureDesc::Filter::NEAREST;
					Texture texture = Texture(desc);

					texture.resetPixelStorage();


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

		Texture::Texture(const TextureDesc & d) {
			init(d);
		}

		Texture::Texture(const std::shared_ptr<TextureImpl> tex, const Color & col) : texture(tex), hue(col) {}

		Texture::Texture(const Texture & tex, const Color & col) : texture(tex.texture), hue(col) {}

		Texture::Texture(const Color & col) : Texture(Texture::getSolidColor(), col) {}

		void Texture::init(const TextureDesc & desc) {
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

		unsigned int Texture::getWidth() {
			return texture == nullptr ? 0 : texture->desc.width;
		}

		const unsigned int Texture::getWidth() const {
			return texture == nullptr ? 0 : texture->desc.width;
		}

		unsigned int Texture::getHeight() {
			return texture == nullptr ? 0 : texture->desc.height;
		}

		const unsigned int Texture::getHeight() const {
			return texture == nullptr ? 0 : texture->desc.height;
		}


		Color& Texture::getHue() {
			return hue;
		}

		const Color& Texture::getHue() const {
			return hue;
		}

		void Texture::setHue(const Color & col) {
			hue = col;
		}

		Vector<float, 4>& Texture::getTransform() {
			return transform;
		}

		const Vector<float, 4>& Texture::getTransform() const {
			return transform;
		}

		void Texture::setTransform(const Vector<float, 4> & trans) {
			transform = trans;
		}

		void Texture::bind() const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->bind();
		}

		void Texture::bind(const TextureSlot slot) const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->bind(slot);
		}

		void Texture::unbind() const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->unbind();
		}

		void Texture::resetPixelStorage() {
			MACE__VERIFY_TEXTURE_INIT();

			setPackStorageHint(PixelStorage::ALIGNMENT, 4);
			setUnpackStorageHint(PixelStorage::ALIGNMENT, 4);
			setPackStorageHint(PixelStorage::ROW_LENGTH, 0);
			setUnpackStorageHint(PixelStorage::ROW_LENGTH, 0);
		}

		void Texture::setData(const void* data, const int mipmap) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setData(data, mipmap);
		}

		void Texture::setUnpackStorageHint(const PixelStorage hint, const int value) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setUnpackStorageHint(hint, value);
		}

		void Texture::setPackStorageHint(const PixelStorage hint, const int value) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setPackStorageHint(hint, value);
		}

		void Texture::readPixels(void* data) const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->readPixels(data);
		}

		bool Texture::operator==(const Texture & other) const {
			return transform == other.transform && hue == other.hue && texture == other.texture;
		}

		bool Texture::operator!=(const Texture & other) const {
			return !operator==(other);
		}

		gfx::WindowModule* GraphicsContext::getWindow() {
			return window;
		}

		const gfx::WindowModule* GraphicsContext::getWindow() const {
			return window;
		}

		Texture& GraphicsContext::createTexture(const std::string & name, const Texture & texture) {
			if (hasTexture(name)) {
				MACE__THROW(AlreadyExists, "Texture with name " + name + " has already been created");
			}

			return textures[name] = texture;
		}

		Texture& GraphicsContext::getOrCreateTexture(const std::string & name, const TextureCreateCallback create) {
			if (!hasTexture(name)) {
				return createTexture(name, create());
			} else {
				return getTexture(name);
			}
		}

		Texture& GraphicsContext::getOrCreateTextureFromFile(const std::string & name, const std::string & path) {
			return getOrCreateTexture(name, [&path]() {
				return Texture::createFromFile(path);
			});
		}

		Model& GraphicsContext::createModel(const std::string & name, const Model & mod) {
			if (hasModel(name)) {
				MACE__THROW(AlreadyExists, "Model with name " + name + " has already been created");
			}

			return models[name] = mod;
		}

		Model& GraphicsContext::getOrCreateModel(const std::string & name, const ModelCreateCallback create) {
			if (!hasModel(name)) {
				return createModel(name, create());
			} else {
				return getModel(name);
			}
		}

		bool GraphicsContext::hasTexture(const std::string & name) const {
			//map.count() returns 1 if key exists, 0 otherwise.
			return textures.count(name) != 0;//the verbosity is to suppress warnings of casting from std::size_t to bool
		}

		bool GraphicsContext::hasModel(const std::string & name) const {
			return models.count(name) != 0;
		}

		void GraphicsContext::setTexture(const std::string & name, const Texture & texture) {
			textures[name] = texture;
		}

		Texture& GraphicsContext::getTexture(const std::string & name) {
			return textures.at(name);
		}

		const Texture& GraphicsContext::getTexture(const std::string & name) const {
			return textures.at(name);
		}

		void GraphicsContext::setModel(const std::string & name, const Model & model) {
			models[name] = model;
		}

		Model& GraphicsContext::getModel(const std::string & name) {
			return models.at(name);
		}

		const Model& GraphicsContext::getModel(const std::string & name) const {
			return models.at(name);
		}

		std::map<std::string, Texture>& GraphicsContext::getTextures() {
			return textures;
		}

		const std::map<std::string, Texture>& GraphicsContext::getTextures() const {
			return textures;
		}

		std::map<std::string, Model>& GraphicsContext::getModels() {
			return models;
		}

		const std::map<std::string, Model>& GraphicsContext::getModels() const {
			return models;
		}

		GraphicsContext::GraphicsContext(gfx::WindowModule * win) :window(win) {
#ifdef MACE_DEBUG_CHECK_NULLPTR
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

		TextureImpl::TextureImpl(const TextureDesc & t) : desc(t) {}

		TextureDesc::TextureDesc(const unsigned int w, const unsigned int h, const Format form) : format(form), width(w), height(h) {}
	}//gfx
}//mc
