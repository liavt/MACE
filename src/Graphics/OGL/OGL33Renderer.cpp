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
#include <MACE/Utility/Preprocessor.h>

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
			//magic constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

			//how many floats in the uniform buffer
#define MACE__ENTITY_DATA_BUFFER_SIZE sizeof(float) * 24
		//which binding location the uniform buffer goes to
#define MACE__ENTITY_DATA_LOCATION 0
#define MACE__ENTITY_DATA_USAGE GL_DYNAMIC_DRAW

#define MACE__PAINTER_DATA_BUFFER_SIZE (sizeof(float) * 3 * 4) + (3 * sizeof(Color))
#define MACE__PAINTER_DATA_LOCATION 1
#define MACE__PAINTER_DATA_USAGE GL_DYNAMIC_DRAW

#define MACE__SCENE_ATTACHMENT_INDEX 0
#define MACE__ID_ATTACHMENT_INDEX 1

		//location of vbo of vertices and texture coordinates in the vao for each protocl
#define MACE__VAO_VERTICES_LOCATION 0
#define MACE__VAO_TEX_COORD_LOCATION 1

			IncludeString vertexLibrary = IncludeString({
	#	include <MACE/Graphics/OGL/Shaders/mc_vertex.glsl>
			}, "mc_vertex");
			/**
			@todo Remove discard from shader
			*/
			IncludeString fragmentLibrary = IncludeString({
	#	include <MACE/Graphics/OGL/Shaders/mc_frag.glsl>
			}, "mc_frag");
			IncludeString positionLibrary = IncludeString({
	#	include <MACE/Graphics/OGL/Shaders/mc_position.glsl>
			}, "mc_position");
			IncludeString entityLibrary = IncludeString({
	#	include <MACE/Graphics/OGL/Shaders/mc_entity.glsl>
			}, "mc_entity");
			IncludeString coreLibrary = IncludeString({
	#	include <MACE/Graphics/OGL/Shaders/mc_core.glsl>
			}, "mc_core");

			OGL33Renderer::OGL33Renderer() : sslPreprocessor(""), frameBuffer(), depthBuffer(), sceneTexture(), idTexture(), clearColor(Colors::BLACK) {}

			void OGL33Renderer::onResize(const Size width, const Size height) {
				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we make it 1 instead

				ogl::setViewport(0, 0, width == 0 ? 1 : width, height == 0 ? 1 : height);

				depthBuffer.destroy();

				frameBuffer.destroy();

				generateFramebuffer(width == 0 ? 1 : width, height == 0 ? 1 : height);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error resizing framebuffer for renderer");
			}

			void OGL33Renderer::onInit(const Size width, const Size height) {
				glewExperimental = true;
				GLenum result = glewInit();
				if (result != GLEW_OK) {
					std::ostringstream errorMessage;
					errorMessage << "GLEW failed to initialize: ";
					//to convert from GLubyte* to string, we can use the << in ostream. For some reason the
					//+ operater in std::string can not handle this conversion.
					errorMessage << glewGetErrorString(result);

					if (result == GLEW_ERROR_NO_GL_VERSION) {
						errorMessage << "\nThis can be a result of an outdated graphics driver. Please ensure that you have OpenGL 3.0+";
					}
					MACE__THROW(InitializationFailed, errorMessage.str());
				}

				try {
					gfx::ogl::checkGLError(__LINE__, __FILE__, "Internal Error: This should be ignored silently, it is a bug with glew");
				} catch (...) {
					//glew sometimes throws errors that can be ignored (GL_INVALID_ENUM)
				}

				if (!GLEW_VERSION_3_3) {
					std::cerr << "OpenGL 3.3 not found, falling back to a lower version, which may cause undefined results. Try updating your graphics driver to fix this." << std::endl;
				}

#ifdef MACE_DEBUG
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
				std::cout << "OpenGL has been created succesfully!" << std::endl;
				std::cout << "Version: " << std::endl << "	" << glGetString(GL_VERSION) << std::endl;
				std::cout << "Vendor: " << std::endl << "	" << glGetString(GL_VENDOR) << std::endl;
				std::cout << "Renderer: " << std::endl << "	" << glGetString(GL_RENDERER) << std::endl;
				std::cout << "Shader version: " << std::endl << "	" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
#endif

				sceneTexture.init();
				sceneTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				sceneTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating scene texture for renderer");

				idTexture.init();
				idTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				idTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating id texture for renderer");

				//gl states
				ogl::enable(GL_BLEND);
				ogl::setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				ogl::enable(GL_MULTISAMPLE);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error enabling blending, and multisampling for renderer");

				generateFramebuffer(width, height);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error generating framebuffer for renderer");

				entityUniforms.init();
				entityUniforms.bind();
				//we set it to null, because during the actual rendering we set the data
				entityUniforms.setData(MACE__ENTITY_DATA_BUFFER_SIZE, nullptr, MACE__ENTITY_DATA_USAGE);

				entityUniforms.setLocation(MACE__ENTITY_DATA_LOCATION);

				painterUniforms.init();
				painterUniforms.bind();
				painterUniforms.setData(MACE__PAINTER_DATA_BUFFER_SIZE, nullptr, MACE__PAINTER_DATA_USAGE);

				painterUniforms.setLocation(MACE__PAINTER_DATA_LOCATION);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating UniformBuffer for renderer");
			}

			void OGL33Renderer::onSetUp(os::WindowModule *) {
				frameBuffer.bind();
				sceneTexture.bind();
				idTexture.bind();

				frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//we want to clear the id texture to black only - not the color set by the user. this requires 3 setDrawBuffers - which is annoying
				frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setClearColor(0, 0, 0, 1);
				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT);

				MACE_CONSTEXPR Enum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
					GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX };
				frameBuffer.setDrawBuffers(2, drawBuffers);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to set up renderer");

			}

			void OGL33Renderer::onTearDown(os::WindowModule * win) {
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
			}

			void OGL33Renderer::onDestroy() {
				depthBuffer.destroy();

				frameBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();

				entityUniforms.destroy();
				painterUniforms.destroy();

				for (std::map<std::pair<Painter::Brush, Painter::RenderType>, std::unique_ptr<RenderProtocol>>::iterator iter = protocols.begin(); iter != protocols.end(); ++iter) {
					iter->second->program.destroy();
					iter->second->vao.destroy();
				}

				protocols.clear();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error destroying renderer");
			}

			void OGL33Renderer::onQueue(GraphicsEntity *) {}

			void OGL33Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
				clearColor = Color(r, g, b, a);
			}

			GraphicsEntity * OGL33Renderer::getEntityAt(const int x, const int y) {
				frameBuffer.bind();

				GLFWwindow* win = glfwGetCurrentContext();
				if (win == nullptr) {
					MACE__THROW(InvalidState, "This thread does not have a rendering context!");
				}
				int height;
				glfwGetFramebufferSize(win, nullptr, &height);

				Index pixel = 0;
				frameBuffer.setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				//opengl y-axis is inverted from window coordinates
				frameBuffer.readPixels(x, height - y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);

				if (pixel > 0) {
					//the entity that was there was removed from the renderqueue
					if (pixel > renderQueue.size()) {
						return nullptr;
					}

					return renderQueue[--pixel];
				}
				return nullptr;
			}

			std::shared_ptr<PainterImpl> OGL33Renderer::getPainter(const GraphicsEntity * const entity) const {
				return std::shared_ptr<PainterImpl>(new OGL33Painter(entity));
			}

			void OGL33Renderer::generateFramebuffer(const Size& width, const Size& height) {
				depthBuffer.init();
				depthBuffer.bind();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating depth buffers for renderer");

				sceneTexture.setData(nullptr, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, 0);
				idTexture.setData(nullptr, width, height, GL_UNSIGNED_INT, GL_RED_INTEGER, GL_R32UI, 0);
				depthBuffer.setStorage(GL_DEPTH_COMPONENT, width, height);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting texture for renderer Z-buffers");

				//for our custom FBOs. we render using a z-sslBuffer to figure out which entity is clicked on
				frameBuffer.init();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating FrameBuffer for the renderer");

				frameBuffer.attachTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneTexture);
				frameBuffer.attachTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, idTexture);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error attaching texture to FrameBuffer for the renderer");

				switch (frameBuffer.checkStatus(GL_FRAMEBUFFER)) {
					case GL_FRAMEBUFFER_UNDEFINED:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_UNDEFINED: The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: One of the framebuffer attachments are incomplete!");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The framebuffer is missing at least one image");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi. ");
						break;
					case GL_FRAMEBUFFER_UNSUPPORTED:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. It can also be that the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures. ");
						break;
					case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
						throw ogl::FramebufferError("GL_FRAMEBUFFER_LAYER_TARGETS: Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target. ");
						break;
					case GL_FRAMEBUFFER_COMPLETE:
					default:
						//success
						break;
				}

				MACE_CONSTEXPR Enum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
				frameBuffer.setDrawBuffers(2, buffers);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting draw buffers in FrameBuffer for the renderer");

				glViewport(0, 0, width, height);

				windowRatios[0] = static_cast<float>(context->getWindow()->getLaunchConfig().width) / static_cast<float>(width);
				windowRatios[1] = static_cast<float>(context->getWindow()->getLaunchConfig().width) / static_cast<float>(height);
			}

			std::string OGL33Renderer::processShader(const std::string & shader) {
				Preprocessor shaderPreprocessor = Preprocessor(shader, getSSLPreprocessor());
				return shaderPreprocessor.preprocess();
			}

			void OGL33Renderer::loadEntityUniforms(const GraphicsEntity * const entity) {
				if (!entity->getProperty(Entity::INIT)) {
					MACE__THROW(InitializationFailed, "Entity is not initializd.");
				}

				const Entity::Metrics metrics = entity->getMetrics();
				const float opacity = entity->getOpacity();

				//now we set the uniforms defining the transformations of the entity
				entityUniforms.bind();

				//holy crap thats a lot of flags. this is the fastest way to map the sslBuffer. the difference is MASSIVE. try it.
				float* mappedEntityData = static_cast<float*>(entityUniforms.mapRange(0, MACE__ENTITY_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
				std::copy(metrics.translation.begin(), metrics.translation.end(), mappedEntityData);
				mappedEntityData += 4;//pointer arithmetic!
				std::copy(metrics.rotation.begin(), metrics.rotation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(metrics.inheritedTranslation.begin(), metrics.inheritedTranslation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(metrics.inheritedRotation.begin(), metrics.inheritedRotation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(metrics.scale.begin(), metrics.scale.end(), mappedEntityData);
				mappedEntityData += 3;
				std::copy(&opacity, &opacity + sizeof(opacity), mappedEntityData);
				entityUniforms.unmap();
			}

			void OGL33Renderer::loadPainterUniforms(const TransformMatrix& transform, const Color& prim, const Color& second, const Vector<float, 4>& data) {
				float color[4] = {};
				prim.flatten(color);

				//now we set the uniforms defining the transformations of the entity
				Binder b(&painterUniforms);
				//orphan the buffer (this is required for intel gpus)
				painterUniforms.setData(MACE__PAINTER_DATA_BUFFER_SIZE, nullptr, MACE__PAINTER_DATA_USAGE);
				//holy crap thats a lot of flags. this is the fastest way to map the buffer. basically it specifies we are writing, and to invalidate the old buffer and overwrite any changes.
				float* mappedEntityData = static_cast<float*>(painterUniforms.mapRange(0, MACE__PAINTER_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
				std::copy(transform.translation.begin(), transform.translation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(transform.rotation.begin(), transform.rotation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(transform.scaler.begin(), transform.scaler.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(std::begin(color), std::end(color), mappedEntityData);
				mappedEntityData += 4;
				second.flatten(color);
				std::copy(std::begin(color), std::end(color), mappedEntityData);
				mappedEntityData += 4;
				std::copy(data.begin(), data.end(), mappedEntityData);
				painterUniforms.unmap();
			}

			OGL33Renderer::RenderProtocol& OGL33Renderer::getProtocol(const GraphicsEntity* const entity, const std::pair<Painter::Brush, Painter::RenderType> settings) {
				auto protocol = protocols.find(settings);
				if (protocol == protocols.end()) {
					std::unique_ptr<OGL33Renderer::RenderProtocol> prot = std::unique_ptr<OGL33Renderer::RenderProtocol>(new OGL33Renderer::RenderProtocol());
					prot->program = getShadersForSettings(settings);
					prot->vao = getVAOForSettings(settings);

					protocols.insert(std::pair<std::pair<Painter::Brush, Painter::RenderType>, std::unique_ptr<OGL33Renderer::RenderProtocol>>(settings, std::move(prot)));

					protocol = protocols.find(settings);
				}

#ifdef MACE_DEBUG
				if (protocol->second == nullptr) {
					MACE__THROW(NullPointer, "Internal Error: protocol was nullptr");
				}
#endif

				protocol->second->program.bind();
				protocol->second->program.setUniform("mc_EntityID", static_cast<unsigned int>(entity->getID()));

				return *protocol->second;
			}

			ogl::ShaderProgram OGL33Renderer::getShadersForSettings(const std::pair<Painter::Brush, Painter::RenderType>& settings) {
				ogl::ShaderProgram program;
				program.init();

				if (settings.second == Painter::RenderType::QUAD) {
					program.createVertex(processShader({
	#include <MACE/Graphics/OGL/Shaders/RenderTypes/quad.v.glsl>
					}));
				} else {
					MACE__THROW(BadFormat, "OpenGL Renderer: Unsupported render type: " + std::to_string(static_cast<unsigned int>(settings.second)));
				}

				if (settings.first == Painter::Brush::COLOR) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/color.f.glsl>
					}));

					program.link();
				} else if (settings.first == Painter::Brush::TEXTURE) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/texture.f.glsl>
					}));

					program.link();
				} else if (settings.first == Painter::Brush::MASK) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/mask.f.glsl>
					}));

					program.link();

					program.bind();

					program.createUniform("tex");
					program.createUniform("mask");

					//binding the samplers
					program.setUniform("tex", 0);
					program.setUniform("mask", 1);
				} else if (settings.first == Painter::Brush::MASKED_BLEND) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/masked_blend.f.glsl>
					}));

					program.link();

					program.bind();

					program.createUniform("tex1");
					program.createUniform("tex2");
					program.createUniform("mask");

					//binding the samplers
					program.setUniform("tex1", 0);
					program.setUniform("tex2", 1);
					program.setUniform("mask", 2);
				} else {
					MACE__THROW(BadFormat, "OpenGL Renderer: Unsupported brush type: " + std::to_string(static_cast<unsigned int>(settings.first)));
				}

				program.createUniform("mc_EntityID");
				entityUniforms.bindToUniformBlock(program, "mc_BaseEntityBuffer");
				painterUniforms.bindToUniformBlock(program, "mc_PainterSettingsBuffer");

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating shader program for painter!");
				return program;
			}

			ogl::VertexArray OGL33Renderer::getVAOForSettings(const std::pair<Painter::Brush, Painter::RenderType>& settings) {
				ogl::VertexArray vao;
				vao.init();

				if (settings.second == Painter::RenderType::QUAD) {
					MACE_CONSTEXPR float squareTextureCoordinates[8] = {
						0.0f,1.0f,
						0.0f,0.0f,
						1.0f,0.0f,
						1.0f,1.0f,
					};

					MACE_CONSTEXPR unsigned int squareIndices[6] = {
						0,1,3,
						1,2,3
					};

					MACE_CONSTEXPR float squareVertices[12] = {
						-1.0f,-1.0f,0.0f,
						-1.0f,1.0f,0.0f,
						1.0f,1.0f,0.0f,
						1.0f,-1.0f,0.0f
					};

					Binder b(&vao);
					vao.loadVertices(4, squareVertices, MACE__VAO_VERTICES_LOCATION, 3);
					vao.storeDataInAttributeList(4, squareTextureCoordinates, MACE__VAO_TEX_COORD_LOCATION, 2);
					vao.loadIndices(6, squareIndices);
				}

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating VAO for painter!");

				return vao;
			}

			const mc::Preprocessor& OGL33Renderer::getSSLPreprocessor() {
				if (sslPreprocessor.macroNumber() == 0) {
					if (GLEW_VERSION_3_3) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_3", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 330 core"));
					} else if (GLEW_VERSION_3_2) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_2", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 150 core"));
					} else if (GLEW_VERSION_3_1) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_1", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 140 core"));
					} else if (GLEW_VERSION_3_0) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_0", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 130 core"));
					} else if (GLEW_VERSION_2_1) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_2_1", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 120"));
					} else if (GLEW_VERSION_2_0) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_2_0", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 110"));
					} else {
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#error GLSL is not supported on this system."));
					}

					//indirection is the only way to expand macros in other macros
					//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define MACE__DEFINE_MACRO(name) if(std::strcmp("" #name ,MACE_STRINGIFY_NAME(name))){sslPreprocessor.defineMacro( Macro( #name , MACE_STRINGIFY( name ) ));}
					MACE__DEFINE_MACRO(MACE__ENTITY_DATA_LOCATION);

					MACE__DEFINE_MACRO(MACE__SCENE_ATTACHMENT_INDEX);
					MACE__DEFINE_MACRO(MACE__ID_ATTACHMENT_INDEX);

					MACE__DEFINE_MACRO(MACE__VAO_VERTICES_LOCATION);
					MACE__DEFINE_MACRO(MACE__VAO_TEX_COORD_LOCATION);
#undef MACE__DEFINE_MACRO

					sslPreprocessor.addInclude(vertexLibrary);
					sslPreprocessor.addInclude(fragmentLibrary);
					sslPreprocessor.addInclude(positionLibrary);
					sslPreprocessor.addInclude(entityLibrary);
					sslPreprocessor.addInclude(coreLibrary);
				}
				return sslPreprocessor;
			}//getSSLPreprocessor

			OGL33Painter::OGL33Painter(const GraphicsEntity * const entity) : PainterImpl(entity), renderer(dynamic_cast<OGL33Renderer*>(mc::os::getCurrentWindow()->getContext()->getRenderer().get())) {
				if (renderer == nullptr) {
					//this should never happen unless someone extended Renderer and returned a OGL33Painter for some reason...
					MACE__THROW(NullPointer, "Internal Error: OGL33Painter cant be used without a OGL33Renderer");
				}

			}

			void OGL33Painter::init() {
				renderer->loadEntityUniforms(entity);

				renderer->entityUniforms.bind();
				renderer->entityUniforms.bindForRender();

				renderer->painterUniforms.bind();
				renderer->painterUniforms.bindForRender();
			}

			void OGL33Painter::destroy() {}

			void OGL33Painter::loadSettings() {
				renderer->loadPainterUniforms(state.transformation, state.primaryColor, state.secondaryColor, state.data);
			}

			void OGL33Painter::draw(const Painter::Brush brush, const Painter::RenderType type) {
				const OGL33Renderer::RenderProtocol prot = renderer->getProtocol(entity, { brush, type });
				prot.vao.bind();
				prot.program.bind();

				if (type == Painter::RenderType::QUAD) {
					prot.vao.draw(GL_TRIANGLES);
				}
			}
		}//ogl
	}//gfx
}//mc

#if defined(_MSC_VER) && _MSC_VER >= 1400 
 //pop the disable of warning 4996 which is a useless warning in our scenario
#	pragma warning( pop ) 
#endif 