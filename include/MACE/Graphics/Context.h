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

		class ModelImpl: public Initializable {
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;
		};

		class Model: public Initializable {
		public:

		};

		class TextureImpl;

		class Texture: public Initializable, public Bindable {
		public:
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

			/**
			@todo finish adding all the internal formats. Update the getInternalFormat in OGLRenderer.cpp with the new enum values
			*/
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

			Texture();
			Texture(const Color& col);
			Texture(const std::shared_ptr<TextureImpl> tex, const Color& col = Color(0.0f, 0.0f, 0.0f, 0.0f));
			Texture(const Texture& tex, const Color& col = Color(0.0, 0.0f, 0.0f, 0.0f));
			explicit Texture(const char* file);
			explicit Texture(const std::string& file);

			void init() override;
			void destroy() override;

			bool isCreated() const;

			//this needs to be defined in the header file to prevent linker conflicts, because Entity.cpp does not have opencv included ever.
#			ifdef MACE_OPENCV
			void load(cv::Mat mat) {
				//cv::flip(mat, mat, 0);

				Texture::Format colorFormat = Texture::Format::BGR;
				if (mat.channels() == 1) {
					colorFormat = Texture::Format::LUMINANCE;
				} else if (mat.channels() == 2) {
					colorFormat = Texture::Format::LUMINANCE_ALPHA;
				} else if (mat.channels() == 4) {
					colorFormat = Texture::Format::BGRA;
				}

				Texture::Type type = Texture::Type::UNSIGNED_BYTE;
				if (mat.depth() == CV_8S) {
					type = Texture::Type::BYTE;
				} else if (mat.depth() == CV_16U) {
					type = Texture::Type::UNSIGNED_SHORT;
				} else if (mat.depth() == CV_16S) {
					type = Texture::Type::SHORT;
				} else if (mat.depth() == CV_32S) {
					type = Texture::Type::INT;
				} else if (mat.depth() == CV_32F) {
					type = Texture::Type::FLOAT;
				} else if (mat.depth() == CV_64F) {
					MACE__THROW(BadFormat, "Unsupported cv::Mat depth: CV_64F");
				}

				resetPixelStorage();
				setPackStorageHint(Texture::PixelStorage::ALIGNMENT, (mat.step & 3) ? 1 : 4);
				setPackStorageHint(Texture::PixelStorage::ROW_LENGTH, static_cast<int>(mat.step / mat.elemSize()));

				setData(mat.ptr(), mat.cols, mat.rows, type, colorFormat, Texture::InternalFormat::RGBA);
			}

			Texture(cv::Mat mat) : Texture() {
				init();
				load(mat);
			}

			cv::Mat toMat(const Size width, const Size height) {
				cv::Mat img(height, width, CV_8UC3);

				resetPixelStorage();
				setPackStorageHint(Texture::PixelStorage::ALIGNMENT, (img.step & 3) ? 1 : 4);
				setPackStorageHint(Texture::PixelStorage::ROW_LENGTH, static_cast<int>(img.step / img.elemSize()));

				bind();

				getImage(Texture::Format::BGR, Texture::Type::UNSIGNED_BYTE, img.data);

				cv::flip(img, img, 0);

				return img;
			}
#			endif//MACE_OPENCV

			void load(const char* file);
			void load(const std::string& file);
			void load(const Color& c);
			void load(const unsigned char* c, const Size size);
			template<std::size_t S>
			void load(const unsigned char c[S]) {
				load(c, S);
			}

			Color& getPaint();
			const Color& getPaint() const;
			void setPaint(const Color& col);

			void bind() const override;
			void bind(const Index location) const;
			void unbind() const override;

			void resetPixelStorage();

			void setMinFilter(const Texture::ResizeFilter filter);
			void setMagFilter(const Texture::ResizeFilter filter);

			void setData(const void* data, const Size width, const Size height, const Texture::Type type = Texture::Type::FLOAT, const Texture::Format format = Texture::Format::RGB, const Texture::InternalFormat internalFormat = Texture::InternalFormat::RGB, const Index mipmap = 0);

			void setUnpackStorageHint(const Texture::PixelStorage hint, const int value);
			void setPackStorageHint(const Texture::PixelStorage hint, const int value);

			void setWrap(const Texture::WrapMode wrap);
			void setWrapS(const Texture::WrapMode wrap);
			void setWrapT(const Texture::WrapMode wrap);

			void setSwizzle(const Texture::SwizzleMode mode, const Texture::SwizzleMode arg);

			void getImage(const Texture::Format format, const Texture::Type type, void* data) const;

			bool operator==(const Texture& other) const;
			bool operator!=(const Texture& other) const;
		private:
			std::shared_ptr<TextureImpl> texture;

			Color paint;
		};//Texture

		class TextureImpl: public Initializable, public Bindable {
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void bind() const override = 0;
			virtual void bind(const Index location) const = 0;
			virtual void unbind() const override = 0;

			virtual bool isCreated() const = 0;

			virtual void setData(const void* data, const Size width, const Size height, const Texture::Type type, const Texture::Format format, const Texture::InternalFormat internalFormat, const Index mipmap) = 0;

			virtual void setMinFilter(const Texture::ResizeFilter filter) = 0;
			virtual void setMagFilter(const Texture::ResizeFilter filter) = 0;

			virtual void setUnpackStorageHint(const Texture::PixelStorage hint, const int value) = 0;
			virtual void setPackStorageHint(const Texture::PixelStorage hint, const int value) = 0;

			virtual void setWrapS(const Texture::WrapMode wrap) = 0;
			virtual void setWrapT(const Texture::WrapMode wrap) = 0;

			virtual void getImage(const Texture::Format format, const Texture::Type type, void* data) const = 0;

			virtual void setSwizzle(const Texture::SwizzleMode mode, const Texture::SwizzleMode arg) = 0;
		};

		class GraphicsContext: public Initializable {
			friend class Texture;
			friend class Model;
		public:
			GraphicsContext(os::WindowModule* win);
			//prevent copying
			GraphicsContext(const GraphicsContext& other) = delete;
			virtual ~GraphicsContext() = default;

			void init() override;
			void render();
			void destroy() override;

			virtual std::shared_ptr<Renderer> getRenderer() const = 0;

			os::WindowModule* getWindow();
			const os::WindowModule* getWindow() const;

			void createTexture(const std::string& name, const Texture& texture = Texture());
			
			bool hasTexture(const std::string& name) const;

			void setTexture(const std::string& name, const Texture& texture);
			Texture& getTexture(const std::string& name);
			const Texture& getTexture(const std::string& name) const;

			std::map<std::string, Texture>& getTextures();
			const std::map<std::string, Texture>& getTextures() const;

		//	std::map<std::string, Model>& getModels();
			//const std::map<std::string, Model>& getModels() const;
		protected:
			os::WindowModule* window;

			virtual std::shared_ptr<ModelImpl> createModelImpl() const = 0;
			virtual std::shared_ptr<TextureImpl> createTextureImpl() const = 0;

			virtual void onInit(os::WindowModule* win) = 0;
			virtual void onRender(os::WindowModule* win) = 0;
			virtual void onDestroy(os::WindowModule* win) = 0;

		private:
			std::map<std::string, Texture> textures{};
		};
	}
}//mc

#endif//MACE__GRAPHICS_CONTEXT_H