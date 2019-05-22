/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/OGL33Context.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>
#include <MACE/Graphics/OGL/FreetypeFont.h>

namespace mc {
	namespace gfx {
		namespace ogl33 {
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
					case TextureDesc::InternalFormat::R8:
						return GL_R8;
					case TextureDesc::InternalFormat::R16:
						return GL_R16;
					case TextureDesc::InternalFormat::RGB8:
						return GL_RGB8;
					case TextureDesc::InternalFormat::RGBA8:
						return GL_RGBA8;
					case TextureDesc::InternalFormat::SRGB:
						return GL_SRGB;
					case TextureDesc::InternalFormat::SRGB8:
						return GL_SRGB8;
					case TextureDesc::InternalFormat::SRGB_ALPHA:
						return GL_SRGB_ALPHA;
						default MACE_UNLIKELY:
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

				const std::unordered_map<PrimitiveType, Enum> generatePrimitiveTypeLookup() {
					std::unordered_map<PrimitiveType, Enum> out{};
#define MACE__TABLE_ENTRY(name, value) out[PrimitiveType::name] = value;
					MACE__TABLE_ENTRY(POINTS, GL_POINTS);
					MACE__TABLE_ENTRY(LINES, GL_LINES);
					MACE__TABLE_ENTRY(LINES_ADJACENCY, GL_LINES_ADJACENCY);
					MACE__TABLE_ENTRY(LINES_STRIP, GL_LINE_STRIP);
					MACE__TABLE_ENTRY(LINES_STRIP_ADJACENCY, GL_LINE_STRIP_ADJACENCY);
					MACE__TABLE_ENTRY(TRIANGLES, GL_TRIANGLES);
					MACE__TABLE_ENTRY(TRIANGLES_ADJACENCY, GL_TRIANGLES_ADJACENCY);
					MACE__TABLE_ENTRY(TRIANGLES_STRIP, GL_TRIANGLE_STRIP);
					MACE__TABLE_ENTRY(TRIANGLES_STRIP_ADJACENCY, GL_TRIANGLE_STRIP_ADJACENCY);
#undef MACE__TABLE_ENTRY

					return out;
				}

				inline Enum lookupPrimitiveType(const PrimitiveType type) {
					static auto lookupTable = generatePrimitiveTypeLookup();

					return lookupTable[type];
				}
			}//anon namespace

			OGL33Texture::OGL33Texture(const TextureDesc& desc) : TextureImpl(desc), ogl33::Texture2D() {
				ogl33::Texture2D::init();
				ogl33::Texture2D::bind();

				if (desc.minFilter == TextureDesc::Filter::MIPMAP_LINEAR) {
					ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				} else if (desc.minFilter == TextureDesc::Filter::MIPMAP_NEAREST) {
					ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				} else if (desc.minFilter == TextureDesc::Filter::LINEAR) {
					ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				} else if (desc.minFilter == TextureDesc::Filter::NEAREST) {
					ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
				ogl33::Texture2D::destroy();
			}

			void OGL33Texture::setData(const void* data, const int mipmap, const PixelStorageHints hints) {
				ogl33::Texture2D::bind();

				setPixelStorage(GL_UNPACK_ALIGNMENT, hints.alignment);
				setPixelStorage(GL_UNPACK_ROW_LENGTH, hints.rowLength);

				ogl33::Texture2D::setData(data, desc.width, desc.height, getType(desc.type), getFormat(desc.format), getInternalFormat(desc.internalFormat), mipmap);

				if (desc.minFilter == TextureDesc::Filter::MIPMAP_LINEAR || desc.minFilter == TextureDesc::Filter::MIPMAP_NEAREST) {
					ogl33::Texture2D::generateMipmap();
				}
			}

			void OGL33Texture::readPixels(void* data, const PixelStorageHints hints) const {
				ogl33::Texture2D::bind();

				setPixelStorage(GL_PACK_ALIGNMENT, hints.alignment);
				setPixelStorage(GL_PACK_ROW_LENGTH, hints.rowLength);

				ogl33::Texture2D::getImage(getFormat(desc.format), getType(desc.type), data);
			}

			void OGL33Texture::bindTextureSlot(const TextureSlot slot) const {
				ogl33::Texture2D::bind(static_cast<unsigned int>(slot));
			}

			OGL33Model::OGL33Model() {
				ogl33::VertexArray::init();
			}

			OGL33Model::~OGL33Model() {
				ogl33::VertexArray::destroy();
			}

			void OGL33Model::bind() const {
				ogl33::VertexArray::bind();

				// not sure if we need to rebind all the VBO's in the VAO, or whether just binding the VAO will do

				//for (Index i = 0; i < buffers.size(); ++i) {
				//	buffers[i].bind();
				//}

				//if (indices.getIndiceNumber() > 0) {
				//	indices.bind();
				//}
			}

			void OGL33Model::draw() const {
				bind();

				if (indices.getIndiceNumber() > 0) {
					glDrawElements(lookupPrimitiveType(primitiveType), indices.getIndiceNumber(), GL_UNSIGNED_INT, nullptr);
				} else {
					glDrawArrays(lookupPrimitiveType(primitiveType), 0, getVertexNumber());
				}
			}

			void OGL33Model::loadTextureCoordinates(const unsigned int dataSize, const float* data) {
				bind();

				ogl33::VertexArray::storeDataInAttributeList(dataSize * sizeof(float), data, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION, 2);
			}

			void OGL33Model::loadVertices(const unsigned int verticeSize, const float* vertices) {
				bind();

				ogl33::VertexArray::loadVertices(verticeSize, vertices, MACE__VAO_DEFAULT_VERTICES_LOCATION, 3);
			}

			void OGL33Model::loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
				bind();

				ogl33::VertexArray::loadIndices(indiceNum, indiceData);
			}

			void OGL33Context::onInit(gfx::WindowModule*) {
				freetype.init();

				renderer = std::unique_ptr<Renderer>(new OGL33Renderer());
			}

			void OGL33Context::onRender(gfx::WindowModule*) {}

			void OGL33Context::onDestroy(gfx::WindowModule*) {
				renderer.reset();

				freetype.destroy();
			}

			OGL33Context::OGL33Context(gfx::WindowModule * win) : GraphicsContext(win) {}

			Renderer* OGL33Context::getRenderer() const {
				return renderer.get();
			}

			std::shared_ptr<ModelImpl> OGL33Context::createModelImpl() {
				return std::unique_ptr<ModelImpl>(new OGL33Model());
			}

			std::shared_ptr<TextureImpl> OGL33Context::createTextureImpl(const TextureDesc & desc) {
				return std::unique_ptr<TextureImpl>(new OGL33Texture(desc));
			}
			std::shared_ptr<FontImpl> OGL33Context::createFontImpl(const FontDesc & desc) {
				return std::shared_ptr<FontImpl>(new fty::FreetypeFont(desc, freetype));
			}
		}//ogl33
	}//gfx
}//mc
