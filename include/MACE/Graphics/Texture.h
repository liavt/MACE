/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_TEXTURE_H
#define MACE__GRAPHICS_TEXTURE_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Error.h>
#include <MACE/Utility/Color.h>
#include <MACE/Utility/Vector.h>
#include <MACE/Utility/Transform.h>

#include <memory>
#include <string>
#include <functional>

#ifdef MACE_OPENCV
#	include <opencv2/opencv.hpp>
#endif//MACE_OPENCV

namespace mc {
	namespace gfx {
		class GraphicsContextComponent;

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

		using TextureSlot = Byte;
	}//gfx

	namespace internal {
		template<typename T>
		constexpr gfx::TextureSlot getTextureSlot(T value) {
			return static_cast<gfx::TextureSlot>(value);
		}

		template<gfx::TextureSlot>
		constexpr gfx::TextureSlot getTextureSlot(gfx::TextureSlot value) {
			return value;
		}
	}

	namespace gfx {
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

		using TextureSetDataCallback = std::function<const void* ()>;
		using TextureReadDataCallback = std::function<void(void*)>;

		class MACE_NOVTABLE TextureImpl {
			friend class Texture;
		public:
			TextureImpl(const TextureDesc& t);
			virtual MACE__DEFAULT_OPERATORS(TextureImpl);

			virtual void bindTextureSlot(const TextureSlot slot) const = 0;

			virtual void setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const PixelStorageHints hints) = 0;

			virtual void readPixels(void* data, const PixelStorageHints hints) const = 0;
		protected:
			const TextureDesc desc;
		};

		class Texture {
			friend class Painter;
		public:
			Texture();
			Texture(GraphicsContextComponent* context, const TextureDesc& d);
			Texture(GraphicsContextComponent* context, const Color& col);
			Texture(const Texture& tex);
			Texture(const Texture& tex, const Color& col);

			/**
			@rendercontext
			*/
			void init(GraphicsContextComponent* context, const TextureDesc& desc);
			/**
			@rendercontext
			*/
			void destroy();

			bool isCreated() const noexcept;

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

			Color& getHue();
			const Color& getHue() const;
			void setHue(const Color& col);

			Vector<RelativeUnit, 4> & getTransform();
			const Vector<RelativeUnit, 4> & getTransform() const;
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

				MACE_ASSERT(W == texture->desc.width || H == texture->desc.height, "Input data is not equal to Texture width and height");

				setData(static_cast<const void*>(data[0]), mipmap, hints);
			}
			/**
			@rendercontext
			*/
			void readPixels(void* data, const PixelStorageHints hints = PixelStorageHints()) const;

			bool operator==(const Texture& other) const;
			bool operator!=(const Texture& other) const;
		private:
			std::shared_ptr<TextureImpl> texture;

			Color hue = Colors::INVISIBLE;

			Vector<RelativeUnit, 4> transform{0.0f, 0.0f, 1.0f, 1.0f};

			Texture(const std::shared_ptr<TextureImpl> tex, const Color& col = Color(0.0f, 0.0f, 0.0f, 0.0f));

			void bindTextureSlot(const TextureSlot slot) const;
		};//Texture
	}
}//mc

#endif//MACE__GRAPHICS_TEXTURE_H
