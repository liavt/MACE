/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
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

		namespace Enums {
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
				//The next 2 are deprecated in OpenGL 3.3
				//This comment exists to remind people not to
				//add them.
				//LUMINANCE,
				//LUMINANCE_ALPHA
			};

			enum class InternalFormat: Enum {
				DEPTH,
				DEPTH_STENCIL,
				RED,
				RG,
				RGB,
				RGBA
			};

			enum class ResizeFilter: Byte {
				//MIPMAP_LINEAR and MIPMAP_NEAREST automatically
				//generate mipmaps in OpenGL renderer.
				//They cant be used for the magnication filter,
				//only the minification filter
				MIPMAP_LINEAR,
				MIPMAP_NEAREST,
				NEAREST,
				LINEAR,
			};

			enum class PixelStorage: Byte {
				ALIGNMENT,
				ROW_LENGTH
			};

			enum class WrapMode: Byte {
				//OpenGL supports CLAMP_TO_BORDER and BORDER, however those 2
				//require additional work which is outside the scope of MACE.
				//Also, DirectX doesn't support them, and their use case is
				//very sparsely documented.
				CLAMP,
				REPEAT,
				MIRRORED_REPEAT,
				MIRROR_CLAMP,
			};

			//there is no ZERO and ONE like in OpenGL because DirectX doesn't support that
			enum class SwizzleMode: Byte {
				/*
				The reasoning for these specific numerical values is because then we can just cast it and dd it to GL_RED, because GL spec says:
				GL_RED = ...
				GL_GREEN = GL_RED + 1
				GL_BLUE = GL_RED + 2
				GL_ALPHA = GL_RED + 3

				Meaning:
				GL_RED + static_cast<Enum>(arg) = specified component constant, where arg is Enums::SwizzleMode

				So GL_RED + static_cast<Enum>(Enums::SwizzleMode::B) would become GL_BLUE because GL_BLUE = GL_RED + 2

				Thank god for enum classes!
				*/
				R = 0,
				G = 1,
				B = 2,
				A = 3
			};

			enum class ImageFormat: Byte {
				//each enum is equal to how many components in that type of image
				//the image load/save functions use swizzle masks to differentiate
				//between GRAY and R, GRAY_ALPHA and RG
				GRAY = 1,
				GRAY_ALPHA = 2,
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
		}

		class ModelImpl: public Initializable, public Bindable {
			friend class Model;
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void bind() const override = 0;
			virtual void unbind() const override = 0;

			virtual void draw() const = 0;

			virtual void loadTextureCoordinates(const Size dataSize, const float* data) = 0;
			virtual void loadVertices(const Size verticeSize, const float* vertices) = 0;
			virtual void loadIndices(const Size indiceNum, const unsigned int* indiceData) = 0;

			virtual bool isCreated() const = 0;

			bool operator==(const ModelImpl& other) const;
			bool operator!=(const ModelImpl& other) const;
		protected:
			Enums::PrimitiveType primitiveType = Enums::PrimitiveType::TRIANGLES;
		};

		class Model: public Initializable, public Bindable {
		public:
			static Model& getQuad();

			Model();
			Model(const std::shared_ptr<ModelImpl> mod);
			Model(const Model& other);
			~Model() = default;

			void init() override;
			void destroy() override;

			void bind() const override;
			void unbind() const override;

			void createTextureCoordinates(const Size dataSize, const float* data);
			template<const Size N>
			void createTextureCoordinates(const float(&data)[N]) {
				createTextureCoordinates(N, data);
			}

			void createVertices(const Size verticeSize, const float* vertices, const Enums::PrimitiveType& prim);
			template<const Size N>
			void createVertices(const float(&vertices)[N], const Enums::PrimitiveType& prim) {
				createVertices(N, vertices, prim);
			}

			void createIndices(const Size indiceNum, const unsigned int* indiceData);
			template<const Size N>
			void createIndices(const unsigned int(&indiceData)[N]) {
				createIndices(N, indiceData);
			}

			Enums::PrimitiveType getPrimitiveType();
			const Enums::PrimitiveType getPrimitiveType() const;

			void draw() const;

			bool isCreated() const;

			bool operator==(const Model& other) const;
			bool operator!=(const Model& other) const;
		private:
			std::shared_ptr<ModelImpl> model;
		};

		class TextureImpl: public Initializable, public Bindable {
			friend class Texture;
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void bind() const override = 0;
			virtual void bind(const unsigned int location) const = 0;
			virtual void unbind() const override = 0;

			virtual bool isCreated() const = 0;

			virtual void setData(const void* data, const Index mipmap) = 0;

			virtual void setMinFilter(const Enums::ResizeFilter filter) = 0;
			virtual void setMagFilter(const Enums::ResizeFilter filter) = 0;

			virtual void setUnpackStorageHint(const Enums::PixelStorage hint, const int value) = 0;
			virtual void setPackStorageHint(const Enums::PixelStorage hint, const int value) = 0;

			virtual void setWrapS(const Enums::WrapMode wrap) = 0;
			virtual void setWrapT(const Enums::WrapMode wrap) = 0;

			virtual void readPixels(void* data) const = 0;

			virtual void setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg) = 0;
		protected:
			unsigned int width = 0, height = 0;

			Enums::Type type = Enums::Type::FLOAT;
			Enums::Format format = Enums::Format::RGBA;
			Enums::InternalFormat internalFormat = Enums::InternalFormat::RGBA;
		};

		class Texture: public Initializable, public Bindable {
		public:
			static Texture create(const Color& col, const Size width = 1, const Size height = 1);
			static Texture createFromFile(const std::string& file, const Enums::ImageFormat format = Enums::ImageFormat::DONT_CARE);
			static Texture createFromFile(const char* file, const Enums::ImageFormat format = Enums::ImageFormat::DONT_CARE);
			static Texture createFromMemory(const unsigned char * c, const Size size);

			static Texture& getSolidColor();
			/**
			- Vertical gradient
			- Darker part on bottom
			- Height is 100
			*/
			static Texture& getGradient();

			Texture();
			Texture(const Color& col);
			Texture(const std::shared_ptr<TextureImpl> tex, const Color& col = Color(0.0f, 0.0f, 0.0f, 0.0f));
			Texture(const Texture& tex, const Color& col = Color(0.0, 0.0f, 0.0f, 0.0f));

			void init() override;
			void destroy() override;

			bool isCreated() const;
			bool isEmpty() const;

			unsigned int getWidth();
			const unsigned int getWidth() const;

			unsigned int getHeight();
			const unsigned int getHeight() const;

			Enums::Type getType();
			const Enums::Type getType() const;

			Enums::Format getFormat();
			const Enums::Format getFormat() const;

			Enums::InternalFormat getInternalFormat();
			const Enums::InternalFormat getInternalFormat() const;

			//this needs to be defined in the header file to prevent linker conflicts, because Entity.cpp does not have opencv included ever.
#			ifdef MACE_OPENCV
			void load(cv::Mat mat) {
				//cv::flip(mat, mat, 0);

				Enums::Format colorFormat = Enums::Format::BGR;
				if (mat.channels() == 1) {
					colorFormat = Enums::Format::LUMINANCE;
				} else if (mat.channels() == 2) {
					colorFormat = Enums::Format::LUMINANCE_ALPHA;
				} else if (mat.channels() == 4) {
					colorFormat = Enums::Format::BGRA;
				}

				Enums::Type type = Enums::Type::UNSIGNED_BYTE;
				if (mat.depth() == CV_8S) {
					type = Enums::Type::BYTE;
				} else if (mat.depth() == CV_16U) {
					type = Enums::Type::UNSIGNED_SHORT;
				} else if (mat.depth() == CV_16S) {
					type = Enums::Type::SHORT;
				} else if (mat.depth() == CV_32S) {
					type = Enums::Type::INT;
				} else if (mat.depth() == CV_32F) {
					;
					type = Enums::Type::FLOAT;
				} else if (mat.depth() == CV_64F) {
					MACE__THROW(BadFormat, "Unsupported cv::Mat depth: CV_64F");
				}

				resetPixelStorage();
				setPackStorageHint(Enums::PixelStorage::ALIGNMENT, (mat.step & 3) ? 1 : 4);
				setPackStorageHint(Enums::PixelStorage::ROW_LENGTH, static_cast<int>(mat.step / mat.elemSize()));

				setData(mat.ptr(), mat.cols, mat.rows, type, colorFormat, Enums::InternalFormat::RGBA);
			}

			Texture(cv::Mat mat) : Texture() {
				init();
				load(mat);
			}

			cv::Mat toMat(const Size width, const Size height) {
				cv::Mat img(height, width, CV_8UC3);

				resetPixelStorage();
				setPackStorageHint(Enums::PixelStorage::ALIGNMENT, (img.step & 3) ? 1 : 4);
				setPackStorageHint(Enums::PixelStorage::ROW_LENGTH, static_cast<int>(img.step / img.elemSize()));

				bind();

				getImage(Enums::Format::BGR, Enums::Type::UNSIGNED_BYTE, img.data);

				cv::flip(img, img, 0);

				return img;
			}
#			endif//MACE_OPENCV

			void load(const char* file, const Enums::ImageFormat format = Enums::ImageFormat::DONT_CARE);
			void load(const std::string& file, const Enums::ImageFormat format = Enums::ImageFormat::DONT_CARE);
			void load(const Color& c, const Size width = 1, const Size height = 1);

			Color& getHue();
			const Color& getHue() const;
			void setHue(const Color& col);

			Vector<float, 4>& getTransform();
			const Vector<float, 4>& getTransform() const;
			void setTransform(const Vector<float, 4>& trans);

			void bind() const;
			void bind(const unsigned int location) const;
			void unbind() const override;

			void resetPixelStorage();

			void setMinFilter(const Enums::ResizeFilter filter);
			void setMagFilter(const Enums::ResizeFilter filter);

			void setData(const void* data, const Size width, const Size height,
						 const Enums::Type type = Enums::Type::FLOAT, const Enums::Format format = Enums::Format::RGBA,
						 const Enums::InternalFormat internalFormat = Enums::InternalFormat::RGBA, const Index mipmap = 0);
			template<typename T, Size W, Size H>
			void setData(const T(&data)[W][H],
						 const Enums::Type type = Enums::Type::FLOAT, const Enums::Format format = Enums::Format::RGBA,
						 const Enums::InternalFormat internalFormat = Enums::InternalFormat::RGBA, const Index mipmap = 0) {
				MACE_STATIC_ASSERT(W != 0, "Width of Texture can not be 0!");
				MACE_STATIC_ASSERT(H != 0, "Height of Texture can not be 0!");

				setData(data, W, H, type, format, internalFormat, mipmap);
			}
			template<Size W, Size H>
			void setData(const float(&data)[W][H], const Enums::Format format = Enums::Format::RGBA,
						 const Enums::InternalFormat internalFormat = Enums::InternalFormat::RGBA, const Index mipmap = 0) {
				MACE_STATIC_ASSERT(W != 0, "Width of Texture can not be 0!");
				MACE_STATIC_ASSERT(H != 0, "Height of Texture can not be 0!");

				setData(data, W, H, Enums::Type::FLOAT, format, internalFormat, mipmap);
			}
			template<Size W, Size H>
			void setData(const int(&data)[W][H], const Enums::Format format = Enums::Format::RGBA,
						 const Enums::InternalFormat internalFormat = Enums::InternalFormat::RGBA, const Index mipmap = 0) {
				MACE_STATIC_ASSERT(W != 0, "Width of Texture can not be 0!");
				MACE_STATIC_ASSERT(H != 0, "Height of Texture can not be 0!");

				setData(data, W, H, Enums::Type::INT, format, internalFormat, mipmap);
			}
			template<Size W, Size H>
			void setData(const unsigned int(&data)[W][H], const Enums::Format format = Enums::Format::RGBA,
						 const Enums::InternalFormat internalFormat = Enums::InternalFormat::RGBA, const Index mipmap = 0) {
				MACE_STATIC_ASSERT(W != 0, "Width of Texture can not be 0!");
				MACE_STATIC_ASSERT(H != 0, "Height of Texture can not be 0!");

				setData(data, W, H, Enums::Type::UNSIGNED_INT, format, internalFormat, mipmap);
			}


			void setData(const void* data, const Index mipmap = 0);
			template<typename T, Size W, Size H>
			void setData(const T(&data)[W][H], const Index mipmap = 0) {
				MACE_STATIC_ASSERT(W != 0, "Width of Texture can not be 0!");
				MACE_STATIC_ASSERT(H != 0, "Height of Texture can not be 0!");

				if (W != texture->width || H != texture->height) {
					MACE__THROW(AssertionFailed, "Input data is not equal to Texture width and height");
				}

				setData(data, mipmap);
			}

			void setUnpackStorageHint(const Enums::PixelStorage hint, const int value);
			void setPackStorageHint(const Enums::PixelStorage hint, const int value);

			void setWrap(const Enums::WrapMode wrap);
			void setWrapS(const Enums::WrapMode wrap);
			void setWrapT(const Enums::WrapMode wrap);

			void setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg);

			void readPixels(void* data) const;

			bool operator==(const Texture& other) const;
			bool operator!=(const Texture& other) const;
		private:
			std::shared_ptr<TextureImpl> texture;

			Color hue = Colors::INVISIBLE;

			Vector<float, 4> transform{ 0.0f, 0.0f, 1.0f, 1.0f };
		};//Texture

		class GraphicsContext: public Initializable {
			friend class Texture;
			friend class Model;
		public:
			typedef Texture(*TextureCreateCallback)();
			typedef Model(*ModelCreateCallback)();

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

			void createTexture(const std::string& name, const Texture& texture = Texture());
			Texture& getOrCreateTexture(const std::string& name, const TextureCreateCallback create);
			void createModel(const std::string& name, const Model& texture = Model());
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
			virtual std::shared_ptr<ModelImpl> createModelImpl() const = 0;
			virtual std::shared_ptr<TextureImpl> createTextureImpl() const = 0;

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