/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/OGLContext.h>
#include <MACE/Graphics/OGL/OGLRenderer.h>
#include <MACE/Graphics/OGL/FreetypeFont.h>

namespace mc {
	namespace internal {
		namespace ogl {
			namespace {
				thread_local Context* currentContext = nullptr;

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

			Dispatchable::Dispatchable(std::shared_ptr<Context> con) : context(con) {}

			void Dispatchable::dispatch(const DispatchFunction func) const {
				context->dispatch(func);
			}

			Texture::Texture(std::shared_ptr<Context> con, const gfx::TextureDesc& desc) : TextureImpl(desc), ogl::Texture2D(), Dispatchable(con) {
				dispatch([this, desc]() {
					ogl::Texture2D::init();
					ogl::Texture2D::bind();

					if (desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_LINEAR) {
						ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					} else if (desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_NEAREST) {
						ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					} else if (desc.minFilter == gfx::TextureDesc::Filter::LINEAR) {
						ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					} else if (desc.minFilter == gfx::TextureDesc::Filter::NEAREST) {
						ogl::Texture2D::setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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

					ogl::Texture2D::createStorage(desc.width, desc.height, getType(desc.type), getFormat(desc.format), getInternalFormat(desc.internalFormat), desc.mipmapLevels);
				});
			}

			Texture::~Texture() {
				dispatch([this]() {
					ogl::Texture2D::destroy();
				});
			}

			void Texture::setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const gfx::PixelStorageHints hints) {
				dispatch([this, data, x, y, w, h, mipmap, hints]() {
					setPixelStorage(GL_UNPACK_ALIGNMENT, hints.alignment);
					setPixelStorage(GL_UNPACK_ROW_LENGTH, hints.rowLength);

					ogl::Texture2D::setSubdata(data, x, y, w, h, getType(desc.type), getFormat(desc.format), mipmap);

					if (desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_LINEAR || desc.minFilter == gfx::TextureDesc::Filter::MIPMAP_NEAREST) {
						ogl::Texture2D::generateMipmap();
					}
				});
			}

			void Texture::readPixels(void* data, const gfx::PixelStorageHints hints) const {
				dispatch([this, data, hints]() {
					setPixelStorage(GL_PACK_ALIGNMENT, hints.alignment);
					setPixelStorage(GL_PACK_ROW_LENGTH, hints.rowLength);

					ogl::Texture2D::getImage(getFormat(desc.format), getType(desc.type), data);
				});
			}

			void Texture::bindTextureSlot(const gfx::TextureSlot slot) const {
				dispatch([this, slot]() {
					ogl::Texture2D::bind(static_cast<unsigned int>(slot));
				});
			}

			Model::Model(std::shared_ptr<Context> con) : Dispatchable(con) {
				dispatch([this]() {
					ogl::VertexArray::init();
				});
			}

			Model::~Model() {
				dispatch([this]() {
					ogl::VertexArray::destroy();
				});
			}

			void Model::bind() const {
				dispatch([this]() {
					ogl::VertexArray::bind();
				});
				// not sure if we need to rebind all the VBO's in the VAO, or whether just binding the VAO will do

				//for (Index i = 0; i < buffers.size(); ++i) {
				//	buffers[i].bind();
				//}

				//if (indices.getIndiceNumber() > 0) {
				//	indices.bind();
				//}
			}

			void Model::draw() const {
				dispatch([this]() {
					bind();

					if (indices.getIndiceNumber() > 0) {
						glDrawElements(lookupPrimitiveType(primitiveType), indices.getIndiceNumber(), GL_UNSIGNED_INT, nullptr);
					} else {
						glDrawArrays(lookupPrimitiveType(primitiveType), 0, getVertexNumber());
					}
				});
			}

			void Model::loadTextureCoordinates(const unsigned int dataSize, const float* data) {
				dispatch([this, dataSize, data]() {
					bind();

					ogl::VertexArray::storeDataInAttributeList(dataSize * sizeof(float), data, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION, 2);
				});
			}

			void Model::loadVertices(const unsigned int verticeSize, const float* vertices) {
				dispatch([this, verticeSize, vertices]() {
					bind();

					ogl::VertexArray::loadVertices(verticeSize, vertices, MACE__VAO_DEFAULT_VERTICES_LOCATION, 3);
				});
			}

			void Model::loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
				dispatch([this, indiceNum, indiceData]() {
					bind();

					ogl::VertexArray::loadIndices(indiceNum, indiceData);
				});
			}

			void Context::onInit(gfx::WindowModule*) {
				freetype.init();

				currentContext = this;
			}

			void Context::onRender(gfx::WindowModule*) {
				std::lock_guard<std::mutex> guard(dispatchMutex);
				processDispatchQueue();
			}

			void Context::onDestroy(gfx::WindowModule*) {
				freetype.destroy();

				currentContext = nullptr;
			}

			void Context::dispatch(const DispatchFunction dispatch) {
				MACE_ASSERT(dispatch, "Internal Error: DispatchFunction in OGL Context was empty!");
				if (currentContext == this) {
					processDispatchQueue();
					dispatch();
				} else {
					std::lock_guard<std::mutex> guard(dispatchMutex);
					dispatchQueue.push(dispatch);
				}
			}

			void Context::processDispatchQueue() {
				while (!dispatchQueue.empty()) {
					const DispatchFunction& dispatch = dispatchQueue.front();
					dispatchQueue.pop();
					dispatch();
				}
			}

			Context::Context(gfx::WindowModule* win) : GraphicsContextComponent(win) {}

			std::shared_ptr<gfx::Renderer> Context::getRenderer() {
				if (renderer == nullptr) {
					//this has to be done here and not in the constructor because we need access to shared_from_this()
					renderer = std::shared_ptr<Renderer>(new Renderer(std::static_pointer_cast<Context>(shared_from_this())));
				}
				return renderer;
			}

			std::shared_ptr<gfx::ModelImpl> Context::createModelImpl() {
				return std::shared_ptr<gfx::ModelImpl>(new Model(std::static_pointer_cast<Context>(shared_from_this())));
			}

			std::shared_ptr<gfx::TextureImpl> Context::createTextureImpl(const gfx::TextureDesc& desc) {
				return std::shared_ptr<gfx::TextureImpl>(new Texture(std::static_pointer_cast<Context>(shared_from_this()), desc));
			}
			std::shared_ptr<gfx::FontImpl> Context::createFontImpl(const gfx::FontDesc& desc) {
				return std::shared_ptr<gfx::FontImpl>(new fty::FreetypeFont(desc, freetype));
			}
		}//ogl
	}//internal
}//mc
