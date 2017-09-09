/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/OGL/OGL33Context.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>

//debug
#include <iostream>

namespace mc {
	namespace gfx {
		namespace ogl {
			namespace {
				GLenum getFormat(const TextureDesc::Format format) {
					switch (format) {
						//INTENSITY and LUMINANCE are deprecated since core 3.1, so we implement them with swizzle masks
						case TextureDesc::Format::INTENSITY:
						case TextureDesc::Format::LUMINANCE:
						case TextureDesc::Format::RED:
							return GL_RED;
						case TextureDesc::Format::LUMINANCE_ALPHA:
						case TextureDesc::Format::RG:
							return GL_RG;
						case TextureDesc::Format::RGB:
							return GL_RGB;
						case TextureDesc::Format::RGBA:
							return GL_RGBA;
						case TextureDesc::Format::BGR:
							return GL_BGR;
						case TextureDesc::Format::BGRA:
							return GL_BGRA;
						case TextureDesc::Format::RED_INTEGER:
							return GL_RED_INTEGER;
						case TextureDesc::Format::RG_INTEGER:
							return GL_RG_INTEGER;
						case TextureDesc::Format::RGB_INTEGER:
							return GL_RGB_INTEGER;
						case TextureDesc::Format::BGR_INTEGER:
							return GL_BGR_INTEGER;
						case TextureDesc::Format::RGBA_INTEGER:
							return GL_RGBA_INTEGER;
						case TextureDesc::Format::BGRA_INTEGER:
							return GL_BGRA_INTEGER;
						case TextureDesc::Format::STENCIL:
							return GL_STENCIL;
						case TextureDesc::Format::DEPTH:
							return GL_DEPTH;
						case TextureDesc::Format::DEPTH_STENCIL:
							return GL_DEPTH_STENCIL;
						default:
							MACE__THROW(BadFormat, "Unsupported format by OpenGL");
					}
				}

				GLenum getInternalFormat(const TextureDesc::InternalFormat format) {
					switch (format) {
						case TextureDesc::InternalFormat::DEPTH:
							return GL_DEPTH_COMPONENT;
						case TextureDesc::InternalFormat::DEPTH_STENCIL:
							return GL_DEPTH_STENCIL;
						case TextureDesc::InternalFormat::RED:
							return GL_RED;
						case TextureDesc::InternalFormat::RG:
							return GL_RG;
						case TextureDesc::InternalFormat::RGB:
							return GL_RGB;
						case TextureDesc::InternalFormat::RGBA:
							return GL_RGBA;
						default:
							MACE__THROW(BadFormat, "Unsupported internal format by OpenGL");
					}
				}

				GLenum getType(const TextureDesc::Type type) {
					switch (type) {
						case TextureDesc::Type::UNSIGNED_BYTE:
							return GL_UNSIGNED_BYTE;
						case TextureDesc::Type::BYTE:
							return GL_BYTE;
						case TextureDesc::Type::UNSIGNED_SHORT:
							return GL_UNSIGNED_SHORT;
						case TextureDesc::Type::SHORT:
							return GL_SHORT;
						case TextureDesc::Type::UNSIGNED_INT:
							return GL_UNSIGNED_INT;
						case TextureDesc::Type::INT:
							return GL_INT;
						case TextureDesc::Type::FLOAT:
							return GL_FLOAT;
						case TextureDesc::Type::UNSIGNED_BYTE_3_3_2:
							return GL_UNSIGNED_BYTE_3_3_2;
						case TextureDesc::Type::UNSIGNED_BYTE_2_3_3_REV:
							return GL_UNSIGNED_BYTE_2_3_3_REV;
						case TextureDesc::Type::UNSIGNED_SHORT_5_6_5:
							return GL_UNSIGNED_SHORT_5_6_5;
						case TextureDesc::Type::UNSIGNED_SHORT_5_6_5_REV:
							return GL_UNSIGNED_SHORT_5_6_5_REV;
						case TextureDesc::Type::UNSIGNED_SHORT_4_4_4_4:
							return GL_UNSIGNED_SHORT_4_4_4_4;
						case TextureDesc::Type::UNSIGNED_SHORT_4_4_4_4_REV:
							return GL_UNSIGNED_SHORT_4_4_4_4_REV;
						case TextureDesc::Type::UNSIGNED_SHORT_5_5_5_1:
							return GL_UNSIGNED_SHORT_5_5_5_1;
						case TextureDesc::Type::UNSIGNED_SHORT_1_5_5_5_REV:
							return GL_UNSIGNED_SHORT_1_5_5_5_REV;
						case TextureDesc::Type::UNSIGNED_INT_8_8_8_8:
							return GL_UNSIGNED_INT_8_8_8_8;
						case TextureDesc::Type::UNSIGNED_INT_8_8_8_8_REV:
							return GL_UNSIGNED_INT_8_8_8_8_REV;
						case TextureDesc::Type::UNSIGNED_INT_10_10_10_2:
							return GL_UNSIGNED_INT_10_10_10_2;
						case TextureDesc::Type::UNSIGNED_INT_2_10_10_10_REV:
							return GL_UNSIGNED_INT_2_10_10_10_REV;
						default:
							MACE__THROW(BadFormat, "Unsupported type by OpenGL");
					}
				}
			}//anon namespace

			OGL33Texture::OGL33Texture(const TextureDesc& desc) : Texture2DImpl(desc), ogl::Texture2D() {
				ogl::Texture2D::init();
				ogl::Texture2D::bind();

				if (desc.minFilter == TextureDesc::Filter::MIPMAP_LINEAR) {
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				} else if (desc.minFilter == TextureDesc::Filter::MIPMAP_NEAREST) {
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else if (desc.minFilter == TextureDesc::Filter::LINEAR) {
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				} else if (desc.minFilter == TextureDesc::Filter::NEAREST) {
					ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else {
					MACE__THROW(UnsupportedRenderer, "Unsupported ResizeFilter for OpenGL: " + std::to_string(static_cast<Byte>(desc.minFilter)));
				}
				if (desc.magFilter == TextureDesc::Filter::MIPMAP_LINEAR ||
					desc.magFilter == TextureDesc::Filter::MIPMAP_NEAREST) {
					MACE__THROW(UnsupportedRenderer, "Mipmap resize filtering can't be used as a magnification filter with OpenGL");
				} else if (desc.magFilter == TextureDesc::Filter::LINEAR) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				} else if (desc.magFilter == TextureDesc::Filter::NEAREST) {
					setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				} else {
					MACE__THROW(UnsupportedRenderer, "Unsupported ResizeFilter for OpenGL: " + std::to_string(static_cast<Byte>(desc.magFilter)));
				}
				
				if (desc.wrapS == TextureDesc::Wrap::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				} else if (desc.wrapS == TextureDesc::Wrap::WRAP) {
					setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
				} else if (desc.wrapS == TextureDesc::Wrap::MIRROR) {
					setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				} else if (desc.wrapS == TextureDesc::Wrap::BORDER) {
					setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture: " + std::to_string(static_cast<Byte>(desc.wrapS)));
				}

				if (desc.wrapT == TextureDesc::Wrap::CLAMP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				} else if (desc.wrapT == TextureDesc::Wrap::WRAP) {
					setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
				} else if (desc.wrapT == TextureDesc::Wrap::MIRROR) {
					setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				} else if (desc.wrapT == TextureDesc::Wrap::BORDER) {
					setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, desc.borderColor.begin());
				} else {
					MACE__THROW(BadFormat, "Unknown wrap mode for OpenGL texture: " + std::to_string(static_cast<Byte>(desc.wrapT)));
				}

				/*
				switch (desc.format) {
					case TextureDesc::Format::INTENSITY:
						setParameter(GL_TEXTURE_SWIZZLE_A, GL_RED);
						MACE_FALLTHROUGH;
					case TextureDesc::Format::LUMINANCE:
						setParameter(GL_TEXTURE_SWIZZLE_G, GL_RED);
						setParameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
						break;
					case TextureDesc::Format::LUMINANCE_ALPHA:
						setParameter(GL_TEXTURE_SWIZZLE_G, GL_RED);
						setParameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
						setParameter(GL_TEXTURE_SWIZZLE_A, GL_GREEN);
						break;
					default:
						break;
				}
				*/
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, desc.borderColor.begin());
			}

			OGL33Texture::~OGL33Texture() {
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

			void OGL33Texture::setData(const void * data, const Index mipmap) {
				ogl::Texture2D::bind();
				ogl::Texture2D::setData(data, desc.width, desc.height, getType(desc.type), getFormat(desc.format), getInternalFormat(desc.internalFormat), mipmap);

				if (desc.minFilter == TextureDesc::Filter::MIPMAP_LINEAR || desc.minFilter == TextureDesc::Filter::MIPMAP_NEAREST) {
					//ogl::Texture2D::generateMipmap();
				}
			}

			void OGL33Texture::readPixels(void * data) const {
				ogl::Texture2D::bind();
				ogl::Texture2D::getImage(getFormat(desc.format), getType(desc.type), data);
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

			std::shared_ptr<Texture2DImpl> OGL33Context::createTextureImpl(const TextureDesc& desc) const {
				return std::unique_ptr<Texture2DImpl>(new OGL33Texture(desc));
			}
		}//ogl
	}//gfx
}//mc
