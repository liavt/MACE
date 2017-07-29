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
				ogl::Texture2D::bind();
				if (filter == Enums::ResizeFilter::MIPMAP_LINEAR) {
					ogl::Texture2D::generateMipmap();
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				} else if (filter == Enums::ResizeFilter::MIPMAP_NEAREST) {
					ogl::Texture2D::generateMipmap();
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else if (filter == Enums::ResizeFilter::LINEAR) {
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				} else if (filter == Enums::ResizeFilter::NEAREST) {
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else {
					MACE__THROW(UnsupportedRenderer, "Unsupported ResizeFilter for OpenGL: " + std::to_string(static_cast<Byte>(filter)));
				}
			}

			void OGL33Texture::setMagFilter(const Enums::ResizeFilter filter) {
				ogl::Texture2D::bind();
				if (filter == Enums::ResizeFilter::MIPMAP_LINEAR ||
					filter == Enums::ResizeFilter::MIPMAP_NEAREST) {
					MACE__THROW(UnsupportedRenderer, "Mipmap resize filtering can't be used as a magnification filter with OpenGL");
				} else if (filter == Enums::ResizeFilter::LINEAR) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				} else if (filter == Enums::ResizeFilter::NEAREST) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				} else {
					MACE__THROW(UnsupportedRenderer, "Unsupported ResizeFilter for OpenGL: " + std::to_string(static_cast<Byte>(filter)));
				}
			}

			void OGL33Texture::setUnpackStorageHint(const Enums::PixelStorage hint, const int value) {
				ogl::Texture2D::bind();
				switch (hint) {
					case Enums::PixelStorage::ALIGNMENT:
						setPixelStorage(GL_UNPACK_ALIGNMENT, value);
						break;
					case Enums::PixelStorage::ROW_LENGTH:
						setPixelStorage(GL_UNPACK_ROW_LENGTH, value);
						break;
					default:
						MACE__THROW(UnsupportedRenderer, "Specified hint is unavailable for OpenGL: " + std::to_string(static_cast<Byte>(hint)));
				}
			}

			void OGL33Texture::setPackStorageHint(const Enums::PixelStorage hint, const int value) {
				ogl::Texture2D::bind();
				switch (hint) {
					case Enums::PixelStorage::ALIGNMENT:
						setPixelStorage(GL_PACK_ALIGNMENT, value);
						break;
					case Enums::PixelStorage::ROW_LENGTH:
						setPixelStorage(GL_PACK_ROW_LENGTH, value);
						break;
					default:
						MACE__THROW(UnsupportedRenderer, "Specified hint is unavailable for OpenGL: " + std::to_string(static_cast<Byte>(hint)));
				}
			}

			void OGL33Texture::setWrapS(const Enums::WrapMode wrap) {
				ogl::Texture2D::bind();
				if (wrap == Enums::WrapMode::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				} else if (wrap == Enums::WrapMode::REPEAT) {
					setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRRORED_REPEAT) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRROR_CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture: " + std::to_string(static_cast<Byte>(wrap)));
				}
			}

			void OGL33Texture::setWrapT(const Enums::WrapMode wrap) {
				ogl::Texture2D::bind();
				if (wrap == Enums::WrapMode::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				} else if (wrap == Enums::WrapMode::REPEAT) {
					setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRRORED_REPEAT) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				} else if (wrap == Enums::WrapMode::MIRROR_CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture: " + std::to_string(static_cast<Byte>(wrap)));
				}
			}

			void OGL33Texture::setData(const void * data, const Index mipmap) {
				ogl::Texture2D::bind();
				ogl::Texture2D::setData(data, width, height, getType(type), getFormat(format), getInternalFormat(internalFormat), mipmap);
			}

			void OGL33Texture::readPixels(void * data) const {
				ogl::Texture2D::bind();
				ogl::Texture2D::getImage(getFormat(format), getType(type), data);
			}

			void OGL33Texture::setSwizzle(const Enums::SwizzleMode mode, const Enums::SwizzleMode arg) {
				/*
				See the underlying value of Enums::SwizzleMode...
				R = 0
				G = 1
				B = 2
				A = 3

				The reasoning for this is because then we can just cast it and dd it to GL_RED, because GL spec says:
				GL_RED = ...
				GL_GREEN = GL_RED + 1
				GL_BLUE = GL_RED + 2
				GL_ALPHA = GL_RED + 3

				This is the same for GL_TEXTURE_SWIZZLE_R
				*/
				ogl::Texture2D::bind();
				ogl::Texture2D::setParameter(GL_TEXTURE_SWIZZLE_R + static_cast<Enum>(mode), GL_RED + static_cast<Enum>(arg));
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

			void OGL33Model::draw() const {
				for (Index i = 0; i < buffers.size(); ++i) {
					buffers[i].bind();
				}

				using Enums::PrimitiveType;

				Enum type;
				if (primitiveType == PrimitiveType::POINTS) {
					type = GL_POINTS;
				} else if (primitiveType == PrimitiveType::LINES) {
					type = GL_LINES;
				} else if (primitiveType == PrimitiveType::LINES_ADJACENCY) {
					type = GL_LINES_ADJACENCY;
				} else if (primitiveType == PrimitiveType::LINES_STRIP) {
					type = GL_LINE_STRIP;
				} else if (primitiveType == PrimitiveType::LINES_STRIP_ADJACENCY) {
					type = GL_LINE_STRIP_ADJACENCY;
				} else if (primitiveType == PrimitiveType::TRIANGLES) {
					type = GL_TRIANGLES;
				} else if (primitiveType == PrimitiveType::TRIANGLES_ADJACENCY) {
					type = GL_TRIANGLES_ADJACENCY;
				} else if (primitiveType == PrimitiveType::TRIANGLES_STRIP) {
					type = GL_TRIANGLE_STRIP;
				} else if (primitiveType == PrimitiveType::TRIANGLES_STRIP_ADJACENCY) {
					type = GL_TRIANGLE_STRIP_ADJACENCY;
				} else {
					MACE__THROW(UnsupportedRenderer, "Unknown draw mode for OpenGL model: " + std::to_string(static_cast<short int>(primitiveType)));
				}

				if (indices.getIndiceNumber() > 0) {
					indices.bind();

					glDrawElements(type, static_cast<GLsizei>(indices.getIndiceNumber()), GL_UNSIGNED_INT, nullptr);
				} else {
					glDrawArrays(type, 0, getVertexNumber());
				}
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
				renderer = std::unique_ptr<Renderer>(new OGL33Renderer());
			}

			void OGL33Context::onRender(gfx::WindowModule *) {}

			void OGL33Context::onDestroy(gfx::WindowModule *) {
				renderer.reset();
			}

			OGL33Context::OGL33Context(gfx::WindowModule * win) : GraphicsContext(win) {}

			Renderer* OGL33Context::getRenderer() const {
				return renderer.get();
			}

			std::shared_ptr<ModelImpl> OGL33Context::createModelImpl() const {
				return std::unique_ptr<ModelImpl>(new OGL33Model());
			}

			std::shared_ptr<TextureImpl> OGL33Context::createTextureImpl() const {
				return std::unique_ptr<TextureImpl>(new OGL33Texture());
			}
		}//ogl
	}//gfx
}//mc