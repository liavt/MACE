/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/OGLContext.h>
#include <MACE/Graphics/OGL/OGLModel.h>
#include <MACE/Graphics/OGL/OGLTexture.h>
#include <MACE/Graphics/OGL/OGLRenderTarget.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/RenderTarget.h>
#include <MACE/Core/System.h>

#include <GLFW/glfw3.h>

//output error messages to console
#include <sstream>

#ifdef MACE_DEBUG
#	include <iostream>
#endif

namespace mc {
	namespace internal {
		namespace ogl {
			namespace {
				thread_local Context* currentContext = nullptr;

				Shader createShader(const Enum type, const gfx::Painter::RenderFeatures features, const char* source) {
					Shader s = Shader(type);
					s.init();
#define MACE__SHADER_MACRO(name, def) "#define " #name " " MACE_STRINGIFY_DEFINITION(def) "\n"
					std::vector<const char*> sources = std::vector<const char*>({
						MACE__SHADER_MACRO(MACE_ENTITY_DATA_LOCATION, MACE__ENTITY_DATA_LOCATION),
						MACE__SHADER_MACRO(MACE_ENTITY_DATA_NAME, MACE__ENTITY_DATA_NAME),
						MACE__SHADER_MACRO(MACE_PAINTER_DATA_LOCATION, MACE__PAINTER_DATA_LOCATION),
						MACE__SHADER_MACRO(MACE_PAINTER_DATA_NAME, MACE__PAINTER_DATA_NAME),
						MACE__SHADER_MACRO(MACE_SCENE_ATTACHMENT_INDEX, MACE__SCENE_ATTACHMENT_INDEX),
						MACE__SHADER_MACRO(MACE_ID_ATTACHMENT_INDEX, MACE__ID_ATTACHMENT_INDEX),
						MACE__SHADER_MACRO(MACE_DATA_ATTACHMENT_INDEX, MACE__DATA_ATTACHMENT_INDEX),
						MACE__SHADER_MACRO(MACE_VAO_DEFAULT_VERTICES_LOCATION, MACE__VAO_DEFAULT_VERTICES_LOCATION),
						MACE__SHADER_MACRO(MACE_VAO_DEFAULT_TEXTURE_COORD_LOCATION, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION),
#include <MACE/Graphics/OGL/Shaders/Shared.glsl>
																				});
#undef MACE__SHADER_MACRO

#define MACE__SHADER_RENDER_FEATURE(name) if(MACE__HAS_RENDER_FEATURE(features, name)){ sources.insert(sources.begin(), "#define MACE_" MACE_STRINGIFY(name) " 1\n"); }
					MACE__SHADER_RENDER_FEATURE(DISCARD_INVISIBLE);
					MACE__SHADER_RENDER_FEATURE(FILTER);
					MACE__SHADER_RENDER_FEATURE(TEXTURE);
					MACE__SHADER_RENDER_FEATURE(TEXTURE_TRANSFORM);
					MACE__SHADER_RENDER_FEATURE(STORE_ID);
#undef MACE__SHADER_RENDER_FEATURE

					sources.insert(sources.begin(), "#version 330 core\n");

					if (type == GL_VERTEX_SHADER) {
						sources.push_back(
#include <MACE/Graphics/OGL/Shaders/Vert.glsl>
						);
					} else if (type == GL_FRAGMENT_SHADER) {
						sources.push_back(
#include <MACE/Graphics/OGL/Shaders/Frag.glsl>
						);
					}
#ifdef MACE_DEBUG
					else MACE_UNLIKELY{
						MACE__THROW(gfx::BadFormat, "Internal Error: Unknown GLSL shader type: " + std::to_string(type));
					}
#endif
					sources.push_back(source);
					s.setSource(static_cast<const GLsizei>(sources.size()), sources.data(), nullptr);
					s.compile();
					return s;
				}

				void initializeRenderProtocolForSettings(Context::RenderProtocol& prot, const std::pair<gfx::Painter::Brush, gfx::Painter::RenderFeatures>& settings) {
					ogl::ShaderProgram program;

					program.init();

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error initializing ShaderProgram");

					program.attachShader(createShader(GL_VERTEX_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/RenderTypes/standard.v.glsl>
					));

					if (settings.first == gfx::Painter::Brush::COLOR) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/color.f.glsl>
						));

						program.link();
					} else if (settings.first == gfx::Painter::Brush::TEXTURE) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/texture.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex");

						program.setUniform("tex", static_cast<int>(gfx::PainterTextureSlots::FOREGROUND));
					} else if (settings.first == gfx::Painter::Brush::MASK) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/mask.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex");
						program.createUniform("mask");

						//binding the samplers
						program.setUniform("tex", static_cast<int>(gfx::PainterTextureSlots::FOREGROUND));
						program.setUniform("mask", static_cast<int>(gfx::PainterTextureSlots::MASK));
					} else if (settings.first == gfx::Painter::Brush::MASK) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/mask.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex");
						program.createUniform("mask");

						//binding the samplers
						program.setUniform("tex", static_cast<int>(gfx::PainterTextureSlots::FOREGROUND));
						program.setUniform("mask", static_cast<int>(gfx::PainterTextureSlots::MASK));
					} else if (settings.first == gfx::Painter::Brush::BLEND) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/blend.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex1");
						program.createUniform("tex2");

						program.setUniform("tex1", static_cast<int>(gfx::PainterTextureSlots::FOREGROUND));
						program.setUniform("tex2", static_cast<int>(gfx::PainterTextureSlots::BACKGROUND));
					} else if (settings.first == gfx::Painter::Brush::CONDITIONAL_MASK) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/conditional_mask.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex1");
						program.createUniform("tex2");
						program.createUniform("mask");

						//binding the samplers
						program.setUniform("tex1", static_cast<int>(gfx::PainterTextureSlots::FOREGROUND));
						program.setUniform("tex2", static_cast<int>(gfx::PainterTextureSlots::BACKGROUND));
						program.setUniform("mask", static_cast<int>(gfx::PainterTextureSlots::MASK));
					} else if (settings.first == gfx::Painter::Brush::MULTICOMPONENT_BLEND) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/multicomponent_blend.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex1");
						program.createUniform("tex2");

						program.setUniform("tex1", static_cast<int>(gfx::PainterTextureSlots::FOREGROUND));
						program.setUniform("tex2", static_cast<int>(gfx::PainterTextureSlots::BACKGROUND));

						prot.sourceBlend = GL_SRC1_COLOR;
						prot.destBlend = GL_ONE_MINUS_SRC1_COLOR;

						prot.multitarget = false;
					} else MACE_UNLIKELY{
						MACE__THROW(gfx::BadFormat, "OpenGL 3.3 Renderer: Unsupported brush type: " + std::to_string(static_cast<unsigned int>(settings.first)));
					}

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating shader program for RenderProtocol");

					program.bind();

					program.createUniformBuffer(MACE_STRINGIFY_DEFINITION(MACE__ENTITY_DATA_NAME), MACE__ENTITY_DATA_LOCATION);
					program.createUniformBuffer(MACE_STRINGIFY_DEFINITION(MACE__PAINTER_DATA_NAME), MACE__PAINTER_DATA_LOCATION);

					prot.program = program;

					prot.created = true;

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating uniform buffer for RenderProtocol ShaderProgram");
				}

				Context::ProtocolHash hashSettings(const std::pair<gfx::Painter::Brush, gfx::Painter::RenderFeatures>& settings) {
					Context::ProtocolHash j;
					/*
					endianness doesn't really matter, since the only purpose for this hash is
					for unordered_map. so as long as it's consisent at runtime, its good.
					*/
					j = static_cast<Context::ProtocolHash>(settings.first);
					j <<= 8;
					j |= static_cast<Context::ProtocolHash>(settings.second);
					return j;
				}

				std::unordered_map<gfx::FrameBufferTarget, const Enum*> generateFramebufferTargetLookup() {
					static MACE_CONSTEXPR const Enum colorBuffers[] = {
								GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
								GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
					};

					static MACE_CONSTEXPR const Enum dataBuffers[] = {
							GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX,
							GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
					};

					std::unordered_map<gfx::FrameBufferTarget, const Enum*> out{};
					out[gfx::FrameBufferTarget::COLOR] = colorBuffers;
					out[gfx::FrameBufferTarget::DATA] = dataBuffers;
					return out;
				}

				const Enum* lookupFramebufferTarget(const gfx::FrameBufferTarget target) {
					static auto lookupTable = generateFramebufferTargetLookup();

					return lookupTable[target];
				}
			}//anon namespace

			void Context::onInit(gfx::WindowModule*) {
				freetype.init();

				currentContext = this;

				const int version = gladLoadGL(glfwGetProcAddress);
				if (version == 0) {
					std::ostringstream errorMessage;
					errorMessage << "This system (OpenGL " << glGetString(GL_VERSION) << ")";
					errorMessage << " does not support the required OpenGL version required by this renderer, ";
					errorMessage << "OpenGL 3.3";
					MACE__THROW(gfx::UnsupportedRenderer, errorMessage.str());
				}

#ifdef MACE_DEBUG
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "OpenGL Info:\n";
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Version:\n\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_VERSION) << '\n';
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Vendor:\n\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_VENDOR) << '\n';
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Renderer:\n\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_RENDERER) << '\n';
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Shader version:\n\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Supported extensions:\n";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN);
#define MACE__DEBUG_OUTPUT_EXTENSION(ext) if(GLAD_##ext){std::cout << "\t* " << #ext << '\n';}
				MACE__DEBUG_OUTPUT_EXTENSION(GL_ARB_buffer_storage);
				MACE__DEBUG_OUTPUT_EXTENSION(GL_ARB_texture_storage);
				MACE__DEBUG_OUTPUT_EXTENSION(GL_ARB_invalidate_subdata);
				MACE__DEBUG_OUTPUT_EXTENSION(GL_ARB_multi_bind);
				MACE__DEBUG_OUTPUT_EXTENSION(GL_ARB_direct_state_access);
				MACE__DEBUG_OUTPUT_EXTENSION(GL_EXT_direct_state_access);
				MACE__DEBUG_OUTPUT_EXTENSION(GL_KHR_debug);
#undef MACE__DEBUG_OUTPUT_EXTENSION
#endif//MACE_DEBUG

				if (GLAD_GL_KHR_debug) {
					std::cout << "Advanced OpenGL debug output enabled.\n";
					glEnable(GL_DEBUG_OUTPUT);
					// we are only printing to std::cout, so we don't need to hamper performance by doing synchronous callback
					glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
					glDebugMessageCallback(static_cast<GLDEBUGPROC>(debugMessageCallback), nullptr);
				}

#ifdef GLAD_OPTION_GL_DEBUG
#ifdef MACE_DEBUG
				gladSetGLPreCallback(preGladCallback);
				gladSetGLPostCallback(postGladCallback);
#else
				gladSetGLPreCallback([](CString, GLADapiproc, int, ...) {
					//do nothing
				});
				// do nothing
				gladSetGLPostCallback([](void*, CString, GLADapiproc, int, ...) {
					//ogl33::checkGLError(__LINE__, __FILE__, name);
				});
#endif//MACE_DEBUG
#endif//GLAD_DEBUG
				std::cout << os::consoleColor() << std::flush;

				ogl::enable(GL_BLEND);
				ogl::enable(GL_MULTISAMPLE);

				checkGLError(__LINE__, __FILE__, "Internal Error: An error occured creating the Context");

				eventManager.addListener<gfx::PreRenderEvent>(parent, [this](auto win) {
					preRender(win);
				});
			}


			void Context::bindProtocol(Painter* painter, const std::pair<gfx::Painter::Brush, gfx::Painter::RenderFeatures> settings) {
				const Context::ProtocolHash hash = hashSettings(settings);
				//its a pointer so that we dont do a copy operation on assignment here
				RenderProtocol& protocol = protocols[hash];

				if (!protocol.created) MACE_UNLIKELY{
					initializeRenderProtocolForSettings(protocol, settings);
				}

				MACE_ASSERT(protocol.created, "Internal Error: initalizeRenderProtocolForSettings() did not properly create the requested RenderProtocol");

				const RenderProtocol& oldProtocol = protocols[currentProtocol];

				//if there hasn't been a protocol bound yet, bind everything to default values
				if (!oldProtocol.created || hash != currentProtocol) {
					currentProtocol = hash;

					if (oldProtocol.sourceBlend != protocol.sourceBlend || oldProtocol.destBlend != protocol.destBlend) {
						ogl::setBlending(protocol.sourceBlend, protocol.destBlend);
					}

					if (oldProtocol.multitarget != protocol.multitarget) {
						FrameBuffer::setDrawBuffers(protocols[currentProtocol].multitarget ? 2 : 1, lookupFramebufferTarget(currentTarget));
					}

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error swapping RenderProtocol");
				}

				if (oldProtocol.program.getID() != protocol.program.getID()) {
					protocol.program.bind();
				}

				const UniformBuffer* buffers[] = {
					&painter->uniformBuffers.entityData,
					&painter->uniformBuffers.painterData
				};

				protocol.program.bindUniformBuffers(buffers, os::getArraySize(buffers));

				checkGLError(__LINE__, __FILE__, "Internal Error: An error occured binding the RenderProtocol");
			}

			void Context::setTarget(const gfx::FrameBufferTarget& target) {
				if (target != currentTarget) {
					currentTarget = target;
				}
			}

			void Context::onRender(gfx::WindowModule*) {
				std::lock_guard<std::mutex> guard(dispatchMutex);
				processDispatchQueue();
				checkGLError(__LINE__, __FILE__, "Internal Error: An error occured during onRender()");
			}

			void Context::onDestroy(gfx::WindowModule*) {
				eventManager.destroy();

				freetype.destroy();

				for (auto iter = protocols.begin(); iter != protocols.end(); ++iter) {
					iter->second.program.destroy();
				}

				protocols.clear();

				checkGLError(__LINE__, __FILE__, "Internal Error: Error destroying Context");

				currentContext = nullptr;
			}

			void Context::preRender(gfx::WindowModule*) {
				RenderProtocol& proto = protocols[currentProtocol];
				if (proto.created) {
					ogl::setBlending(proto.sourceBlend, proto.destBlend);
				}

				checkGLError(__LINE__, __FILE__, "Internal Error: An error occured during preRender()");
			}

			void Context::dispatch(const DispatchFunction dispatch) {
				MACE_ASSERT(dispatch, "Internal Error: DispatchFunction in OGL Context was empty!");
				if (currentContext == this) {
					processDispatchQueue();
					dispatch();
				} else {
					std::lock_guard<std::mutex> guard(dispatchMutex);
					dispatchQueue.push(std::move(dispatch));
				}
			}

			void Context::processDispatchQueue() {
				while (!dispatchQueue.empty()) {
					const DispatchFunction dispatch = dispatchQueue.front();
					dispatchQueue.pop();
					MACE_ASSERT(dispatch, "Internal Error: DispatchFunction in queue was empty!");
					dispatch();
				}
			}

			Context::Context(gfx::WindowModule* win) : GraphicsContextComponent(win) {}

			gfx::ComponentPtr<gfx::Renderer> Context::createRenderTarget() {
				return gfx::ComponentPtr<gfx::Renderer>(new ogl::Renderer(std::static_pointer_cast<Context>(shared_from_this())));
			}

			std::shared_ptr<gfx::ModelImpl> Context::createModelImpl() {
				return std::shared_ptr<gfx::ModelImpl>(new ogl::Model(std::static_pointer_cast<Context>(shared_from_this())));
			}

			std::shared_ptr<gfx::TextureImpl> Context::createTextureImpl(const gfx::TextureDesc& desc) {
				return std::shared_ptr<gfx::TextureImpl>(new ogl::Texture(std::static_pointer_cast<Context>(shared_from_this()), desc));
			}
			std::shared_ptr<gfx::FontImpl> Context::createFontImpl(const gfx::FontDesc& desc) {
				return std::shared_ptr<gfx::FontImpl>(new fty::FreetypeFont(desc, freetype));
			}
		}//ogl
	}//internal
}//mc
