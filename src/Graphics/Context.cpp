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
#ifdef MACE_DEBUG
#	define MACE__VERIFY_TEXTURE_INIT() do{if(texture == nullptr){ MACE__THROW(InvalidState, "This Texture has not had init() called yet"); }}while(0)
#	define MACE__VERIFY_MODEL_INIT() do{if(model == nullptr){ MACE__THROW(InvalidState, "This Model has not had init() called yet"); }}while(0)
#else
#	define MACE__VERIFY_TEXTURE_INIT()
#	define MACE__VERIFY_MODEL_INIT()
#endif

		//these are the names for cached resources in the GraphicsContext
#define MACE__RESOURCE_SOLIDCOLOR "MC/SolidColor"
#define MACE__RESOURCE_GRADIENT "MC/Gradient"
		//how many pixels in the gradient
#define MACE__RESOURCE_GRADIENT_HEIGHT 100
#define MACE__RESOURCE_QUAD "MC/Quad"

		bool ModelImpl::operator==(const ModelImpl & other) const {
			return primitiveType == other.primitiveType;
		}

		bool ModelImpl::operator!=(const ModelImpl & other) const {
			return !operator==(other);
		}

		Model & Model::getQuad() {
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

					model.createVertices(squareVertices, Enums::PrimitiveType::TRIANGLES);
					model.createIndices(squareIndices);
					model.createTextureCoordinates(squareTextureCoordinates);

					return model;
				});
			}
		}

		Model::Model() : model(nullptr) {}

		Model::Model(const std::shared_ptr<ModelImpl> mod) : model(mod) {}

		Model::Model(const Model & other) : model(other.model) {}

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

		void Model::createTextureCoordinates(const Size dataSize, const float * data) {
			MACE__VERIFY_MODEL_INIT();

			model->loadTextureCoordinates(dataSize, data);
		}

		void Model::createVertices(const Size verticeSize, const float * vertices, const Enums::PrimitiveType& prim) {
			MACE__VERIFY_MODEL_INIT();

			model->primitiveType = prim;
			model->loadVertices(verticeSize, vertices);

		}

		void Model::createIndices(const Size indiceNum, const unsigned int * indiceData) {
			MACE__VERIFY_MODEL_INIT();

			model->loadIndices(indiceNum, indiceData);
		}

		Enums::PrimitiveType Model::getPrimitiveType() {
			return model->primitiveType;
		}

		const Enums::PrimitiveType Model::getPrimitiveType() const {
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

		bool Model::operator==(const Model & other) const {
			return model == other.model;
		}

		bool Model::operator!=(const Model & other) const {
			return !operator==(other);
		}

		Texture Texture::create(const Color & col, const Size width, const Size height) {
			Texture tex = Texture();

			tex.init();
			tex.load(col, width, height);

			return tex;
		}

		Texture Texture::createFromFile(const std::string & file, const Enums::ImageFormat format) {
			return Texture::createFromFile(file.c_str(), format);
		}

		Texture Texture::createFromFile(const char * file, const Enums::ImageFormat format) {
			Texture tex = Texture();

			tex.init();
			tex.load(file, format);

			return tex;
		}

		Texture Texture::createFromMemory(const unsigned char * c, const Size size) {
			Texture texture = Texture();
			texture.resetPixelStorage();

			int width, height, componentSize;

			Byte* image = stbi_load_from_memory(c, size, &width, &height, &componentSize, STBI_rgb_alpha);

			try {
				if (image == nullptr || width == 0 || height == 0 || componentSize == 0) {
					MACE__THROW(BadImage, "Unable to read image from memory: " + std::string(stbi_failure_reason()));
				}

				texture.setData(image, width, height, gfx::Enums::Type::UNSIGNED_BYTE, gfx::Enums::Format::RGBA, gfx::Enums::InternalFormat::RGBA, 0);
			} catch (const std::exception& e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			texture.setMinFilter(Enums::ResizeFilter::MIPMAP_LINEAR);
			texture.setMagFilter(Enums::ResizeFilter::NEAREST);

			return texture;
		}

		Texture& Texture::getSolidColor() {
			GraphicsContext* context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {
				return context->getOrCreateTexture(MACE__RESOURCE_SOLIDCOLOR, []() {
					Texture texture = Texture();

					texture.init();

					texture.resetPixelStorage();

					MACE_CONSTEXPR const float data[] = { 1 };
					texture.setData(data, 1, 1, gfx::Enums::Type::FLOAT, gfx::Enums::Format::RED, gfx::Enums::InternalFormat::RED, 0);

					texture.setSwizzle(gfx::Enums::SwizzleMode::G, gfx::Enums::SwizzleMode::R);
					texture.setSwizzle(gfx::Enums::SwizzleMode::B, gfx::Enums::SwizzleMode::R);
					texture.setSwizzle(gfx::Enums::SwizzleMode::A, gfx::Enums::SwizzleMode::R);

					texture.setMinFilter(Enums::ResizeFilter::NEAREST);
					texture.setMagFilter(Enums::ResizeFilter::NEAREST);

					return texture;
				});
			}
		}

		Texture & Texture::getGradient() {
			GraphicsContext* context = gfx::getCurrentWindow()->getContext();
			if (context == nullptr) {
				MACE__THROW(NullPointer, "No graphics context found in window!");
			} else {
				return context->getOrCreateTexture(MACE__RESOURCE_GRADIENT, []() {
					Texture texture = Texture();

					texture.init();

					texture.resetPixelStorage();

					float data[MACE__RESOURCE_GRADIENT_HEIGHT];
					for (unsigned int i = 0; i < MACE__RESOURCE_GRADIENT_HEIGHT; ++i) {
						//the darker part is on the bottom
						data[i] = static_cast<float>(MACE__RESOURCE_GRADIENT_HEIGHT - i) / static_cast<float>(MACE__RESOURCE_GRADIENT_HEIGHT);
					}
					texture.setData(data, 1, MACE__RESOURCE_GRADIENT_HEIGHT, gfx::Enums::Type::FLOAT, gfx::Enums::Format::RED, gfx::Enums::InternalFormat::RED, 0);

					texture.setSwizzle(gfx::Enums::SwizzleMode::G, gfx::Enums::SwizzleMode::R);
					texture.setSwizzle(gfx::Enums::SwizzleMode::B, gfx::Enums::SwizzleMode::R);

					texture.setMinFilter(Enums::ResizeFilter::LINEAR);
					texture.setMagFilter(Enums::ResizeFilter::NEAREST);

					return texture;
				});
			}
		}

		Texture::Texture() : texture(nullptr), hue(0.0f, 0.0f, 0.0f, 0.0f) {}

		Texture::Texture(const std::shared_ptr<TextureImpl> tex, const Color& col) : texture(tex), hue(col) {}

		Texture::Texture(const Texture & tex, const Color & col) : texture(tex.texture), hue(col) {}

		Texture::Texture(const Color& col) : Texture(Texture::getSolidColor(), col) {}

		void Texture::init() {
			if (texture == nullptr) {
				texture = gfx::getCurrentWindow()->getContext()->createTextureImpl();
			}

			texture->init();
		}

		void Texture::destroy() {
			MACE__VERIFY_TEXTURE_INIT();

			texture->destroy();
			texture.reset();
		}

		bool Texture::isCreated() const {
			return texture != nullptr;
		}

		bool Texture::isEmpty() const {
			return texture == nullptr ? true : texture->width == 0 && texture->height == 0;
		}

		unsigned int Texture::getWidth() {
			return texture == nullptr ? 0 : texture->width;
		}

		const unsigned int Texture::getWidth() const {
			return texture == nullptr ? 0 : texture->width;
		}

		unsigned int Texture::getHeight() {
			return texture == nullptr ? 0 : texture->height;
		}

		const unsigned int Texture::getHeight() const {
			return texture == nullptr ? 0 : texture->height;
		}

		Enums::Type Texture::getType() {
			return texture->type;
		}

		const Enums::Type Texture::getType() const {
			return texture->type;
		}

		Enums::Format Texture::getFormat() {
			return texture->format;
		}

		const Enums::Format Texture::getFormat() const {
			return texture->format;
		}

		Enums::InternalFormat Texture::getInternalFormat() {
			return texture->internalFormat;
		}

		const Enums::InternalFormat Texture::getInternalFormat() const {
			return texture->internalFormat;
		}

		void Texture::load(const char * file, const Enums::ImageFormat imgFormat) {
			MACE__VERIFY_TEXTURE_INIT();

			resetPixelStorage();

			int width, height, actualComponents;
			Byte* image = stbi_load(file, &width, &height, &actualComponents, static_cast<int>(imgFormat));

			try {
				if (image == nullptr || width == 0 || height == 0 || actualComponents == 0) {
					MACE__THROW(BadImage, "Unable to read image: " + std::string(file) + '\n' + stbi_failure_reason());
				}

				/*if DONT_CARE, the outputComponents is equal to the amount of components in image,
				otherwise equal to amount of requestedComponents
				*/
				const int outputComponents = (imgFormat == Enums::ImageFormat::DONT_CARE
											  ? actualComponents : static_cast<int>(imgFormat));

				Enums::Format format;
				Enums::InternalFormat internalFormat;

				if (outputComponents == 1) {
					format = Enums::Format::RED;
					internalFormat = Enums::InternalFormat::RED;
					if (imgFormat == Enums::ImageFormat::GRAY) {
						setSwizzle(Enums::SwizzleMode::G, Enums::SwizzleMode::R);
						setSwizzle(Enums::SwizzleMode::B, Enums::SwizzleMode::R);
					}
				} else if (outputComponents == 2) {
					format = Enums::Format::RG;
					internalFormat = Enums::InternalFormat::RG;
					if (imgFormat == Enums::ImageFormat::GRAY_ALPHA) {
						setSwizzle(Enums::SwizzleMode::G, Enums::SwizzleMode::R);
						setSwizzle(Enums::SwizzleMode::B, Enums::SwizzleMode::R);
						setSwizzle(Enums::SwizzleMode::A, Enums::SwizzleMode::G);
					}
				} else if (outputComponents == 3) {
					format = Enums::Format::RGB;
					internalFormat = Enums::InternalFormat::RGB;
				} else if (outputComponents == 4) {
					format = Enums::Format::RGBA;
					internalFormat = Enums::InternalFormat::RGBA;
				} else {
					MACE__THROW(BadImage, "Internal Error: outputComponents is not 1-4!");
				}

				setData(image, width, height, gfx::Enums::Type::UNSIGNED_BYTE, format, internalFormat);
			} catch (const std::exception& e) {
				stbi_image_free(image);
				throw e;
			}

			stbi_image_free(image);

			setMinFilter(Enums::ResizeFilter::MIPMAP_LINEAR);
			setMagFilter(Enums::ResizeFilter::NEAREST);
		}

		void Texture::load(const std::string & file, const Enums::ImageFormat format) {
			load(file.c_str(), format);
		}

		void Texture::load(const Color & c, const Size width, const Size height) {
			MACE__VERIFY_TEXTURE_INIT();

			resetPixelStorage();

			setData(&c, width, height, gfx::Enums::Type::FLOAT, gfx::Enums::Format::RGBA, gfx::Enums::InternalFormat::RGBA, 0);

			setMinFilter(Enums::ResizeFilter::NEAREST);
			setMagFilter(Enums::ResizeFilter::NEAREST);
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

		Vector<float, 4>& Texture::getTransform() {
			return transform;
		}

		const Vector<float, 4>& Texture::getTransform() const {
			return transform;
		}

		void Texture::setTransform(const Vector<float, 4>& trans) {
			transform = trans;
		}

		void Texture::bind() const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->bind();
		}

		void Texture::bind(const unsigned int location) const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->bind(location);
		}

		void Texture::unbind() const {
			MACE__VERIFY_TEXTURE_INIT();

			texture->unbind();
		}

		void Texture::resetPixelStorage() {
			MACE__VERIFY_TEXTURE_INIT();

			setPackStorageHint(Enums::PixelStorage::ALIGNMENT, 4);
			setUnpackStorageHint(Enums::PixelStorage::ALIGNMENT, 4);
			setPackStorageHint(Enums::PixelStorage::ROW_LENGTH, 0);
			setUnpackStorageHint(Enums::PixelStorage::ROW_LENGTH, 0);
		}

		void Texture::setMinFilter(const Enums::ResizeFilter filter) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setMinFilter(filter);
		}

		void Texture::setMagFilter(const Enums::ResizeFilter filter) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setMagFilter(filter);
		}

		void Texture::setData(const void * data, const Size width, const Size height, const Enums::Type type, const Enums::Format format, const Enums::InternalFormat internalFormat, const Index mipmap) {
			texture->type = type;
			texture->format = format;
			texture->internalFormat = internalFormat;
			if (width == 0 || height == 0) {
				return;
			}
			texture->width = width;
			texture->height = height;
			texture->setData(data, mipmap);
		}

		void Texture::setData(const void * data, const Index mipmap) {
			if (isEmpty()) {
				return;
			}

			texture->setData(data, mipmap);
		}

		void Texture::setUnpackStorageHint(const Enums::PixelStorage hint, const int value) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setUnpackStorageHint(hint, value);
		}

		void Texture::setPackStorageHint(const Enums::PixelStorage hint, const int value) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setPackStorageHint(hint, value);
		}

		void Texture::setWrap(const Enums::WrapMode wrap) {
			setWrapS(wrap);
			setWrapT(wrap);
		}

		void Texture::setWrapS(const Enums::WrapMode wrap) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setWrapS(wrap);
		}

		void Texture::setWrapT(const Enums::WrapMode wrap) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setWrapT(wrap);
		}

		void Texture::setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg) {
			MACE__VERIFY_TEXTURE_INIT();

			texture->setSwizzle(mode, arg);
		}

		void Texture::readPixels(void * data) const {
			MACE__VERIFY_TEXTURE_INIT();

			if (isEmpty()) {
				MACE__THROW(InvalidState, "Can\'t read pixels from a Texture that doesn\'t have data yet");
			}

			texture->readPixels(data);
		}

		bool Texture::operator==(const Texture& other) const {
			return transform == other.transform && hue == other.hue && texture == other.texture;
		}

		bool Texture::operator!=(const Texture& other) const {
			return !operator==(other);
		}

		gfx::WindowModule * GraphicsContext::getWindow() {
			return window;
		}

		const gfx::WindowModule * GraphicsContext::getWindow() const {
			return window;
		}

		void GraphicsContext::createTexture(const std::string & name, const Texture & texture) {
			if (hasTexture(name)) {
				MACE__THROW(AlreadyExists, "Texture with name " + name + " has already been created");
			}

			textures[name] = texture;
		}

		Texture& GraphicsContext::getOrCreateTexture(const std::string & name, const TextureCreateCallback create) {
			if (!hasTexture(name)) {
				createTexture(name, create());

				return getTexture(name);
			} else {
				return getTexture(name);
			}
		}

		void GraphicsContext::createModel(const std::string & name, const Model& mod) {
			if (hasModel(name)) {
				MACE__THROW(AlreadyExists, "Model with name " + name + " has already been created");
			}

			models[name] = mod;
		}

		Model & GraphicsContext::getOrCreateModel(const std::string & name, const ModelCreateCallback create) {
			if (!hasModel(name)) {
				createModel(name, create());

				return getModel(name);
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

		Texture & GraphicsContext::getTexture(const std::string & name) {
			return textures.at(name);
		}

		const Texture & GraphicsContext::getTexture(const std::string & name) const {
			return textures.at(name);
		}

		void GraphicsContext::setModel(const std::string & name, const Model & model) {
			models[name] = model;
		}

		Model & GraphicsContext::getModel(const std::string & name) {
			return models.at(name);
		}

		const Model & GraphicsContext::getModel(const std::string & name) const {
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