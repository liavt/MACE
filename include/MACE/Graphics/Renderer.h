/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_RENDERER_H
#define MACE__GRAPHICS_RENDERER_H

#include <MACE/Core/Interfaces.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Utility/Vector.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/Color.h>

#include <deque>
#include <stack>

#ifdef MACE_OPENCV
#	include <opencv2/opencv.hpp>
#endif//MACE_OPENCV

namespace mc {
	namespace gfx {
		/**
		Thrown when an error occured trying to read or write an image
		*/
		MACE__DECLARE_ERROR(BadImage);

		/**
		Thrown when a format is invalid
		*/
		MACE__DECLARE_ERROR(BadFormat);

		class TextureImpl;

		class Texture: public Initializable {
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

			Texture();
			Texture(const Color& col);
			Texture(const std::shared_ptr<TextureImpl> tex, const Color& col = Color(0.0f, 0.0f, 0.0f, 0.0f));
			Texture(const Texture& tex, const Color& col = Color(0.0, 0.0f, 0.0f, 0.0f));
			Texture(const char* file);
			Texture(const std::string& file);

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

				texture->resetPixelStorage();
				texture->setPackStorageHint(Texture::PixelStorage::ALIGNMENT, (mat.step & 3) ? 1 : 4);
				texture->setPackStorageHint(Texture::PixelStorage::ROW_LENGTH, static_cast<int>(mat.step / mat.elemSize()));

				texture->setData(mat.ptr(), mat.cols, mat.rows, type, colorFormat, Texture::InternalFormat::RGBA);
			}

			Texture(cv::Mat mat) : Texture() {
				init();
				load(mat);
			}

			cv::Mat toMat(const Size width, const Size height) {
				cv::Mat img(height, width, CV_8UC3);

				texture->resetPixelStorage();
				texture->setPackStorageHint(Texture::PixelStorage::ALIGNMENT, (img.step & 3) ? 1 : 4);
				texture->setPackStorageHint(Texture::PixelStorage::ROW_LENGTH, static_cast<int>(img.step / img.elemSize()));

				texture->bind();

				glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);

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

			void bind() const;
			void bind(const Index location) const;
			void unbind() const;

			void resetPixelStorage();

			void setMinFilter(const Texture::ResizeFilter filter);
			void setMagFilter(const Texture::ResizeFilter filter);

			void setData(const void* data, const Size width, const Size height, const Texture::Type type = Texture::Type::FLOAT, const Texture::Format format = Texture::Format::RGB, const Texture::InternalFormat internalFormat = Texture::InternalFormat::RGB, const Index mipmap = 0);

			void setUnpackStorageHint(const Texture::PixelStorage hint, const int value);
			void setPackStorageHint(const Texture::PixelStorage hint, const int value);

			void setWrap(const Texture::WrapMode wrap);
			void setWrapS(const Texture::WrapMode wrap);
			void setWrapT(const Texture::WrapMode wrap);

			bool operator==(const Texture& other) const;
			bool operator!=(const Texture& other) const;
		private:
			std::shared_ptr<TextureImpl> texture;

			Color paint;
		};//Texture

		class TextureImpl: public Initializable {
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void bind() const = 0;
			virtual void bind(const Index location) const = 0;
			virtual void unbind() const = 0;

			virtual void resetPixelStorage() = 0;

			virtual bool isCreated() const = 0;

			virtual void setData(const void* data, const Size width, const Size height, const Texture::Type type, const Texture::Format format, const Texture::InternalFormat internalFormat, const Index mipmap) = 0;

			virtual void setMinFilter(const Texture::ResizeFilter filter) = 0;
			virtual void setMagFilter(const Texture::ResizeFilter filter) = 0;

			virtual void setUnpackStorageHint(const Texture::PixelStorage hint, const int value) = 0;
			virtual void setPackStorageHint(const Texture::PixelStorage hint, const int value) = 0;

			virtual void setWrapS(const Texture::WrapMode wrap) = 0;
			virtual void setWrapT(const Texture::WrapMode wrap) = 0;
		};

		//if the container we use is ever going to be changed, we typedef
		using RenderQueue = std::deque<GraphicsEntity*>;

		//forward define for friend declaration in later classes
		class Renderer;
		class PainterImpl;

		class Painter: public Initializable {
		public:
			//painter stuff
			enum class Brush: Byte {
				TEXTURE = 0, COLOR = 1, MASK = 2
			};

			enum class RenderType: Byte {
				QUAD = 0
			};

			struct State {
				TransformMatrix transformation;

				Color color;

				Vector<float, 4> data;

				bool operator==(const State& other) const;
				bool operator!=(const State& other) const;
			};

			enum class DirtyFlag: Byte {
				PRIMARY_COLOR = 0,
				SECONDARY_COLOR = 1,
				TRANSLATION = 2,
				ROTATION = 3,
				SCALE = 4
			};

			Painter(GraphicsEntity* const en);

			void init() override;
			void destroy() override;

			std::shared_ptr<PainterImpl> getImplementation();
			const std::shared_ptr<PainterImpl> getImplementation() const;

			std::shared_ptr<PainterImpl> operator*();
			const std::shared_ptr<PainterImpl> operator*() const;

			std::shared_ptr<PainterImpl> operator->();
			const std::shared_ptr<PainterImpl> operator->() const;
		private:
			std::shared_ptr<PainterImpl> impl = nullptr;
		};

		class PainterImpl: public Initializable {
			friend class Renderer;
		public:
			virtual ~PainterImpl() noexcept = default;

			virtual void init() override = 0;
			virtual void destroy() override = 0;

			void fillRect(const float x = 0.0f, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f);
			void fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size);
			void fillRect(const Vector<float, 4>& dim);

			void drawImage(const Texture& img, const float x = 0.0f, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f);
			void drawImage(const Texture& img, const Vector<float, 2>& pos, const Vector<float, 2>& size);
			void drawImage(const Texture& img, const Vector<float, 4>& dim);

			void maskImage(const Texture& img, const Texture& mask, const float minimum = 0.0f, const float maximum = 1.0f, const float x = 0.0, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f);

			const GraphicsEntity* const getEntity() const;

			void setColor(const Color& col);
			Color& getColor();
			const Color& getColor() const;

			void setData(const Vector<float, 4>& col);
			Vector<float, 4>& getData();
			const Vector<float, 4>& getData() const;

			void resetColor();

			void setTransformation(const TransformMatrix& trans);
			TransformMatrix& getTransformation();
			const TransformMatrix& getTransformation() const;

			void translate(const Vector<float, 3>& vec);
			void translate(const float x, const float y, const float z = 0.0f);

			void rotate(const Vector<float, 3>& vec);
			void rotate(const float x, const float y, const float z);

			void scale(const Vector<float, 3>& vec);
			void scale(const float x, const float y, const float z = 0.0f);

			void resetTransform();

			void push();
			void pop();

			virtual void reset();

			virtual bool operator==(const PainterImpl& other) const;
			bool operator!=(const PainterImpl& other) const;
		protected:
			PainterImpl(const GraphicsEntity* const en);

			virtual void loadSettings() = 0;
			virtual void draw(const Painter::Brush brush, const Painter::RenderType type) = 0;

			const GraphicsEntity* const entity;

			std::stack<Painter::State> stateStack;
			Painter::State state;
		};

		Renderer* getRenderer();
		/**
		@param r New Renderer. Must be dynamically allocated as `delete` will be called with it.
		*/
		void setRenderer(Renderer* r);

		/**
		@todo add function to change how many samples msaa uses
		*/
		class Renderer {
		public:
			virtual ~Renderer() = default;

			virtual GraphicsEntity* getEntityAt(const int x, const int y) = 0;

			/**
			@opengl
			*/
			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) = 0;

			virtual void onResize(const Size width, const Size height) = 0;
			virtual void onInit(const Size originalWidth, const Size originalHeight) = 0;
			virtual void onSetUp(os::WindowModule* win) = 0;
			virtual void onTearDown(os::WindowModule* win) = 0;
			virtual void onDestroy() = 0;
			virtual void onQueue(GraphicsEntity* en) = 0;

			virtual std::shared_ptr<PainterImpl> getPainter(const GraphicsEntity * const entity) const = 0;
			virtual std::shared_ptr<TextureImpl> getTexture() const = 0;

			/**
			@internal
			@opengl
			*/
			void resize(const Size width, const Size height);

			/**
			@internal
			@opengl
			*/
			void init(const Size originalWidth, const Size originalHeight);

			/**
			@internal
			@opengl
			*/
			void setUp(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void tearDown(os::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void checkInput(os::WindowModule* win);


			/**
			@internal
			@opengl
			*/
			void destroy();

			/**
			@internal
			*/
			void flagResize();

			Index queue(GraphicsEntity* e);

			void remove(const Index id);

			/**
			@opengl
			*/
			void setRefreshColor(const Color& c);

			Size getOriginalWidth() const;
			Size getOriginalHeight() const;

			Size getWidth() const;
			Size getHeight() const;

			Size getSamples() const;

			Vector<float, 2> getWindowRatios() const;

			RenderQueue getRenderQueue() const;

			bool isResized() const;
		protected:
			Index pushEntity(GraphicsEntity*  entity);

			RenderQueue renderQueue = RenderQueue();

			Size originalWidth = 0;
			Size originalHeight = 0;

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;
		};//Renderer
	}//gfx
}//mc

#endif
