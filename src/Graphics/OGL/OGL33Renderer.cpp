/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

//std::copy raises a dumb warning on this version of msvc which does not contribute anything at all
//the reason why this check does not use MACE_MSVC is because that would require including Constants.h which is what raises the warning
#if defined(_MSC_VER) && _MSC_VER >= 1400 
#	pragma warning( push ) 
#	pragma warning( disable: 4996 ) 
#endif 

#define MACE_EXPOSE_GLFW
#include <MACE/Graphics/OGL/OGL33Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Core/System.h>

#include <GLFW/glfw3.h>

//we need to include algorithim for std::copy
#include <algorithm>
//cstring is for strcmp
#include <cstring>
//std::begin and std::end
#include <iterator>

//output error messages to console
#include <sstream>

//for printing error messages/opengl info
#include <iostream>

namespace mc {
	namespace gfx {
		namespace ogl33 {
			//how many floats in the uniform buffer
#define MACE__ENTITY_DATA_BUFFER_SIZE sizeof(float) * 28
			//which binding location the uniform buffer goes to
#define MACE__ENTITY_DATA_LOCATION 0
#define MACE__ENTITY_DATA_USAGE GL_DYNAMIC_DRAW
			//the definition is later stringified. cant be a string because this gets added to the shader via a macro (see createShader)
#define MACE__ENTITY_DATA_NAME _mc_EntityData

#define MACE__PAINTER_DATA_BUFFER_SIZE sizeof(float) * 56
#define MACE__PAINTER_DATA_LOCATION 1
#define MACE__PAINTER_DATA_USAGE GL_DYNAMIC_DRAW
#define MACE__PAINTER_DATA_NAME _mc_PainterData

#define MACE__SCENE_ATTACHMENT_INDEX 0
#define MACE__ID_ATTACHMENT_INDEX 1
#define MACE__DATA_ATTACHMENT_INDEX 2

			namespace {
				Shader createShader(const Enum type, const Painter::RenderFeatures features, const char* source) {
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

					if ((features & Painter::RenderFeatures::DISCARD_INVISIBLE) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_DISCARD_INVISIBLE 1\n");
					}
					if ((features & Painter::RenderFeatures::FILTER) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_FILTER 1\n");
					}
					if ((features & Painter::RenderFeatures::TEXTURE) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_TEXTURE 1\n");
					}
					if ((features & Painter::RenderFeatures::TEXTURE_TRANSFORM) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_TEXTURE_TRANSFORM 1\n");
					}
					if ((features & Painter::RenderFeatures::INHERIT_TRANSLATION) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_INHERIT_TRANSLATION 1\n");
					}
					if ((features & Painter::RenderFeatures::INHERIT_SCALE) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_INHERIT_SCALE 1\n");
					}
					if ((features & Painter::RenderFeatures::INHERIT_ROTATION) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_INHERIT_ROTATION 1\n");
					}
					if ((features & Painter::RenderFeatures::STORE_ID) != Painter::RenderFeatures::NONE) {
						sources.insert(sources.begin(), "#define MACE_STORE_ID 1\n");
					}

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
#ifdef MACE_DEBUG_INTERNAL_ERRORS
					else {
						MACE__THROW(BadFormat, "Internal Error: Unknown GLSL shader type: " + std::to_string(type));
					}
#endif
					sources.push_back(source);
					s.setSource(sources.size(), sources.data(), nullptr);
					s.compile();
					return s;
				}

				ogl33::ShaderProgram createShadersForSettings(const std::pair<Painter::Brush, Painter::RenderFeatures>& settings) {
					ogl33::ShaderProgram program;
					program.init();

					ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error initializing ShaderProgram");

					program.attachShader(createShader(GL_VERTEX_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/RenderTypes/standard.v.glsl>
					));

					if (settings.first == Painter::Brush::COLOR) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/color.f.glsl>
						));

						program.link();
					} else if (settings.first == Painter::Brush::TEXTURE) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/texture.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex");

						program.setUniform("tex", static_cast<int>(TextureSlot::FOREGROUND));
					} else if (settings.first == Painter::Brush::MASK) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/mask.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex");
						program.createUniform("mask");

						//binding the samplers
						program.setUniform("tex", static_cast<int>(TextureSlot::FOREGROUND));
						program.setUniform("mask", static_cast<int>(TextureSlot::MASK));
					} else if (settings.first == Painter::Brush::BLEND) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/blend.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex1");
						program.createUniform("tex2");

						program.setUniform("tex1", static_cast<int>(TextureSlot::FOREGROUND));
						program.setUniform("tex2", static_cast<int>(TextureSlot::BACKGROUND));
					} else if (settings.first == Painter::Brush::MASKED_BLEND) {
						program.attachShader(createShader(GL_FRAGMENT_SHADER, settings.second,
#							include <MACE/Graphics/OGL/Shaders/Brushes/masked_blend.f.glsl>
						));

						program.link();

						program.bind();

						program.createUniform("tex1");
						program.createUniform("tex2");
						program.createUniform("mask");

						//binding the samplers
						program.setUniform("tex1", static_cast<int>(TextureSlot::FOREGROUND));
						program.setUniform("tex2", static_cast<int>(TextureSlot::BACKGROUND));
						program.setUniform("mask", static_cast<int>(TextureSlot::MASK));
					} else {
						MACE__THROW(BadFormat, "OpenGL 3.3 Renderer: Unsupported brush type: " + std::to_string(static_cast<unsigned int>(settings.first)));
					}

					ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating shader program for painter");
					return program;
				}
			}//anon namespace

			OGL33Renderer::OGL33Renderer() {}

			void OGL33Renderer::onInit(gfx::WindowModule* win) {
				glewExperimental = true;
				const GLenum result = glewInit();
				if (result != GLEW_OK) {
					std::ostringstream errorMessage;
					errorMessage << "OpenGL failed to initialize: ";
					//to convert from GLubyte* to string, we can use the << in ostream. For some reason the
					//+ operater in std::string can not handle this conversion.
					errorMessage << glewGetErrorString(result);
					errorMessage << ": ";
					if (result == GLEW_ERROR_NO_GL_VERSION) {
						errorMessage << "NO_GL_VERSION: There was no OpenGL version found on this system";
					} else if (result == GLEW_ERROR_GL_VERSION_10_ONLY) {
						errorMessage << "GL_VERSION_10_ONLY: The version of OpenGL found on this system is outdated: OpenGL 1.0 found, OpenGL 1.1+ required";
					}

					MACE__THROW(UnsupportedRenderer, errorMessage.str());
				}

				if (!GLEW_VERSION_3_3) {
					std::ostringstream errorMessage;
					errorMessage << "This system (OpenGL " << glGetString(GL_VERSION) << ")";
					errorMessage << " does not support the required OpenGL version required by this renderer, ";
					errorMessage << "OpenGL 3.3";
					MACE__THROW(UnsupportedRenderer, errorMessage.str());
				}

				try {
					gfx::ogl33::forceCheckGLError(__LINE__, __FILE__, "Internal Error: This should be ignored silently, it is a bug with glew");
				} catch (...) {
					//glew sometimes throws errors that can be ignored (GL_INVALID_ENUM)
					//see https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library (section GLEW) saying to ignore a GLEW error immediatevely after glewInit()
				}

#ifdef MACE_DEBUG_OPENGL
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "OpenGL Info:" << std::endl;
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Version: " << std::endl << "\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_VERSION) << std::endl;
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Vendor: " << std::endl << "\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_VENDOR) << std::endl;
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Renderer: " << std::endl << "\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_RENDERER) << std::endl;
				std::cout << os::consoleColor(os::ConsoleColor::LIGHT_GREEN) << "Shader version: " << std::endl << "\t";
				std::cout << os::consoleColor(os::ConsoleColor::GREEN) << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
				std::cout << os::consoleColor();
#endif

				const WindowModule::LaunchConfig& config = win->getLaunchConfig();

				generateFramebuffer(config.width, config.height);

				ogl33::forceCheckGLError(__LINE__, __FILE__, "An OpenGL error occured initializing OGL33Renderer");
			}

			void OGL33Renderer::onSetUp(gfx::WindowModule *) {
				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: An error occured before onSetUp");

				frameBuffer.bind();

				MACE_CONSTEXPR const Enum buffers[] = {
					GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
					GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
					GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX
				};
				frameBuffer.setDrawBuffers(3, buffers);

				glClearBufferfi(GL_DEPTH_STENCIL, 0, 0.0f, 0);

				glClearBufferfv(GL_COLOR, MACE__SCENE_ATTACHMENT_INDEX, clearColor.begin());

				MACE_CONSTEXPR const GLuint idClearValue = 0;

				glClearBufferuiv(GL_COLOR, MACE__ID_ATTACHMENT_INDEX, &idClearValue);

				MACE_CONSTEXPR const float dataClearValues[] = {
					0.0f,
					0.0f,
					0.0f,
					0.0f
				};

				glClearBufferfv(GL_COLOR, MACE__DATA_ATTACHMENT_INDEX, dataClearValues);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to clear framebuffer");
			}

			void OGL33Renderer::onTearDown(gfx::WindowModule * win) {
				ogl33::checkGLError(__LINE__, __FILE__, "Error occured during rendering");

				frameBuffer.unbind();

				ogl33::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				int width, height;
				glfwGetWindowSize(win->getGLFWWindow(), &width, &height);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.getID());
				ogl33::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
				ogl33::FrameBuffer::setDrawBuffer(GL_BACK);

				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to tear down renderer");

				glfwSwapBuffers(win->getGLFWWindow());

				ogl33::forceCheckGLError(__LINE__, __FILE__, "An OpenGL error occurred during a rendering frame");
			}

			void OGL33Renderer::onResize(gfx::WindowModule*, const Size width, const Size height) {
				frameBuffer.destroy();
				{
					Object* renderBuffers[] = {
						&sceneBuffer,
						&idBuffer,
						&dataBuffer,
						&depthStencilBuffer
					};

					RenderBuffer::destroy(renderBuffers, 4);
				}

				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we make it 1 instead

				ogl33::setViewport(0, 0, width == 0 ? 1 : width, height == 0 ? 1 : height);

				generateFramebuffer(width == 0 ? 1 : width, height == 0 ? 1 : height);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error resizing framebuffer for renderer");
			}

			void OGL33Renderer::onDestroy() {
				frameBuffer.destroy();

				{
					Object* renderBuffers[] = {
						&sceneBuffer,
						&idBuffer,
						&dataBuffer,
						&depthStencilBuffer
					};

					RenderBuffer::destroy(renderBuffers, 4);
				}

				for (auto iter = protocols.begin(); iter != protocols.end(); ++iter) {
					iter->second.program.destroy();
				}

				protocols.clear();

				ogl33::forceCheckGLError(__LINE__, __FILE__, "Internal Error: Error destroying OpenGL 3.3 renderer");

			}

			void OGL33Renderer::onQueue(GraphicsEntity *) {}

			void OGL33Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
				clearColor = Color(r, g, b, a);
			}

			void OGL33Renderer::generateFramebuffer(const int width, const int height) {
				{
					Object* renderBuffers[] = {
						&sceneBuffer,
						&idBuffer,
						&dataBuffer,
						&depthStencilBuffer
					};

					RenderBuffer::init(renderBuffers, 4);
				}

				depthStencilBuffer.bind();
				depthStencilBuffer.setStorage(GL_DEPTH_STENCIL, width, height);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating depth buffers for renderer");

				sceneBuffer.bind();
				sceneBuffer.setStorage(GL_RGBA, width, height);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating scene texture for renderer");

				idBuffer.bind();
				idBuffer.setStorage(GL_R32UI, width, height);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating id texture for renderer");

				dataBuffer.bind();
				dataBuffer.setStorage(GL_RGBA, width, height);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating data texture for renderer");

				frameBuffer.init();
				frameBuffer.bind();

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating FrameBuffer for the renderer");

				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX, sceneBuffer);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX, idBuffer);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX, dataBuffer);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilBuffer);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error attaching texture to FrameBuffer for the renderer");

				switch (frameBuffer.checkStatus(GL_FRAMEBUFFER)) {
					case GL_FRAMEBUFFER_UNDEFINED:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_UNDEFINED: The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: One of the framebuffer attachments are incomplete!");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The framebuffer is missing at least one image");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi. ");
						break;
					case GL_FRAMEBUFFER_UNSUPPORTED:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. It can also be that the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
						MACE__THROW(Framebuffer, "GL_FRAMEBUFFER_LAYER_TARGETS: Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target. ");
						break;
					case GL_FRAMEBUFFER_COMPLETE:
					default:
						//success
						break;
				}

				setTarget(FrameBufferTarget::COLOR);

				ogl33::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting draw buffers in FrameBuffer for the renderer");

				ogl33::setViewport(0, 0, width, height);

				//gl states
				ogl33::enable(GL_BLEND);
				ogl33::setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				ogl33::enable(GL_MULTISAMPLE);

				const gfx::WindowModule::LaunchConfig& config = context->getWindow()->getLaunchConfig();

				windowRatios = {
					static_cast<float>(config.width) / static_cast<float>(width),
					static_cast<float>(config.height) / static_cast<float>(height)
				};

				ogl33::forceCheckGLError(__LINE__, __FILE__,
					("Internal Error: OGL33Renderer: Error resizing framebuffer for width " + std::to_string(width) + " and height " + std::to_string(height)).c_str());
			}

			void OGL33Renderer::getEntitiesAt(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h, EntityID* arr) const {
				frameBuffer.bind();

				const Vector<int, 2> framebufferSize = getContext()->getWindow()->getFramebufferSize();

				ogl33::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				ogl33::FrameBuffer::setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				//opengl y-axis is inverted from window coordinates
				frameBuffer.readPixels(x, framebufferSize.y() - y, w, h, GL_RED_INTEGER, GL_UNSIGNED_INT, arr);
			}

			void OGL33Renderer::getPixelsAt(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h, Color * arr, const FrameBufferTarget target) const {
				frameBuffer.bind();

				const Vector<int, 2> framebufferSize = getContext()->getWindow()->getFramebufferSize();

				Enum colorAttachment;
				switch (target) {
					case FrameBufferTarget::COLOR:
						MACE_FALLTHROUGH;
					default:
						colorAttachment = GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX;
						break;
					case FrameBufferTarget::DATA:
						colorAttachment = GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX;
						break;
				}
				ogl33::FrameBuffer::setReadBuffer(colorAttachment);
				ogl33::FrameBuffer::setDrawBuffer(colorAttachment);
				//opengl y-axis is inverted from window coordinates
				frameBuffer.readPixels(x, framebufferSize.y() - y, w, h, GL_RGBA, GL_FLOAT, arr);
			}

			std::shared_ptr<PainterImpl> OGL33Renderer::createPainterImpl() {
				return std::shared_ptr<PainterImpl>(new OGL33Painter(this));
			}

			void OGL33Renderer::bindProtocol(OGL33Painter* painter, const std::pair<Painter::Brush, Painter::RenderFeatures> settings) {
				auto protocol = protocols.find(settings);
				if (protocol == protocols.end()) {
					RenderProtocol prot = RenderProtocol();
					prot.program = createShadersForSettings(settings);

					prot.program.bind();

					prot.program.createUniformBuffer(MACE_STRINGIFY_DEFINITION(MACE__ENTITY_DATA_NAME), MACE__ENTITY_DATA_LOCATION);
					prot.program.createUniformBuffer(MACE_STRINGIFY_DEFINITION(MACE__PAINTER_DATA_NAME), MACE__PAINTER_DATA_LOCATION);

					auto newProtocolEntry = std::pair<std::pair<Painter::Brush, Painter::RenderFeatures>, OGL33Renderer::RenderProtocol>(settings, prot);
					protocols.insert(newProtocolEntry);

					protocol = protocols.find(newProtocolEntry.first);
				}

				protocol->second.program.bind();
				protocol->second.program.bindUniformBuffer(painter->entityData);
				protocol->second.program.bindUniformBuffer(painter->painterData);
			}

			void OGL33Renderer::setTarget(const FrameBufferTarget & target) {
				if (target == FrameBufferTarget::COLOR) {
					MACE_CONSTEXPR const Enum buffers[] = {
						GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
						GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
					};
					frameBuffer.setDrawBuffers(2, buffers);
				} else if (target == FrameBufferTarget::DATA) {
					MACE_CONSTEXPR const Enum buffers[] = {
						GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX,
						GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
					};
					frameBuffer.setDrawBuffers(2, buffers);
				}

			}

			OGL33Painter::OGL33Painter(OGL33Renderer* const r) : renderer(r) {}

			void OGL33Painter::init() {
				Object* buffers[] = {
					&entityData,
					&painterData
				};

				UniformBuffer::init(buffers, 2);

				createPainterData();
				createEntityData();
			}

			void OGL33Painter::createEntityData() {
				MACE_STATIC_ASSERT(sizeof(float) >= sizeof(EntityID), "This system doesn't not support the required size for EntityID");

				savedMetrics = painter->getEntity()->getMetrics();

				entityData.bind();

				float entityDataBuffer[MACE__ENTITY_DATA_BUFFER_SIZE / sizeof(float)] = {};

				savedMetrics.translation.flatten(entityDataBuffer);
				//offset by 4
				savedMetrics.rotation.flatten(entityDataBuffer + 4);
				savedMetrics.scale.flatten(entityDataBuffer + 8);
				savedMetrics.inheritedTranslation.flatten(entityDataBuffer + 12);
				savedMetrics.inheritedRotation.flatten(entityDataBuffer + 16);
				savedMetrics.inheritedScale.flatten(entityDataBuffer + 20);
				//this crazy line puts a GLuint directly into a float, as GLSL expects a uint instead of a float
				*reinterpret_cast<GLuint*>(entityDataBuffer + 24) = static_cast<GLuint>(painter->getID());

				//we set it to null, because during the actual rendering we set the data
				entityData.setData(MACE__ENTITY_DATA_BUFFER_SIZE, entityDataBuffer, MACE__ENTITY_DATA_USAGE);

				entityData.setName(MACE_STRINGIFY_DEFINITION(MACE__ENTITY_DATA_NAME));
			}

			void OGL33Painter::createPainterData() {
				savedState = painter->getState();//create default state

				painterData.bind();

				float painterDataBuffer[MACE__PAINTER_DATA_BUFFER_SIZE / sizeof(float)] = { 0 };

				savedState.transformation.translation.flatten(painterDataBuffer);
				savedState.transformation.rotation.flatten(painterDataBuffer + 4);
				savedState.transformation.scaler.flatten(painterDataBuffer + 8);
				savedState.data.flatten(painterDataBuffer + 12);
				savedState.foregroundColor.flatten(painterDataBuffer + 16);
				savedState.foregroundTransform.flatten(painterDataBuffer + 20);
				savedState.backgroundColor.flatten(painterDataBuffer + 24);
				savedState.backgroundTransform.flatten(painterDataBuffer + 28);
				savedState.maskColor.flatten(painterDataBuffer + 32);
				savedState.maskTransform.flatten(painterDataBuffer + 36);
				savedState.filter.flatten(painterDataBuffer + 40);

				painterData.setData(MACE__PAINTER_DATA_BUFFER_SIZE, painterDataBuffer, MACE__PAINTER_DATA_USAGE);

				painterData.setName(MACE_STRINGIFY_DEFINITION(MACE__PAINTER_DATA_NAME));
			}

			void OGL33Painter::destroy() {
				Object* buffers[] = {
					&entityData,
					&painterData
				};

				UniformBuffer::destroy(buffers, 2);
			}

			void OGL33Painter::begin() {
				painterData.bind();

				entityData.bind();
			}

			void OGL33Painter::end() {}

			void OGL33Painter::setTarget(const FrameBufferTarget & target) {
				renderer->setTarget(target);
			}

			void OGL33Painter::clean() {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
				if (!painter->getEntity()->getProperty(Entity::INIT)) {
					MACE__THROW(InitializationFailed, "Internal Error: OGL33Renderer: clean(): Painter Entity is not initializd.");
				}
#endif

				const Entity::Metrics metrics = painter->getEntity()->getMetrics();

				if (metrics == savedMetrics) {
					return;
				}

				//now we set the data defining the transformations of the entity
				entityData.bind();

				if (metrics.translation != savedMetrics.translation) {
					entityData.setDataRange(0, sizeof(float) * 3, metrics.translation.begin());
				}
				if (metrics.rotation != savedMetrics.rotation) {
					entityData.setDataRange(sizeof(float) * 4, sizeof(float) * 3, metrics.rotation.begin());
				}
				if (metrics.scale != savedMetrics.scale) {
					entityData.setDataRange(sizeof(float) * 8, sizeof(float) * 3, metrics.scale.begin());
				}
				if (metrics.inheritedTranslation != savedMetrics.inheritedTranslation) {
					entityData.setDataRange(sizeof(float) * 12, sizeof(float) * 3, metrics.inheritedTranslation.begin());
				}
				if (metrics.inheritedRotation != savedMetrics.inheritedRotation) {
					entityData.setDataRange(sizeof(float) * 16, sizeof(float) * 3, metrics.inheritedRotation.begin());
				}
				if (metrics.inheritedScale != savedMetrics.inheritedScale) {
					entityData.setDataRange(sizeof(float) * 20, sizeof(float) * 3, metrics.scale.begin());
				}

				savedMetrics = metrics;

				checkGLError(__LINE__, __FILE__, "Internal Error: Failed to update GPU-side buffer in Entity clean");
			}

			void OGL33Painter::loadSettings(const Painter::State& state) {
				if (state == savedState) {
					return;
				}

				painterData.bind();

				if (state.transformation.translation != savedState.transformation.translation) {
					painterData.setDataRange(0, sizeof(float) * 3, state.transformation.translation.begin());
				}
				if (state.transformation.rotation != savedState.transformation.rotation) {
					painterData.setDataRange(sizeof(float) * 4, sizeof(float) * 3, state.transformation.rotation.begin());
				}
				if (state.transformation.scaler != savedState.transformation.scaler) {
					painterData.setDataRange(sizeof(float) * 8, sizeof(float) * 3, state.transformation.scaler.begin());
				}
				if (state.data != savedState.data) {
					painterData.setDataRange(sizeof(float) * 12, sizeof(float) * 4, state.data.begin());
				}
				if (state.foregroundColor != savedState.foregroundColor) {
					painterData.setDataRange(sizeof(float) * 16, sizeof(float) * 4, state.foregroundColor.begin());
				}
				if (state.foregroundTransform != savedState.foregroundTransform) {
					painterData.setDataRange(sizeof(float) * 20, sizeof(float) * 4, state.foregroundTransform.begin());
				}
				if (state.backgroundColor != savedState.backgroundColor) {
					painterData.setDataRange(sizeof(float) * 24, sizeof(float) * 4, state.backgroundColor.begin());
				}
				if (state.backgroundTransform != savedState.backgroundTransform) {
					painterData.setDataRange(sizeof(float) * 28, sizeof(float) * 4, state.backgroundTransform.begin());
				}
				if (state.maskColor != savedState.maskColor) {
					painterData.setDataRange(sizeof(float) * 32, sizeof(float) * 4, state.maskColor.begin());
				}
				if (state.maskTransform != savedState.maskTransform) {
					painterData.setDataRange(sizeof(float) * 36, sizeof(float) * 4, state.maskTransform.begin());
				}

				if (state.filter != savedState.filter) {
					float matrix[16];
					state.filter.flatten(matrix);
					painterData.setDataRange(sizeof(float) * 40, sizeof(float) * 16, std::begin(matrix));
				}

				savedState = state;
			}

			void OGL33Painter::draw(const Model& m, const Painter::Brush brush) {
				renderer->bindProtocol(this, { brush, savedState.renderFeatures });

				m.bind();
				m.draw();
			}
		}//ogl33
	}//gfx
}//mc

#if defined(_MSC_VER) && _MSC_VER >= 1400 
 //pop the disable of warning 4996 which is a useless warning in our scenario
#	pragma warning( pop ) 
#endif 
