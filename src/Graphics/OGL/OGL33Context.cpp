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
				GLenum getFormat(const Enums::Format format) {
					switch (format) {
						case Enums::Format::RED:
							return GL_RED;
						case Enums::Format::RG:
							return GL_RG;
						case Enums::Format::RGB:
							return GL_RGB;
						case Enums::Format::RGBA:
							return GL_RGBA;
						case Enums::Format::BGR:
							return GL_BGR;
						case Enums::Format::BGRA:
							return GL_BGRA;
						case Enums::Format::RED_INTEGER:
							return GL_RED_INTEGER;
						case Enums::Format::RG_INTEGER:
							return GL_RG_INTEGER;
						case Enums::Format::RGB_INTEGER:
							return GL_RGB_INTEGER;
						case Enums::Format::BGR_INTEGER:
							return GL_BGR_INTEGER;
						case Enums::Format::RGBA_INTEGER:
							return GL_RGBA_INTEGER;
						case Enums::Format::BGRA_INTEGER:
							return GL_BGRA_INTEGER;
						case Enums::Format::STENCIL:
							return GL_STENCIL;
						case Enums::Format::DEPTH:
							return GL_DEPTH;
						case Enums::Format::DEPTH_STENCIL:
							return GL_DEPTH_STENCIL;
						case Enums::Format::LUMINANCE:
							return GL_LUMINANCE;
						case Enums::Format::LUMINANCE_ALPHA:
							return GL_LUMINANCE_ALPHA;
						default:
							MACE__THROW(BadFormat, "Unsupported format by OpenGL");
					}
				}

				GLenum getInternalFormat(const Enums::InternalFormat format) {
					switch (format) {
						case Enums::InternalFormat::DEPTH:
							return GL_DEPTH_COMPONENT;
						case Enums::InternalFormat::DEPTH_STENCIL:
							return GL_DEPTH_STENCIL;
						case Enums::InternalFormat::RED:
							return GL_RED;
						case Enums::InternalFormat::RG:
							return GL_RG;
						case Enums::InternalFormat::RGB:
							return GL_RGB;
						case Enums::InternalFormat::RGBA:
							return GL_RGBA;
						case Enums::InternalFormat::R32UI:
							return GL_R32UI;
						default:
							MACE__THROW(BadFormat, "Unsupported internal format by OpenGL");
					}
				}

				GLenum getType(const Enums::Type type) {
					switch (type) {
						case Enums::Type::UNSIGNED_BYTE:
							return GL_UNSIGNED_BYTE;
						case Enums::Type::BYTE:
							return GL_BYTE;
						case Enums::Type::UNSIGNED_SHORT:
							return GL_UNSIGNED_SHORT;
						case Enums::Type::SHORT:
							return GL_SHORT;
						case Enums::Type::UNSIGNED_INT:
							return GL_UNSIGNED_INT;
						case Enums::Type::INT:
							return GL_INT;
						case Enums::Type::FLOAT:
							return GL_FLOAT;
						case Enums::Type::UNSIGNED_BYTE_3_3_2:
							return GL_UNSIGNED_BYTE_3_3_2;
						case Enums::Type::UNSIGNED_BYTE_2_3_3_REV:
							return GL_UNSIGNED_BYTE_2_3_3_REV;
						case Enums::Type::UNSIGNED_SHORT_5_6_5:
							return GL_UNSIGNED_SHORT_5_6_5;
						case Enums::Type::UNSIGNED_SHORT_5_6_5_REV:
							return GL_UNSIGNED_SHORT_5_6_5_REV;
						case Enums::Type::UNSIGNED_SHORT_4_4_4_4:
							return GL_UNSIGNED_SHORT_4_4_4_4;
						case Enums::Type::UNSIGNED_SHORT_4_4_4_4_REV:
							return GL_UNSIGNED_SHORT_4_4_4_4_REV;
						case Enums::Type::UNSIGNED_SHORT_5_5_5_1:
							return GL_UNSIGNED_SHORT_5_5_5_1;
						case Enums::Type::UNSIGNED_SHORT_1_5_5_5_REV:
							return GL_UNSIGNED_SHORT_1_5_5_5_REV;
						case Enums::Type::UNSIGNED_INT_8_8_8_8:
							return GL_UNSIGNED_INT_8_8_8_8;
						case Enums::Type::UNSIGNED_INT_8_8_8_8_REV:
							return GL_UNSIGNED_INT_8_8_8_8_REV;
						case Enums::Type::UNSIGNED_INT_10_10_10_2:
							return GL_UNSIGNED_INT_10_10_10_2;
						case Enums::Type::UNSIGNED_INT_2_10_10_10_REV:
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

			void OGL33Texture::bind(const unsigned int location) const {
				ogl::Texture2D::bind(location);
			}

			void OGL33Texture::unbind() const {
				ogl::Texture2D::unbind();
			}

			bool OGL33Texture::isCreated() const {
				return ogl::Texture2D::isCreated();
			}

			void OGL33Texture::setMinFilter(const Enums::ResizeFilter filter) {
				if (filter == Enums::ResizeFilter::MIPMAP_LINEAR) {
					generateMipmap();
					setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				} else if (filter == Enums::ResizeFilter::MIPMAP_NEAREST) {
					generateMipmap();
					setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else if (filter == Enums::ResizeFilter::LINEAR) {
					setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				} else if (filter == Enums::ResizeFilter::NEAREST) {
					setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				}
			}

			void OGL33Texture::setMagFilter(const Enums::ResizeFilter filter) {
				if (filter == Enums::ResizeFilter::MIPMAP_LINEAR ||
					filter == Enums::ResizeFilter::MIPMAP_NEAREST) {
					MACE__THROW(BadFormat, "Mipmap resize filtering can't be used as a magnification filter with OpenGL");
				} else if (filter == Enums::ResizeFilter::LINEAR) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				} else if (filter == Enums::ResizeFilter::NEAREST) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}
			}

			void OGL33Texture::setUnpackStorageHint(const Enums::PixelStorage hint, const int value) {
				switch (hint) {
					case Enums::PixelStorage::ALIGNMENT:
						setPixelStorage(GL_UNPACK_ALIGNMENT, value);
						break;
					case Enums::PixelStorage::ROW_LENGTH:
						setPixelStorage(GL_UNPACK_ROW_LENGTH, value);
						break;
					default:
						MACE__THROW(BadFormat, "Specified hint is unavialable for OpenGL");
				}
			}

			void OGL33Texture::setPackStorageHint(const Enums::PixelStorage hint, const int value) {
				switch (hint) {
					case Enums::PixelStorage::ALIGNMENT:
						setPixelStorage(GL_PACK_ALIGNMENT, value);
						break;
					case Enums::PixelStorage::ROW_LENGTH:
						setPixelStorage(GL_PACK_ROW_LENGTH, value);
						break;
					default:
						MACE__THROW(BadFormat, "Specified hint is unavialable for OpenGL");
				}
			}

			void OGL33Texture::setWrapS(const Enums::WrapMode wrap) {
				if (wrap == Enums::WrapMode::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				} else if (wrap == Enums::WrapMode::REPEAT) {
					setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRRORED_REPEAT) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRROR_CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture");
				}
			}

			void OGL33Texture::setWrapT(const Enums::WrapMode wrap) {
				if (wrap == Enums::WrapMode::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				} else if (wrap == Enums::WrapMode::REPEAT) {
					setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRRORED_REPEAT) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRROR_CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture");
				}
			}

			void OGL33Texture::setData(const void * data, const Size width, const Size height, const Enums::Type type, const Enums::Format format, const Enums::InternalFormat internalFormat, const Index mipmap) {
				ogl::Texture2D::setData(data, width, height, getType(type), getFormat(format), getInternalFormat(internalFormat), mipmap);
			}

			void OGL33Texture::getImage(const Enums::Format format, const Enums::Type type, void * data) const {
				ogl::Texture2D::getImage(getFormat(format), getType(type), data);
			}

			void OGL33Texture::setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg) {
				Enum swizzle;
				if (arg == Enums::SwizzleMode::R) {
					swizzle = GL_RED;
				} else if (arg == Enums::SwizzleMode::G) {
					swizzle = GL_GREEN;
				} else if (arg == Enums::SwizzleMode::B) {
					swizzle = GL_BLUE;
				} else if (arg == Enums::SwizzleMode::A) {
					swizzle = GL_ALPHA;
				} else {
					MACE__THROW(BadFormat, "OpenGL: Unsupported SwizzleMode for argument arg");
				}

				if (mode == Enums::SwizzleMode::R) {
					setParameter(GL_TEXTURE_SWIZZLE_R, swizzle);
				} else if (mode == Enums::SwizzleMode::G) {
					setParameter(GL_TEXTURE_SWIZZLE_G, swizzle);
				} else if (mode == Enums::SwizzleMode::B) {
					setParameter(GL_TEXTURE_SWIZZLE_B, swizzle);
				} else if (mode == Enums::SwizzleMode::A) {
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

			void OGL33Model::bind() const {
				ogl::VertexArray::bind();
			}

			void OGL33Model::unbind() const {
				ogl::VertexArray::unbind();
			}

			void OGL33Model::loadTextureCoordinates(const Size dataSize, const float* data) {
				ogl::VertexArray::storeDataInAttributeList(dataSize * sizeof(float), data, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION, 2);
			}

			void OGL33Model::loadVertices(const Size verticeSize, const float* vertices) {
				ogl::VertexArray::loadVertices(verticeSize, vertices, MACE__VAO_DEFAULT_VERTICES_LOCATION, 3);
			}

			void OGL33Model::loadIndices(const Size indiceNum, const unsigned int * indiceData) {
				ogl::VertexArray::loadIndices(indiceNum, indiceData);
			}

			bool OGL33Model::isCreated() const {
				return ogl::VertexArray::isCreated();
			}

			void OGL33Context::onInit(gfx::WindowModule *) {
				renderer = std::shared_ptr<Renderer>(new OGL33Renderer());
			}

			void OGL33Context::onRender(gfx::WindowModule *) {}

			void OGL33Context::onDestroy(gfx::WindowModule *) {
				renderer.reset();
			}

			OGL33Context::OGL33Context(gfx::WindowModule * win) : GraphicsContext(win) {}

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