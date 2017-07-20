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
				LUMINANCE,
				LUMINANCE_ALPHA
			};

			enum class InternalFormat: Enum {
				DEPTH,
				DEPTH_STENCIL,
				RED,
				RG,
				RGB,
				RGBA,
				R32UI
			};

			enum class ResizeFilter: Byte {
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
				CLAMP,
				REPEAT,
				MIRRORED_REPEAT,
				MIRROR_CLAMP,
			};

			//there is no ZERO and ONE like in OpenGL because DirectX doesn't support that
			enum class SwizzleMode: Byte {
				R,
				G,
				B,
				A
			};

			enum class ImageFormat: Byte {
				GRAY,
				GRAY_ALPHA,
				R,
				RG,
				RGB,
				RGBA,
				DONT_CARE
			};
		}

		class ModelImpl: public Initializable, public Bindable {
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void bind() const override = 0;
			virtual void unbind() const override = 0;

			virtual void loadTextureCoordinates(const Size dataSize, const float data[]) = 0;
			virtual void loadVertices(const Size verticeSize, const float vertices[]) = 0;
			virtual void loadIndices(const Size indiceNum, const unsigned int* indiceData) = 0;

			virtual bool isCreated() const = 0;
		};

		class Model: public ModelImpl {
		public:
			Model();
			Model(const std::shared_ptr<ModelImpl> mod);
			Model(const Model& other);
			~Model() = default;

			void init() override;
			void destroy() override;

			void bind() const override;
			void unbind() const override;

			void loadTextureCoordinates(const Size dataSize, const float* data) override;
			template<std::size_t N>
			void loadTextureCoordinates(const float data[N]) {
				loadTextureCoordinates(N, data);
			}
			void loadVertices(const Size verticeSize, const float* vertices) override;
			template<std::size_t N>
			void loadVertices(const float* vertices) {
				loadVertices(N, vertices);
			}

			void loadIndices(const Size indiceNum, const unsigned int* indiceData) override;
			template<std::size_t N>
			void loadIndices(const unsigned int* indiceData) {
				loadIndices(N, indiceData);
			}

			bool isCreated() const override;

			bool operator==(const Model& other) const;
			bool operator!=(const Model& other) const;
		private:
			std::shared_ptr<ModelImpl> model;
		};

		class TextureImpl: public Initializable, public Bindable {
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void bind() const override = 0;
			virtual void bind(const unsigned int location) const = 0;
			virtual void unbind() const override = 0;

			virtual bool isCreated() const = 0;

			virtual void setData(const void* data, const Size width, const Size height, const Enums::Type type, const Enums::Format format, const Enums::InternalFormat internalFormat, const Index mipmap) = 0;

			virtual void setMinFilter(const Enums::ResizeFilter filter) = 0;
			virtual void setMagFilter(const Enums::ResizeFilter filter) = 0;

			virtual void setUnpackStorageHint(const Enums::PixelStorage hint, const int value) = 0;
			virtual void setPackStorageHint(const Enums::PixelStorage hint, const int value) = 0;

			virtual void setWrapS(const Enums::WrapMode wrap) = 0;
			virtual void setWrapT(const Enums::WrapMode wrap) = 0;

			virtual void getImage(const Enums::Format format, const Enums::Type type, void* data) const = 0;

			virtual void setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg) = 0;
		};

		class Texture: public TextureImpl{
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
				} else if (mat.depth() == CV_32F) {;
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

			Color& getPaint();
			const Color& getPaint() const;
			void setPaint(const Color& col);

			void bind() const override;
			void bind(const unsigned int location) const override;
			void unbind() const override;

			void resetPixelStorage();

			void setMinFilter(const Enums::ResizeFilter filter) override;
			void setMagFilter(const Enums::ResizeFilter filter) override;

			void setData(const void* data, const Size width, const Size height,
						 const Enums::Type type = Enums::Type::FLOAT, const Enums::Format format = Enums::Format::RGB,
						 const Enums::InternalFormat internalFormat = Enums::InternalFormat::RGB, const Index mipmap = 0) override;

			void setUnpackStorageHint(const Enums::PixelStorage hint, const int value) override;
			void setPackStorageHint(const Enums::PixelStorage hint, const int value) override;

			void setWrap(const Enums::WrapMode wrap);
			void setWrapS(const Enums::WrapMode wrap) override;
			void setWrapT(const Enums::WrapMode wrap) override;

			void setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg) override;

			void getImage(const Enums::Format format, const Enums::Type type, void* data) const override;

			bool operator==(const Texture& other) const;
			bool operator!=(const Texture& other) const;
		private:
			std::shared_ptr<TextureImpl> texture;

			Color paint;
		};//Texture

		class GraphicsContext: public Initializable {
			friend class Texture;
			friend class Model;
		public:
			typedef Texture(*TextureCreateCallback)();

			GraphicsContext(gfx::WindowModule* win);
			//prevent copying
			GraphicsContext(const GraphicsContext& other) = delete;
			virtual ~GraphicsContext() = default;

			void init() override;
			void render();
			void destroy() override;

			virtual std::shared_ptr<Renderer> getRenderer() const = 0;

			gfx::WindowModule* getWindow();
			const gfx::WindowModule* getWindow() const;

			void createTexture(const std::string& name, const Texture& texture = Texture());
			Texture& getOrCreateTexture(const std::string& name, const TextureCreateCallback create);
			void createModel(const std::string& name, const Model& texture = Model());

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