/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/OGL33Context.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>
#include <MACE/Graphics/OGL/FreetypeFont.h>

namespace mc {
	namespace internal {
		namespace ogl33 {
			namespace {
				thread_local OGL33Context* currentContext = nullptr;

				GLenum getFormat(const gfx::TextureDesc::Format format) {
					switch (format) {
						//INTENSITY and LUMINANCE are deprecated since core 3.1, so we implement them with swizzle masks
					case gfx::TextureDesc::Format::INTENSITY:
					case gfx::TextureDesc::Format::LUMINANCE:
					case gfx::TextureDesc::Format::RED:
						return GL_RED;
					case gfx::TextureDesc::Format::LUMINANCE_ALPHA:
					case gfx::TextureDesc::Format::RG:
						return GL_RG;
					case gfx::TextureDesc::Format::RGB:
						return GL_RGB;
					case gfx::TextureDesc::Format::RGBA:
						return GL_RGBA;
					case gfx::TextureDesc::Format::BGR:
						return GL_BGR;
					case gfx::TextureDesc::Format::BGRA:
						return GL_BGRA;
					case gfx::TextureDesc::Format::RED_INTEGER:
						return GL_RED_INTEGER;
					case gfx::TextureDesc::Format::RG_INTEGER:
						return GL_RG_INTEGER;
					case gfx::TextureDesc::Format::RGB_INTEGER:
						return GL_RGB_INTEGER;
					case gfx::TextureDesc::Format::BGR_INTEGER:
						return GL_BGR_INTEGER;
					case gfx::TextureDesc::Format::RGBA_INTEGER:
						return GL_RGBA_INTEGER;
					case gfx::TextureDesc::Format::BGRA_INTEGER:
						return GL_BGRA_INTEGER;
					case gfx::TextureDesc::Format::STENCIL:
						return GL_STENCIL;
					case gfx::TextureDesc::Format::DEPTH:
						return GL_DEPTH;
					case gfx::TextureDesc::Format::DEPTH_STENCIL:
						return GL_DEPTH_STENCIL;
					default:
						MACE__THROW(gfx::BadFormat, "Unsupported format by OpenGL");
					}
				}

				GLenum getInternalFormat(const gfx::TextureDesc::InternalFormat format) {
					switch (format) {
					case gfx::TextureDesc::InternalFormat::DEPTH24:
						return GL_DEPTH_COMPONENT24;
					case gfx::TextureDesc::InternalFormat::DEPTH24_STENCIL8:
						return GL_DEPTH24_STENCIL8;
					case gfx::TextureDesc::InternalFormat::R8:
						return GL_R8;
					case gfx::TextureDesc::InternalFormat::R16:
						return GL_R16;
					case gfx::TextureDesc::InternalFormat::RG8:
						return GL_RG8;
					case gfx::TextureDesc::InternalFormat::RG16:
						return GL_RG16;
					case gfx::TextureDesc::InternalFormat::RGB8:
						return GL_RGB8;
					case gfx::TextureDesc::InternalFormat::RGBA8:
						return GL_RGBA8;
					case gfx::TextureDesc::InternalFormat::SRGB8:
						return GL_SRGB8;
					case gfx::TextureDesc::InternalFormat::SRGB8_ALPHA8:
						return GL_SRGB8_ALPHA8;
					MACE_UNLIKELY default:
						MACE__THROW(gfx::BadFormat, "Unsupported internal format by OpenGL");
					}
				}

				GLenum getType(const gfx::TextureDesc::Type type) {
					switch (type) {
					case gfx::TextureDesc::Type::UNSIGNED_BYTE:
						return GL_UNSIGNED_BYTE;
					case gfx::TextureDesc::Type::BYTE:
						return GL_BYTE;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT:
						return GL_UNSIGNED_SHORT;
					case gfx::TextureDesc::Type::SHORT:
						return GL_SHORT;
					case gfx::TextureDesc::Type::UNSIGNED_INT:
						return GL_UNSIGNED_INT;
					case gfx::TextureDesc::Type::INT:
						return GL_INT;
					case gfx::TextureDesc::Type::FLOAT:
						return GL_FLOAT;
					case gfx::TextureDesc::Type::UNSIGNED_BYTE_3_3_2:
						return GL_UNSIGNED_BYTE_3_3_2;
					case gfx::TextureDesc::Type::UNSIGNED_BYTE_2_3_3_REV:
						return GL_UNSIGNED_BYTE_2_3_3_REV;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT_5_6_5:
						return GL_UNSIGNED_SHORT_5_6_5;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT_5_6_5_REV:
						return GL_UNSIGNED_SHORT_5_6_5_REV;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT_4_4_4_4:
						return GL_UNSIGNED_SHORT_4_4_4_4;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT_4_4_4_4_REV:
						return GL_UNSIGNED_SHORT_4_4_4_4_REV;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT_5_5_5_1:
						return GL_UNSIGNED_SHORT_5_5_5_1;
					case gfx::TextureDesc::Type::UNSIGNED_SHORT_1_5_5_5_REV:
						return GL_UNSIGNED_SHORT_1_5_5_5_REV;
					case gfx::TextureDesc::Type::UNSIGNED_INT_8_8_8_8:
						return GL_UNSIGNED_INT_8_8_8_8;
					case gfx::TextureDesc::Type::UNSIGNED_INT_8_8_8_8_REV:
						return GL_UNSIGNED_INT_8_8_8_8_REV;
					case gfx::TextureDesc::Type::UNSIGNED_INT_10_10_10_2:
						return GL_UNSIGNED_INT_10_10_10_2;
					case gfx::TextureDesc::Type::UNSIGNED_INT_2_10_10_10_REV:
						return GL_UNSIGNED_INT_2_10_10_10_REV;
					MACE_UNLIKELY default:
						MACE__THROW(gfx::BadFormat, "Unsupported type by OpenGL");
					}
				}

				const std::unordered_map<gfx::PrimitiveType, Enum> generatePrimitiveTypeLookup() {
					std::unordered_map<gfx::PrimitiveType, Enum> out{};
#define MACE__TABLE_ENTRY(name, value) out[gfx::PrimitiveType::name] = value;
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

				inline Enum lookupPrimitiveType(const gfx::PrimitiveType type) {
					static auto lookupTable = generatePrimitiveTypeLookup();

					return lookupTable[type];
				}
			}//anon namespace

			OGL33Texture::OGL33Texture(OGL33Context* const con, const gfx::TextureDesc& desc) : TextureImpl(desc), ogl33::Texture2D(), context(con) {
				context->dispatch([this, desc]() {
					ogl33::Texture2D::init();
					ogl33::Texture2D::bind();

					if (desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_LINEAR) {
						ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					} else if (desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_NEAREST) {
						ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					} else if (desc.minFilter == gfx::TextureDesc::Filter::LINEAR) {
						ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					} else if (desc.minFilter == gfx::TextureDesc::Filter::NEAREST) {
						ogl33::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					} else {
						MACE__THROW(gfx::UnsupportedRenderer, "Unsupported ResizeFilter for OpenGL: " + std::to_string(static_cast<Byte>(desc.minFilter)));
					}
					if (desc.magFilter == gfx::TextureDesc::Filter::MIPMAP_LINEAR ||
						desc.magFilter == gfx::TextureDesc::Filter::MIPMAP_NEAREST) {
						MACE__THROW(gfx::UnsupportedRenderer, "Mipmap resize filtering can't be used as a magnification filter with OpenGL");
					} else if (desc.magFilter == gfx::TextureDesc::Filter::LINEAR) {
						setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					} else if (desc.magFilter == gfx::TextureDesc::Filter::NEAREST) {
						setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					} else {
						MACE__THROW(gfx::UnsupportedRenderer, "Unsupported ResizeFilter for OpenGL: " + std::to_string(static_cast<Byte>(desc.magFilter)));
					}

					if (desc.wrapS == gfx::TextureDesc::Wrap::CLAMP) {
						setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					} else if (desc.wrapS == gfx::TextureDesc::Wrap::WRAP) {
						setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
					} else if (desc.wrapS == gfx::TextureDesc::Wrap::MIRROR) {
						setParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
					} else if (desc.wrapS == gfx::TextureDesc::Wrap::BORDER) {
						setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					} else {
						MACE__THROW(gfx::BadFormat, "Unknown wrap mode for OpenGL texture: " + std::to_string(static_cast<Byte>(desc.wrapS)));
					}

					if (desc.wrapT == gfx::TextureDesc::Wrap::CLAMP) {
						setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					} else if (desc.wrapT == gfx::TextureDesc::Wrap::WRAP) {
						setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
					} else if (desc.wrapT == gfx::TextureDesc::Wrap::MIRROR) {
						setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
					} else if (desc.wrapT == gfx::TextureDesc::Wrap::BORDER) {
						setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
						glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, desc.borderColor.begin());
					} else {
						MACE__THROW(gfx::BadFormat, "Unknown wrap mode for OpenGL texture: " + std::to_string(static_cast<Byte>(desc.wrapT)));
					}

					/*
					switch (desc.format) {
						case gfx::TextureDesc::Format::INTENSITY:
							setParameter(GL_TEXTURE_SWIZZLE_A, GL_RED);
							MACE_FALLTHROUGH;
						case gfx::TextureDesc::Format::LUMINANCE:
							setParameter(GL_TEXTURE_SWIZZLE_G, GL_RED);
							setParameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
							break;
						case gfx::TextureDesc::Format::LUMINANCE_ALPHA:
							setParameter(GL_TEXTURE_SWIZZLE_G, GL_RED);
							setParameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
							setParameter(GL_TEXTURE_SWIZZLE_A, GL_GREEN);
							break;
						default:
							break;
					}
					*/
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, desc.borderColor.begin());

					ogl33::Texture2D::createStorage(desc.width, desc.height, getType(desc.type), getFormat(desc.format), getInternalFormat(desc.internalFormat), desc.mipmapLevels);
				});
			}

			OGL33Texture::~OGL33Texture() {
				context->dispatch([this]() {
					ogl33::Texture2D::destroy();
				});
			}

			void OGL33Texture::setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const gfx::PixelStorageHints hints) {
				context->dispatch([this, data, x, y, w, h, mipmap, hints]() {
					setPixelStorage(GL_UNPACK_ALIGNMENT, hints.alignment);
					setPixelStorage(GL_UNPACK_ROW_LENGTH, hints.rowLength);

					ogl33::Texture2D::setSubdata(data, x, y, w, h, getType(desc.type), getFormat(desc.format), mipmap);

					if (desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_LINEAR || desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_NEAREST) {
						ogl33::Texture2D::generateMipmap();
					}
				});
			}

			void OGL33Texture::readPixels(void* data, const gfx::PixelStorageHints hints) const {
				context->dispatch([this, data, hints]() {
					setPixelStorage(GL_PACK_ALIGNMENT, hints.alignment);
					setPixelStorage(GL_PACK_ROW_LENGTH, hints.rowLength);

					ogl33::Texture2D::getImage(getFormat(desc.format), getType(desc.type), data);
				});
			}

			void OGL33Texture::bindTextureSlot(const gfx::TextureSlot slot) const {
				context->dispatch([this, slot]() {
					ogl33::Texture2D::bind(static_cast<unsigned int>(slot));
				});
			}

			OGL33Model::OGL33Model(OGL33Context* const con) : context(con) {
				context->dispatch([this]() {
					ogl33::VertexArray::init();
				});
			}

			OGL33Model::~OGL33Model() {
				context->dispatch([this]() {
					ogl33::VertexArray::destroy();
				});
			}

			void OGL33Model::bind() const {
				context->dispatch([this]() {
					ogl33::VertexArray::bind();
				});
				// not sure if we need to rebind all the VBO's in the VAO, or whether just binding the VAO will do

				//for (Index i = 0; i < buffers.size(); ++i) {
				//	buffers[i].bind();
				//}

				//if (indices.getIndiceNumber() > 0) {
				//	indices.bind();
				//}
			}

			void OGL33Model::draw() const {
				context->dispatch([this]() {
					bind();

					if (indices.getIndiceNumber() > 0) {
						glDrawElements(lookupPrimitiveType(primitiveType), indices.getIndiceNumber(), GL_UNSIGNED_INT, nullptr);
					} else {
						glDrawArrays(lookupPrimitiveType(primitiveType), 0, getVertexNumber());
					}
				});
			}

			void OGL33Model::loadTextureCoordinates(const unsigned int dataSize, const float* data) {
				context->dispatch([this, dataSize, data]() {
					bind();

					ogl33::VertexArray::storeDataInAttributeList(dataSize * sizeof(float), data, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION, 2);
				});
			}

			void OGL33Model::loadVertices(const unsigned int verticeSize, const float* vertices) {
				context->dispatch([this, verticeSize, vertices]() {
					bind();

					ogl33::VertexArray::loadVertices(verticeSize, vertices, MACE__VAO_DEFAULT_VERTICES_LOCATION, 3);
				});
			}

			void OGL33Model::loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
				context->dispatch([this, indiceNum, indiceData]() {
					bind();

					ogl33::VertexArray::loadIndices(indiceNum, indiceData);
				});
			}

			void OGL33Context::onInit(gfx::WindowModule*) {
				freetype.init();

				renderer = std::unique_ptr<OGL33Renderer>(new OGL33Renderer());

				currentContext = this;
			}

			void OGL33Context::onRender(gfx::WindowModule*) {
				std::lock_guard<std::mutex> guard(dispatchMutex);
				processDispatchQueue();
			}

			void OGL33Context::onDestroy(gfx::WindowModule*) {
				renderer.reset();

				freetype.destroy();

				currentContext = nullptr;
			}

			void OGL33Context::dispatch(const OGL33Dispatch& dispatch) {
				if (currentContext == this) {
					processDispatchQueue();
					dispatch();
				} else {
					std::lock_guard<std::mutex> guard(dispatchMutex);
					dispatchQueue.push(dispatch);
				}
			}

			void OGL33Context::processDispatchQueue() {
				while (!dispatchQueue.empty()) {
					OGL33Dispatch& dispatch = dispatchQueue.front();
					dispatchQueue.pop();
					dispatch();
				}
			}

			OGL33Context::OGL33Context(gfx::WindowModule* win) : GraphicsContextComponent(win) {}

			std::shared_ptr<gfx::Renderer> OGL33Context::getRenderer() const {
				return renderer;
			}

			std::shared_ptr<gfx::ModelImpl> OGL33Context::createModelImpl() {
				return std::shared_ptr<gfx::ModelImpl>(new OGL33Model(this));
			}

			std::shared_ptr<gfx::TextureImpl> OGL33Context::createTextureImpl(const gfx::TextureDesc& desc) {
				return std::shared_ptr<gfx::TextureImpl>(new OGL33Texture(this, desc));
			}
			std::shared_ptr<gfx::FontImpl> OGL33Context::createFontImpl(const gfx::FontDesc& desc) {
				return std::shared_ptr<gfx::FontImpl>(new fty::FreetypeFont(desc, freetype));
			}
		}//ogl33
	}//internal
}//mc
