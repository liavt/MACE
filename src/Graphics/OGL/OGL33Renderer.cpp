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
		namespace ogl {
			namespace {
				//how many floats in the uniform buffer
#define MACE__ENTITY_DATA_BUFFER_SIZE sizeof(float) * 20
				//which binding location the uniform buffer goes to
#define MACE__ENTITY_DATA_LOCATION 0
#define MACE__ENTITY_DATA_USAGE GL_DYNAMIC_DRAW
				//the definition is later stringified. cant be a string because this gets added to the shader.
#define MACE__ENTITY_DATA_NAME _mc_EntityData

#define MACE__PAINTER_DATA_BUFFER_SIZE sizeof(float) * 56
#define MACE__PAINTER_DATA_LOCATION 1
#define MACE__PAINTER_DATA_USAGE GL_DYNAMIC_DRAW
#define MACE__PAINTER_DATA_NAME _mc_PainterData

#define MACE__SCENE_ATTACHMENT_INDEX 0
#define MACE__ID_ATTACHMENT_INDEX 1

				Shader createShader(const Enum type, const char* source) {
					Shader s = Shader(type);
					s.init();
#define MACE__SHADER_MACRO(name, def) "#define " #name " " MACE_STRINGIFY_DEFINITION(def) "\n"
					std::vector<const char*> sources = std::vector<const char*>({
						"#version 330 core\n",
						MACE__SHADER_MACRO(MACE_ENTITY_DATA_LOCATION, MACE__ENTITY_DATA_LOCATION),
						MACE__SHADER_MACRO(MACE_ENTITY_DATA_NAME, MACE__ENTITY_DATA_NAME),
						MACE__SHADER_MACRO(MACE_PAINTER_DATA_LOCATION, MACE__PAINTER_DATA_LOCATION),
						MACE__SHADER_MACRO(MACE_PAINTER_DATA_NAME, MACE__PAINTER_DATA_NAME),
						MACE__SHADER_MACRO(MACE_SCENE_ATTACHMENT_INDEX, MACE__SCENE_ATTACHMENT_INDEX),
						MACE__SHADER_MACRO(MACE_ID_ATTACHMENT_INDEX, MACE__ID_ATTACHMENT_INDEX),
						MACE__SHADER_MACRO(MACE_VAO_DEFAULT_VERTICES_LOCATION, MACE__VAO_DEFAULT_VERTICES_LOCATION),
						MACE__SHADER_MACRO(MACE_VAO_DEFAULT_TEXTURE_COORD_LOCATION, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION),
#include <MACE/Graphics/OGL/Shaders/Shared.glsl>
					});
#undef MACE__SHADER_MACRO

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
			}

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
					gfx::ogl::forceCheckGLError(__LINE__, __FILE__, "Internal Error: This should be ignored silently, it is a bug with glew");
				} catch (...) {
					//glew sometimes throws errors that can be ignored (GL_INVALID_ENUM)
					//see https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library (section GLEW) saying to ignore a GLEW error
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

				//gl states
				ogl::enable(GL_BLEND);
				ogl::setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				ogl::enable(GL_MULTISAMPLE);

				ogl::forceCheckGLError(__LINE__, __FILE__, "An OpenGL error occured initializing OGL33Renderer");
			}

			void OGL33Renderer::onSetUp(gfx::WindowModule *) {
				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: An error occured before onSetUp");

				frameBuffer.bind();
				sceneTexture.bind();
				idTexture.bind();

				frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to clear color buffer");

				//we want to clear the id texture to black only - not the color set by the user. this requires 3 setDrawBuffers - which is annoying
				frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to clear ID buffer");

				MACE_CONSTEXPR const Enum drawBuffers[] = {
					GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
					GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX
				};
				frameBuffer.setDrawBuffers(2, drawBuffers);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to set draw buffers for framebuffer");

			}

			void OGL33Renderer::onTearDown(gfx::WindowModule * win) {
				ogl::checkGLError(__LINE__, __FILE__, "Error occured during rendering");

				frameBuffer.unbind();

				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				int width, height;
				glfwGetWindowSize(win->getGLFWWindow(), &width, &height);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.getID());
				ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setDrawBuffer(GL_BACK);

				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to tear down renderer");

				glfwSwapBuffers(win->getGLFWWindow());

				ogl::forceCheckGLError(__LINE__, __FILE__, "An OpenGL error occurred during a rendering frame");
			}

			void OGL33Renderer::onResize(gfx::WindowModule*, const Size width, const Size height) {
				frameBuffer.destroy();
				depthBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();

				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we make it 1 instead

				ogl::setViewport(0, 0, width == 0 ? 1 : width, height == 0 ? 1 : height);

				generateFramebuffer(width == 0 ? 1 : width, height == 0 ? 1 : height);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error resizing framebuffer for renderer");
			}

			void OGL33Renderer::onDestroy() {
				depthBuffer.destroy();

				frameBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();

				for (auto iter = protocols.begin(); iter != protocols.end(); ++iter) {
					iter->second.program.destroy();
				}

				protocols.clear();

				ogl::forceCheckGLError(__LINE__, __FILE__, "Internal Error: Error destroying OpenGL 3.3 renderer");

			}

			void OGL33Renderer::onQueue(GraphicsEntity *) {}

			void OGL33Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
				clearColor = Color(r, g, b, a);
			}

			void OGL33Renderer::generateFramebuffer(const int width, const int height) {
				depthBuffer.init();
				depthBuffer.bind();
				depthBuffer.setStorage(GL_DEPTH_COMPONENT, width, height);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating depth buffers for renderer");

				sceneTexture.init();
				sceneTexture.bind();
				sceneTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				sceneTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				sceneTexture.setData(nullptr, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, 0);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating scene texture for renderer");

				idTexture.init();
				idTexture.bind();
				idTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				idTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				idTexture.setData(nullptr, width, height, GL_UNSIGNED_INT, GL_RED_INTEGER, GL_R32UI, 0);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating id texture for renderer");

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting texture for renderer Z-buffers");

				//for our custom FBOs. we render using a z-sslBuffer to figure out which entity is clicked on
				frameBuffer.init();
				frameBuffer.bind();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating FrameBuffer for the renderer");

				frameBuffer.attachTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX, sceneTexture);
				frameBuffer.attachTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX, idTexture);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error attaching texture to FrameBuffer for the renderer");

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

				MACE_CONSTEXPR const Enum buffers[2] = {
					GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
					GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX
				};
				frameBuffer.setDrawBuffers(2, buffers);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting draw buffers in FrameBuffer for the renderer");

				ogl::setViewport(0, 0, width, height);

				const gfx::WindowModule::LaunchConfig& config = context->getWindow()->getLaunchConfig();

				windowRatios = {
					static_cast<float>(config.width) / static_cast<float>(width),
					static_cast<float>(config.height) / static_cast<float>(height)
				};

				ogl::forceCheckGLError(__LINE__, __FILE__,
					("Internal Error: OGL33Renderer: Error resizing framebuffer for width " + std::to_string(width) + " and height " + std::to_string(height)).c_str());
			}

			GraphicsEntity * OGL33Renderer::getEntityAt(const int x, const int y) {
				frameBuffer.bind();

				Vector<int, 2> framebufferSize = getContext()->getWindow()->getFramebufferSize();

				Index pixel = 0;
				ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				//opengl y-axis is inverted from window coordinates
				frameBuffer.readPixels(x, framebufferSize.y() - y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);

				if (pixel > 0) {
					//this could happen if the entity that was there was removed from the renderqueue
					//in between onRender() and getEntityAt()
					if (pixel > renderQueue.size()) {
						return nullptr;
					}

					return renderQueue[--pixel];
				}

				return nullptr;
			}

			std::shared_ptr<PainterImpl> OGL33Renderer::createPainterImpl(Painter* const p) {
				return std::shared_ptr<PainterImpl>(new OGL33Painter(this, p));
			}

			OGL33Renderer::RenderProtocol& OGL33Renderer::getProtocol(OGL33Painter* painter, const std::pair<Enums::Brush, Enums::RenderType> settings) {
				auto protocol = protocols.find(settings);
				if (protocol == protocols.end()) {
					RenderProtocol prot = RenderProtocol();
					prot.program = createShadersForSettings(settings);

					painter->painterData.bindToUniformBlock(prot.program, MACE_STRINGIFY_DEFINITION(MACE__PAINTER_DATA_NAME));
					painter->entityData.bindToUniformBlock(prot.program, MACE_STRINGIFY_DEFINITION(MACE__ENTITY_DATA_NAME));

					protocols.insert(std::pair<std::pair<Enums::Brush, Enums::RenderType>, OGL33Renderer::RenderProtocol>(settings, prot));

					protocol = protocols.find(settings);
				}

				return protocol->second;
			}

			ogl::ShaderProgram OGL33Renderer::createShadersForSettings(const std::pair<Enums::Brush, Enums::RenderType>& settings) {
				ogl::ShaderProgram program;
				program.init();

				if (settings.second == Enums::RenderType::STANDARD) {
					program.attachShader(createShader(GL_VERTEX_SHADER,
#include <MACE/Graphics/OGL/Shaders/RenderTypes/standard.v.glsl>
					));
				} else {
					MACE__THROW(BadFormat, "OpenGL 3.3 Renderer: Unsupported render type: " + std::to_string(static_cast<unsigned int>(settings.second)));
				}

				if (settings.first == Enums::Brush::COLOR) {
					program.attachShader(createShader(GL_FRAGMENT_SHADER,
#	include <MACE/Graphics/OGL/Shaders/Brushes/color.f.glsl>
					));

					program.link();
				} else if (settings.first == Enums::Brush::TEXTURE) {
					program.attachShader(createShader(GL_FRAGMENT_SHADER,
#include <MACE/Graphics/OGL/Shaders/Brushes/texture.f.glsl>
					));

					program.link();

					program.bind();

					program.createUniform("tex");

					program.setUniform("tex", static_cast<int>(Enums::TextureSlot::FOREGROUND));
				} else if (settings.first == Enums::Brush::MASK) {
					program.attachShader(createShader(GL_FRAGMENT_SHADER,
#	include <MACE/Graphics/OGL/Shaders/Brushes/mask.f.glsl>
					));

					program.link();

					program.bind();

					program.createUniform("tex");
					program.createUniform("mask");

					//binding the samplers
					program.setUniform("tex", static_cast<int>(Enums::TextureSlot::FOREGROUND));
					program.setUniform("mask", static_cast<int>(Enums::TextureSlot::MASK));
				} else if (settings.first == Enums::Brush::BLEND) {
					program.attachShader(createShader(GL_FRAGMENT_SHADER,
#	include <MACE/Graphics/OGL/Shaders/Brushes/blend.f.glsl>
					));

					program.link();

					program.bind();

					program.createUniform("tex1");
					program.createUniform("tex2");

					program.setUniform("tex1", static_cast<int>(Enums::TextureSlot::FOREGROUND));
					program.setUniform("tex2", static_cast<int>(Enums::TextureSlot::BACKGROUND));
				} else if (settings.first == Enums::Brush::MASKED_BLEND) {
					program.attachShader(createShader(GL_FRAGMENT_SHADER,
#	include <MACE/Graphics/OGL/Shaders/Brushes/masked_blend.f.glsl>
					));

					program.link();

					program.bind();

					program.createUniform("tex1");
					program.createUniform("tex2");
					program.createUniform("mask");

					//binding the samplers
					program.setUniform("tex1", static_cast<int>(Enums::TextureSlot::FOREGROUND));
					program.setUniform("tex2", static_cast<int>(Enums::TextureSlot::BACKGROUND));
					program.setUniform("mask", static_cast<int>(Enums::TextureSlot::MASK));
				} else {
					MACE__THROW(BadFormat, "OpenGL 3.3 Renderer: Unsupported brush type: " + std::to_string(static_cast<unsigned int>(settings.first)));
				}

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating shader program for painter");
				return program;
			}

			OGL33Painter::OGL33Painter(OGL33Renderer* const r, Painter* const p) : PainterImpl(p), renderer(r) {}

			void OGL33Painter::init() {
				createPainterData();
				createEntityData();
			}

			void OGL33Painter::createEntityData() {
				savedMetrics = painter->getEntity()->getMetrics();

				entityData.init();
				entityData.bind();

				float entityDataBuffer[MACE__ENTITY_DATA_BUFFER_SIZE / sizeof(float)] = {};

				savedMetrics.translation.flatten(entityDataBuffer);
				//offset by 4
				savedMetrics.rotation.flatten(entityDataBuffer + 4);
				savedMetrics.inheritedTranslation.flatten(entityDataBuffer + 8);
				savedMetrics.inheritedRotation.flatten(entityDataBuffer + 12);
				savedMetrics.scale.flatten(entityDataBuffer + 16);
				//this crazy line puts a GLuint directly into a float, as GLSL expects a uint instead of a float
				*reinterpret_cast<GLuint*>(entityDataBuffer + 19) = static_cast<GLuint>(painter->getID());

				//we set it to null, because during the actual rendering we set the data
				entityData.setData(MACE__ENTITY_DATA_BUFFER_SIZE, entityDataBuffer, MACE__ENTITY_DATA_USAGE);

				entityData.setLocation(MACE__ENTITY_DATA_LOCATION);
			}

			void OGL33Painter::createPainterData() {
				savedState = painter->getState();

				painterData.init();
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

				//we set it to null, because during the actual rendering we set the data
				painterData.setData(MACE__PAINTER_DATA_BUFFER_SIZE, painterDataBuffer, MACE__PAINTER_DATA_USAGE);

				painterData.setLocation(MACE__PAINTER_DATA_LOCATION);
			}

			void OGL33Painter::destroy() {
				painterData.destroy();
				entityData.destroy();
			}

			void OGL33Painter::begin() {
				painterData.bind();
				painterData.bindForRender();

				entityData.bind();
				entityData.bindForRender();
			}

			void OGL33Painter::end() {}

			void OGL33Painter::clean() {
				if (!painter->getEntity()->getProperty(Entity::INIT)) {
					MACE__THROW(InitializationFailed, "Entity is not initializd.");
				}

				const Entity::Metrics metrics = painter->getEntity()->getMetrics();

				if (metrics == savedMetrics) {
					return;
				}

				//now we set the uniforms defining the transformations of the entity
				entityData.bind();

				if (metrics.translation != savedMetrics.translation) {
					entityData.setDataRange(0, sizeof(float) * 3, metrics.translation.begin());
				}
				if (metrics.rotation != savedMetrics.rotation) {
					entityData.setDataRange(sizeof(float) * 4, sizeof(float) * 3, metrics.rotation.begin());
				}
				if (metrics.inheritedTranslation != savedMetrics.inheritedTranslation) {
					entityData.setDataRange(sizeof(float) * 8, sizeof(float) * 3, metrics.inheritedTranslation.begin());
				}
				if (metrics.inheritedRotation != savedMetrics.inheritedRotation) {
					entityData.setDataRange(sizeof(float) * 12, sizeof(float) * 3, metrics.inheritedRotation.begin());
				}
				if (metrics.scale != savedMetrics.scale) {
					entityData.setDataRange(sizeof(float) * 16, sizeof(float) * 3, metrics.scale.begin());
				}

				savedMetrics = metrics;
			}

			void OGL33Painter::loadSettings(const Painter::State& state) {
				if (state == savedState) {
					return;
				}

				painterData.bind();

				if (state.transformation != savedState.transformation) {
					painterData.setDataRange(0, sizeof(float) * 3, state.transformation.translation.begin());
					painterData.setDataRange(sizeof(float) * 4, sizeof(float) * 3, state.transformation.rotation.begin());
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
					float matrix[16] = {};
					state.filter.flatten(matrix);
					painterData.setDataRange(sizeof(float) * 40, sizeof(float) * 16, std::begin(matrix));
				}

				savedState = state;
			}

			void OGL33Painter::draw(const Model& m, const Enums::Brush brush, const Enums::RenderType type) {
				OGL33Renderer::RenderProtocol& prot = renderer->getProtocol(this, { brush, type });
				m.bind();

				prot.program.bind();

				m.draw();
			}
		}//ogl
	}//gfx
}//mc

#if defined(_MSC_VER) && _MSC_VER >= 1400 
 //pop the disable of warning 4996 which is a useless warning in our scenario
#	pragma warning( pop ) 
#endif 