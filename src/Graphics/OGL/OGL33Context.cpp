/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/OGL/OGL33Context.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>

namespace mc {
	namespace gfx {
		namespace ogl {
			namespace {
				GLenum getFormat(const Texture::Format format) {
					switch (format) {
						case Texture::Format::RED:
							return GL_RED;
						case Texture::Format::RG:
							return GL_RG;
						case Texture::Format::RGB:
							return GL_RGB;
						case Texture::Format::RGBA:
							return GL_RGBA;
						case Texture::Format::BGR:
							return GL_BGR;
						case Texture::Format::BGRA:
							return GL_BGRA;
						case Texture::Format::RED_INTEGER:
							return GL_RED_INTEGER;
						case Texture::Format::RG_INTEGER:
							return GL_RG_INTEGER;
						case Texture::Format::RGB_INTEGER:
							return GL_RGB_INTEGER;
						case Texture::Format::BGR_INTEGER:
							return GL_BGR_INTEGER;
						case Texture::Format::RGBA_INTEGER:
							return GL_RGBA_INTEGER;
						case Texture::Format::BGRA_INTEGER:
							return GL_BGRA_INTEGER;
						case Texture::Format::STENCIL:
							return GL_STENCIL;
						case Texture::Format::DEPTH:
							return GL_DEPTH;
						case Texture::Format::DEPTH_STENCIL:
							return GL_DEPTH_STENCIL;
						case Texture::Format::LUMINANCE:
							return GL_LUMINANCE;
						case Texture::Format::LUMINANCE_ALPHA:
							return GL_LUMINANCE_ALPHA;
						default:
							MACE__THROW(BadFormat, "Unsupported format by OpenGL");
					}
				}

				GLenum getInternalFormat(const Texture::InternalFormat format) {
					switch (format) {
						case Texture::InternalFormat::DEPTH:
							return GL_DEPTH_COMPONENT;
						case Texture::InternalFormat::DEPTH_STENCIL:
							return GL_DEPTH_STENCIL;
						case Texture::InternalFormat::RED:
							return GL_RED;
						case Texture::InternalFormat::RG:
							return GL_RG;
						case Texture::InternalFormat::RGB:
							return GL_RGB;
						case Texture::InternalFormat::RGBA:
							return GL_RGBA;
						case Texture::InternalFormat::R32UI:
							return GL_R32UI;
						default:
							MACE__THROW(BadFormat, "Unsupported internal format by OpenGL");
					}
				}

				GLenum getType(const Texture::Type type) {
					switch (type) {
						case Texture::Type::UNSIGNED_BYTE:
							return GL_UNSIGNED_BYTE;
						case Texture::Type::BYTE:
							return GL_BYTE;
						case Texture::Type::UNSIGNED_SHORT:
							return GL_UNSIGNED_SHORT;
						case Texture::Type::SHORT:
							return GL_SHORT;
						case Texture::Type::UNSIGNED_INT:
							return GL_UNSIGNED_INT;
						case Texture::Type::INT:
							return GL_INT;
						case Texture::Type::FLOAT:
							return GL_FLOAT;
						case Texture::Type::UNSIGNED_BYTE_3_3_2:
							return GL_UNSIGNED_BYTE_3_3_2;
						case Texture::Type::UNSIGNED_BYTE_2_3_3_REV:
							return GL_UNSIGNED_BYTE_2_3_3_REV;
						case Texture::Type::UNSIGNED_SHORT_5_6_5:
							return GL_UNSIGNED_SHORT_5_6_5;
						case Texture::Type::UNSIGNED_SHORT_5_6_5_REV:
							return GL_UNSIGNED_SHORT_5_6_5_REV;
						case Texture::Type::UNSIGNED_SHORT_4_4_4_4:
							return GL_UNSIGNED_SHORT_4_4_4_4;
						case Texture::Type::UNSIGNED_SHORT_4_4_4_4_REV:
							return GL_UNSIGNED_SHORT_4_4_4_4_REV;
						case Texture::Type::UNSIGNED_SHORT_5_5_5_1:
							return GL_UNSIGNED_SHORT_5_5_5_1;
						case Texture::Type::UNSIGNED_SHORT_1_5_5_5_REV:
							return GL_UNSIGNED_SHORT_1_5_5_5_REV;
						case Texture::Type::UNSIGNED_INT_8_8_8_8:
							return GL_UNSIGNED_INT_8_8_8_8;
						case Texture::Type::UNSIGNED_INT_8_8_8_8_REV:
							return GL_UNSIGNED_INT_8_8_8_8_REV;
						case Texture::Type::UNSIGNED_INT_10_10_10_2:
							return GL_UNSIGNED_INT_10_10_10_2;
						case Texture::Type::UNSIGNED_INT_2_10_10_10_REV:
							return GL_UNSIGNED_INT_2_10_10_10_REV;
						default:
							MACE__THROW(BadFormat, "Unsupported type by OpenGL");
					}
				}
			}//anon namespace

			void OGL33Texture::init() {
				ogl::Texture2D::init();
			}

			void OGL33Texture::destroy() {
				ogl::Texture2D::destroy();
			}

			void OGL33Texture::bind() const {
				ogl::Texture2D::bind();
			}

			void OGL33Texture::bind(const Index location) const {
				ogl::Texture2D::bind(location);
			}

			void OGL33Texture::unbind() const {
				ogl::Texture2D::unbind();
			}

			bool OGL33Texture::isCreated() const {
				return ogl::Texture2D::isCreated();
			}

			void OGL33Texture::setMinFilter(const gfx::Texture::ResizeFilter filter) {
				if (filter == gfx::Texture::ResizeFilter::MIPMAP_LINEAR) {
					generateMipmap();
					setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				} else if (filter == gfx::Texture::ResizeFilter::MIPMAP_NEAREST) {
					generateMipmap();
					setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else if (filter == gfx::Texture::ResizeFilter::LINEAR) {
					setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				} else if (filter == gfx::Texture::ResizeFilter::NEAREST) {
					setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				}
			}

			void OGL33Texture::setMagFilter(const gfx::Texture::ResizeFilter filter) {
				if (filter == gfx::Texture::ResizeFilter::MIPMAP_LINEAR ||
					filter == gfx::Texture::ResizeFilter::MIPMAP_NEAREST) {
					MACE__THROW(BadFormat, "Mipmap resize filtering can't be used as a magnification filter with OpenGL");
				} else if (filter == gfx::Texture::ResizeFilter::LINEAR) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				} else if (filter == gfx::Texture::ResizeFilter::NEAREST) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}
			}

			void OGL33Texture::setUnpackStorageHint(const gfx::Texture::PixelStorage hint, const int value) {
				switch (hint) {
					case gfx::Texture::PixelStorage::ALIGNMENT:
						setPixelStorage(GL_UNPACK_ALIGNMENT, value);
						break;
					case gfx::Texture::PixelStorage::ROW_LENGTH:
						setPixelStorage(GL_UNPACK_ROW_LENGTH, value);
						break;
					default:
						MACE__THROW(BadFormat, "Specified hint is unavialable for OpenGL");
				}
			}

			void OGL33Texture::setPackStorageHint(const gfx::Texture::PixelStorage hint, const int value) {
				switch (hint) {
					case gfx::Texture::PixelStorage::ALIGNMENT:
						setPixelStorage(GL_PACK_ALIGNMENT, value);
						break;
					case gfx::Texture::PixelStorage::ROW_LENGTH:
						setPixelStorage(GL_PACK_ROW_LENGTH, value);
						break;
					default:
						MACE__THROW(BadFormat, "Specified hint is unavialable for OpenGL");
				}
			}

			void OGL33Texture::setWrapS(const Texture::WrapMode wrap) {
				if (wrap == Texture::WrapMode::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				} else if (wrap == Texture::WrapMode::REPEAT) {
					setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
				} else if (wrap == Texture::WrapMode::MIRRORED_REPEAT) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				} else if (wrap == Texture::WrapMode::MIRROR_CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture");
				}
			}

			void OGL33Texture::setWrapT(const Texture::WrapMode wrap) {
				if (wrap == Texture::WrapMode::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				} else if (wrap == Texture::WrapMode::REPEAT) {
					setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
				} else if (wrap == Texture::WrapMode::MIRRORED_REPEAT) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				} else if (wrap == Texture::WrapMode::MIRROR_CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture");
				}
			}

			void OGL33Texture::setData(const void * data, const Size width, const Size height, const Texture::Type type, const Texture::Format format, const Texture::InternalFormat internalFormat, const Index mipmap) {
				ogl::Texture2D::setData(data, width, height, getType(type), getFormat(format), getInternalFormat(internalFormat), mipmap);
			}

			void OGL33Texture::getImage(const Texture::Format format, const Texture::Type type, void * data) const {
				ogl::Texture2D::getImage(getFormat(format), getType(type), data);
			}

			void OGL33Texture::setSwizzle(const Texture::SwizzleMode mode, const Texture::SwizzleMode arg) {
				Enum swizzle;
				if (arg == Texture::SwizzleMode::R) {
					swizzle = GL_RED;
				} else if (arg == Texture::SwizzleMode::G) {
					swizzle = GL_GREEN;
				} else if (arg == Texture::SwizzleMode::B) {
					swizzle = GL_BLUE;
				} else if (arg == Texture::SwizzleMode::A) {
					swizzle = GL_ALPHA;
				} else {
					MACE__THROW(BadFormat, "OpenGL: Unsupported SwizzleMode for argument arg");
				}

				if (mode == Texture::SwizzleMode::R) {
					setParameter(GL_TEXTURE_SWIZZLE_R, swizzle);
				} else if (mode == Texture::SwizzleMode::G) {
					setParameter(GL_TEXTURE_SWIZZLE_G, swizzle);
				} else if (mode == Texture::SwizzleMode::B) {
					setParameter(GL_TEXTURE_SWIZZLE_B, swizzle);
				} else if (mode == Texture::SwizzleMode::A) {
					setParameter(GL_TEXTURE_SWIZZLE_A, swizzle);
				} else {
					MACE__THROW(BadFormat, "OpenGL: Unsupported SwizzleMode for argument mode");
				}
			}

			void OGL33Model::init() {
				ogl::VertexArray::init();
			}

			void OGL33Model::destroy() {
				ogl::VertexArray::destroy();
			}

			void OGL33Context::onInit(os::WindowModule *) {
				renderer = std::shared_ptr<Renderer>(new OGL33Renderer());
			}

			void OGL33Context::onRender(os::WindowModule *) {}

			void OGL33Context::onDestroy(os::WindowModule *) {
				renderer.reset();
			}

			OGL33Context::OGL33Context(os::WindowModule * win) : GraphicsContext(win) {}

			std::shared_ptr<Renderer> OGL33Context::getRenderer() const {
				return renderer;
			}

			std::shared_ptr<ModelImpl> OGL33Context::createModelImpl() const {
				return std::shared_ptr<ModelImpl>(new OGL33Model());
			}

			std::shared_ptr<TextureImpl> OGL33Context::createTextureImpl() const {
				return std::shared_ptr<TextureImpl>(new OGL33Texture());
			}
		}//ogl
	}//gfx
}//mc