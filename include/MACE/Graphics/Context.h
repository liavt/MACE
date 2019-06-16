/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_CONTEXT_H
#define MACE__GRAPHICS_CONTEXT_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Utility/Color.h>
#include <memory>
#include <map>
#include <string>
#include <functional>

#ifdef MACE_OPENCV
#	include <opencv2/opencv.hpp>
#endif//MACE_OPENCV

namespace mc {
	namespace gfx {
		class Renderer;

		/**
		Thrown when an error occured trying to read or write an image
		*/
		MACE__DECLARE_ERROR(BadImage);

		/**
		Thrown when a format is invalid
		*/
		MACE__DECLARE_ERROR(BadFormat);

		enum class ImageFormat: Byte {
			//each enum is equal to how many components in that type of image
			//the image load/save functions use swizzle masks to differentiate
			//between LUMINANCE and R, LUMINANCE_ALPHA and RG
			LUMINANCE = 1,
			LUMINANCE_ALPHA = 2,
			INTENSITY = 1,
			R = 1,
			RG = 2,
			RGB = 3,
			RGBA = 4,
			DONT_CARE = 0
		};

		enum class PrimitiveType: short int {
			//these draw modes were chosen because they exist in both OpenGL 3.3 and Direct3D
			POINTS = 0,
			LINES = 1,
			LINES_ADJACENCY = 2,
			LINES_STRIP = 3,
			LINES_STRIP_ADJACENCY = 4,
			TRIANGLES = 5,
			TRIANGLES_ADJACENCY = 6,
			TRIANGLES_STRIP = 7,
			TRIANGLES_STRIP_ADJACENCY = 8,
			//TRIANGLES_FAN does not exist in DirectX 10+ because of performance issues. It is not included in this list
		};

		enum class TextureSlot: Byte {
			FOREGROUND = 0,
			BACKGROUND = 1,
			MASK = 2
		};

		class MACE_NOVTABLE ModelImpl {
			friend class Model;
		public:
			virtual ~ModelImpl() = default;

			virtual void draw() const = 0;

			virtual void loadTextureCoordinates(const unsigned int dataSize, const float* data) = 0;
			virtual void loadVertices(const unsigned int verticeSize, const float* vertices) = 0;
			virtual void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) = 0;

			bool operator==(const ModelImpl& other) const;
			bool operator!=(const ModelImpl& other) const;
		protected:
			PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
		};

		class Model: public Initializable {
		public:
			static Model& getQuad();

			Model();
			Model(const Model& other);
			~Model() = default;

			/**
			@rendercontext
			*/
			void init() override;
			/**
			@rendercontext
			*/
			void destroy() override;

			/**
			@rendercontext
			*/
			void createTextureCoordinates(const unsigned int dataSize, const float* data);
			/**
			@rendercontext
			*/
			template<const unsigned int N>
			void createTextureCoordinates(const float(&data)[N]) {
				createTextureCoordinates(N, data);
			}

			/**
			@rendercontext
			*/
			void createVertices(const unsigned int verticeSize, const float* vertices, const PrimitiveType& prim);
			/**
			@rendercontext
			*/
			template<const unsigned int N>
			void createVertices(const float(&vertices)[N], const PrimitiveType& prim) {
				createVertices(N, vertices, prim);
			}

			/**
			@rendercontext
			*/
			void createIndices(const unsigned int indiceNum, const unsigned int* indiceData);
			/**
			@rendercontext
			*/
			template<const unsigned int N>
			void createIndices(const unsigned int(&indiceData)[N]) {
				createIndices(N, indiceData);
			}

			PrimitiveType getPrimitiveType();
			const PrimitiveType getPrimitiveType() const;

			/**
			@rendercontext
			*/
			void draw() const;

			bool isCreated() const;

			virtual bool operator==(const Model& other) const;
			virtual bool operator!=(const Model& other) const;
		private:
			std::shared_ptr<ModelImpl> model;

			Model(const std::shared_ptr<ModelImpl> mod);
		};

		struct TextureDesc {
			enum class Type: short int {
				UNSIGNED_BYTE,
				BYTE,
				UNSIGNED_SHORT,
				SHORT,
				UNSIGNED_INT,
				INT,
				FLOAT,
				UNSIGNED_BYTE_3_3_2,
				UNSIGNED_BYTE_2_3_3_REV,
				UNSIGNED_SHORT_5_6_5,
				UNSIGNED_SHORT_5_6_5_REV,
				UNSIGNED_SHORT_4_4_4_4,
				UNSIGNED_SHORT_4_4_4_4_REV,
				UNSIGNED_SHORT_5_5_5_1,
				UNSIGNED_SHORT_1_5_5_5_REV,
				UNSIGNED_INT_8_8_8_8,
				UNSIGNED_INT_8_8_8_8_REV,
				UNSIGNED_INT_10_10_10_2,
				UNSIGNED_INT_2_10_10_10_REV,
			};

			enum class Format: short int {
				RED,
				RG,
				RGB,
				RGBA,
				BGR,
				BGRA,
				RED_INTEGER,
				RG_INTEGER,
				RGB_INTEGER,
				BGR_INTEGER,
				RGBA_INTEGER,
				BGRA_INTEGER,
				STENCIL,
				DEPTH,
				DEPTH_STENCIL,
				LUMINANCE,
				LUMINANCE_ALPHA,
				INTENSITY
			};

			enum class InternalFormat: Enum {
				DEPTH24,
				DEPTH24_STENCIL8,
				R8,
				R16,
				RG8,
				RG16,
				RGB8,
				RGBA8,
				SRGB8,
				SRGB8_ALPHA8
			};

			enum class Filter: Byte {
				//MIPMAP_LINEAR and MIPMAP_NEAREST automatically
				//generate mipmaps in OpenGL renderer.
				//They cant be used for the magnication filter,
				//only the minification filter
				MIPMAP_LINEAR,
				MIPMAP_NEAREST,
				NEAREST,
				LINEAR,
				ANISOTROPIC
			};

			enum class Wrap: Byte {
				CLAMP,
				WRAP,
				MIRROR,
				BORDER
			};

			TextureDesc() = default;
			TextureDesc(const Pixels w, const Pixels h, const Format form = Format::RGBA);


			int mipmapLevels = 1;
			Type type = Type::FLOAT;
			Format format = Format::RGBA;
			InternalFormat internalFormat = InternalFormat::RGBA8;

			Pixels width = 0, height = 0;

			Wrap wrapS = Wrap::CLAMP;
			Wrap wrapT = Wrap::CLAMP;
			Filter minFilter = Filter::LINEAR;
			Filter magFilter = Filter::LINEAR;

			Color borderColor = Colors::INVISIBLE;
		};

		struct PixelStorageHints {
			int alignment = 4;
			int rowLength = 0;
		};

		class MACE_NOVTABLE TextureImpl {
			friend class Texture;
		public:
			TextureImpl(const TextureDesc& t);
			virtual ~TextureImpl() = default;

			virtual void bindTextureSlot(const TextureSlot slot) const = 0;

			virtual void setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const PixelStorageHints hints) = 0;

			virtual void readPixels(void* data, const PixelStorageHints hints) const = 0;
		protected:
			const TextureDesc desc;
		};

		class Texture {
			friend class Painter;
		public:
			/**
			@rendercontext
			*/
			static Texture create(const Color& col, const Pixels width = 1, const Pixels height = 1);
			/**
			@rendercontext
			*/
			static Texture createFromFile(const std::string& file, const ImageFormat format = ImageFormat::DONT_CARE, const TextureDesc::Wrap wrap = TextureDesc::Wrap::CLAMP);
			/**
			@rendercontext
			*/
			static Texture createFromFile(const CString file, const ImageFormat format = ImageFormat::DONT_CARE, const TextureDesc::Wrap wrap = TextureDesc::Wrap::CLAMP);
			/**
			@rendercontext
			*/
			static Texture createFromMemory(const unsigned char* c, const Size size);
			/**
			@copydoc createFromMemory(const unsigned char*, const int)
			*/
			template<const Size N>
			static Texture createFromMemory(const unsigned char c[N]) {
				return createFromMemory(c, static_cast<int>(N));
			}
			/**
			@rendercontext
			*/
			static Texture& getSolidColor();
			/**
			- Vertical gradient
			- Darker part on bottom
			- Height is 100

			@rendercontext
			*/
			static Texture& getGradient();

			Texture();
			Texture(const TextureDesc& d);
			Texture(const Color& col);
			Texture(const Texture& tex);
			Texture(const Texture& tex, const Color& col);

			/**
			@rendercontext
			*/
			void init(const TextureDesc& desc);
			/**
			@rendercontext
			*/
			void destroy();

			bool isCreated() const;

			const TextureDesc& getDesc() const;

			Pixels getWidth();
			const Pixels getWidth() const;

			Pixels getHeight();
			const Pixels getHeight() const;

			Vector<Pixels, 2> getDimensions();
			const Vector<Pixels, 2> getDimensions() const;

			//this needs to be defined in the header file to prevent linker conflicts, because Entity.cpp does not have opencv included ever.
#			ifdef MACE_OPENCV
			/**
			@rendercontext
			*/
			Texture(cv::Mat mat) : Texture() {
				TextureDesc desc = TextureDesc(mat.rows, mat.cols);

				if (mat.channels() == 1) {
					desc.format = TextureDesc::Format::RED;
					desc.internalFormat = TextureDesc::InternalFormat::RED;
				} else if (mat.channels() == 2) {
					desc.format = TextureDesc::Format::RG;
					desc.internalFormat = TextureDesc::InternalFormat::RG;
				} else if (mat.channels() == 3) {
					desc.format = TextureDesc::Format::BGR;
					desc.internalFormat = TextureDesc::InternalFormat::RGB;
				} else if (mat.channels() == 4) {
					desc.format = TextureDesc::Format::BGRA;
					desc.internalFormat = TextureDesc::InternalFormat::RGBA;
				}

				desc.type = TextureDesc::Type::UNSIGNED_BYTE;
				if (mat.depth() == CV_8S) {
					desc.type = TextureDesc::Type::BYTE;
				} else if (mat.depth() == CV_16U) {
					desc.type = TextureDesc::Type::UNSIGNED_SHORT;
				} else if (mat.depth() == CV_16S) {
					desc.type = TextureDesc::Type::SHORT;
				} else if (mat.depth() == CV_32S) {
					desc.type = TextureDesc::Type::INT;
				} else if (mat.depth() == CV_32F) {
					desc.type = TextureDesc::Type::FLOAT;
				} else if (mat.depth() == CV_64F) {
					MACE__THROW(BadFormat, "Unsupported cv::Mat depth: CV_64F");
				}

				init(desc);

				PixelStorageHints hints{};
				hints.alignment = (mat.step & 3) ? 1 : 4;
				hints.rowLength = static_cast<int>(mat.step / mat.elemSize());

				setData(mat.ptr(), 0, hints);
			}

			/**
			@rendercontext
			*/
			cv::Mat toMat() {
				cv::Mat img(getHeight(), getWidth(), CV_8UC3);

				PixelStorageHints hints{};
				hints.alignment = (img.step & 3) ? 1 : 4;
				hints.rowLength = static_cast<int>(img.step / img.elemSize());

				readPixels(img.data, hints);

				cv::flip(img, img, 0);

				return img;
			}
#			endif//MACE_OPENCV

			Color & getHue();
			const Color& getHue() const;
			void setHue(const Color & col);

			Vector<RelativeUnit, 4>& getTransform();
			const Vector<RelativeUnit, 4>& getTransform() const;
			void setTransform(const Vector<RelativeUnit, 4> & trans);

			void setData(const void* data, const int x, const int y, const Pixels width, const Pixels height, const int mipmapLevel = 0, const PixelStorageHints hints = PixelStorageHints());
			/**
			@rendercontext
			*/
			void setData(const void* data, const int mipmap = 0, const PixelStorageHints hints = PixelStorageHints());
			/**
			@rendercontext
			*/
			template<typename T, Pixels W, Pixels H>
			void setData(const T(&data)[W][H], const int mipmap = 0, const PixelStorageHints hints = PixelStorageHints()) {
				MACE_STATIC_ASSERT(W != 0, "Width of Texture can not be 0!");
				MACE_STATIC_ASSERT(H != 0, "Height of Texture can not be 0!");

				MACE_IF_CONSTEXPR(W != texture->desc.width || H != texture->desc.height) {
					MACE__THROW(AssertionFailed, "Input data is not equal to Texture width and height");
				}

				setData(static_cast<const void*>(data[0]), mipmap, hints);
			}
			/**
			@rendercontext
			*/
			void readPixels(void* data, const PixelStorageHints hints = PixelStorageHints()) const;

			bool operator==(const Texture & other) const;
			bool operator!=(const Texture & other) const;
		private:
			std::shared_ptr<TextureImpl> texture;

			Color hue = Colors::INVISIBLE;

			Vector<RelativeUnit, 4> transform{0.0f, 0.0f, 1.0f, 1.0f};

			Texture(const std::shared_ptr<TextureImpl> tex, const Color & col = Color(0.0f, 0.0f, 0.0f, 0.0f));

			void bindTextureSlot(const TextureSlot slot) const;
		};//Texture

		struct RenderTargetDesc {
		public:
			Pixels width = 128, height = 128;
		};

		class MACE_NOVTABLE RenderTargetImpl: public Initializable {

		};//RenderTargetImpl

		//forward declare

		struct FontDesc;
		class FontImpl;

		class GraphicsContext: public Initializable {
			friend class Texture;
			friend class Model;
			friend class Font;
		public:
			using TextureCreateCallback = std::function<Texture()>;
			using ModelCreateCallback = std::function<Model()>;

			GraphicsContext(gfx::WindowModule* win);
			//prevent copying
			GraphicsContext(const GraphicsContext& other) = delete;
			virtual ~GraphicsContext() = default;

			void init() override;
			void render();
			void destroy() override;

			virtual Renderer* getRenderer() const = 0;

			gfx::WindowModule* getWindow();
			const gfx::WindowModule* getWindow() const;

			Texture& createTexture(const std::string& name, const Texture& texture = Texture());
			Texture& getOrCreateTexture(const std::string& name, const TextureCreateCallback create);
			Texture& getOrCreateTextureFromFile(const std::string& name, const std::string& path);
			Model& createModel(const std::string& name, const Model& texture = Model());
			Model& getOrCreateModel(const std::string& name, const ModelCreateCallback create);

			bool hasTexture(const std::string& name) const;
			bool hasModel(const std::string& name) const;

			void setTexture(const std::string& name, const Texture& texture);
			Texture& getTexture(const std::string& name);
			const Texture& getTexture(const std::string& name) const;

			void setModel(const std::string& name, const Model& model);
			Model& getModel(const std::string& name);
			const Model& getModel(const std::string& name) const;

			std::map<std::string, Texture>& getTextures();
			const std::map<std::string, Texture>& getTextures() const;

			std::map<std::string, Model>& getModels();
			const std::map<std::string, Model>& getModels() const;
		protected:
			gfx::WindowModule* window;

			/*
			Reasoning behind returning a std::shared_ptr and not a std::unique_ptr...

			Each Model and Texture class needs to have it's own Impl.
			Each Impl acts as a pointer to a resource, whether it be in GPU memory
			or whatever. However, Model and Texture needs the ability to be moved,
			like so:

			Model model = Model();
			Model newModel = m;

			Both Model objects will share the same resource, as it is simply a
			pointer to the actual data. However, with std::unique_ptr, this move
			semantic because near impossible. Because multiple Model objects
			may have to own the same pointer to data, they have to use std::shared_ptr
			*/
			MACE_NODISCARD virtual std::shared_ptr<ModelImpl> createModelImpl() = 0;
			MACE_NODISCARD virtual std::shared_ptr<TextureImpl> createTextureImpl(const TextureDesc& desc) = 0;
			MACE_NODISCARD virtual std::shared_ptr<FontImpl> createFontImpl(const FontDesc& desc) = 0;

			virtual void onInit(gfx::WindowModule* win) = 0;
			virtual void onRender(gfx::WindowModule* win) = 0;
			virtual void onDestroy(gfx::WindowModule* win) = 0;

		private:
			std::map<std::string, Texture> textures{};
			std::map<std::string, Model> models{};
		};
	}
}//mc

#endif//MACE__GRAPHICS_CONTEXT_H
