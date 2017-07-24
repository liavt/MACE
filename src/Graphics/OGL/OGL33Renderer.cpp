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
			//how many floats in the uniform buffer
#define MACE__ENTITY_DATA_BUFFER_SIZE sizeof(float) * 24
		//which binding location the uniform buffer goes to
#define MACE__ENTITY_DATA_LOCATION 0
#define MACE__ENTITY_DATA_USAGE GL_DYNAMIC_DRAW
			//the definition is later stringified. cant be a string because this gets added to the shader.
#define MACE__ENTITY_DATA_NAME _mc_EntityData

#define MACE__PAINTER_DATA_BUFFER_SIZE sizeof(float) * 40
#define MACE__PAINTER_DATA_LOCATION 1
#define MACE__PAINTER_DATA_USAGE GL_DYNAMIC_DRAW
#define MACE__PAINTER_DATA_NAME _mc_PainterData

#define MACE__SCENE_ATTACHMENT_INDEX 0
#define MACE__ID_ATTACHMENT_INDEX 1

			OGL33Renderer::OGL33Renderer() {}

			void OGL33Renderer::onInit(gfx::WindowModule*) {
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
					gfx::ogl::checkGLError(__LINE__, __FILE__, "Internal Error: This should be ignored silently, it is a bug with glew");
				} catch (...) {
					//glew sometimes throws errors that can be ignored (GL_INVALID_ENUM)
					//see https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library (section GLEW) saying to ignore a GLEW error
				}

#ifdef MACE_DEBUG
				std::cout << "OpenGL Info:" << std::endl;
				std::cout << "Version: " << std::endl << "\t" << glGetString(GL_VERSION) << std::endl;
				std::cout << "Vendor: " << std::endl << "\t" << glGetString(GL_VENDOR) << std::endl;
				std::cout << "Renderer: " << std::endl << "\t" << glGetString(GL_RENDERER) << std::endl;
				std::cout << "Shader version: " << std::endl << "\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
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

			void OGL33Renderer::onSetUp(gfx::WindowModule *) {
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

				MACE_CONSTEXPR const Enum drawBuffers[] = {
					GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
					GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX
				};
				frameBuffer.setDrawBuffers(2, drawBuffers);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to set up renderer");

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
			}

			void OGL33Renderer::onResize(gfx::WindowModule*, const Size width, const Size height) {
				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we make it 1 instead

				depthBuffer.destroy();

				frameBuffer.destroy();

				ogl::setViewport(0, 0, width == 0 ? 1 : width, height == 0 ? 1 : height);

				generateFramebuffer(width == 0 ? 1 : width, height == 0 ? 1 : height);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error resizing framebuffer for renderer");
			}

			void OGL33Renderer::onDestroy() {
				depthBuffer.destroy();

				frameBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();

				entityUniforms.destroy();
				painterUniforms.destroy();

				for (auto iter = protocols.begin(); iter != protocols.end(); ++iter) {
					iter->second->program.destroy();
				}

				protocols.clear();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error destroying OpenGL 3.3 renderer");
				os::checkError(__LINE__, __FILE__, "Internal Error: Error destroying OpenGL 3.3 renderer");
			}

			void OGL33Renderer::onQueue(GraphicsEntity *) {}

			void OGL33Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
				clearColor = Color(r, g, b, a);
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

			std::shared_ptr<PainterImpl> OGL33Renderer::createPainterImpl(const GraphicsEntity * const entity) {
				return std::shared_ptr<PainterImpl>(new OGL33Painter(this, entity));
			}

			std::string OGL33Renderer::processShader(const std::string & shader) {
				Preprocessor preprocessor = Preprocessor(shader, getShaderPreprocessor());
				return preprocessor.preprocess();
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

			void OGL33Renderer::loadPainterUniforms(const Painter::State& state) {
				float color[4] = {};

				//now we set the uniforms defining the transformations of the entity
				Binder b(&painterUniforms);
				//orphan the buffer (this is required for intel gpus)
				painterUniforms.setData(MACE__PAINTER_DATA_BUFFER_SIZE, nullptr, MACE__PAINTER_DATA_USAGE);
				//holy crap thats a lot of flags. this is the fastest way to map the buffer. basically it specifies we are writing, and to invalidate the old buffer and overwrite any changes.
				float* mappedEntityData = static_cast<float*>(painterUniforms.mapRange(0, MACE__PAINTER_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
				
std::copy(state.transformation.translation.begin(), state.transformation.translation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(state.transformation.rotation.begin(), state.transformation.rotation.end(), mappedEntityData);
				mappedEntityData += 4;
				std::copy(state.transformation.scaler.begin(), state.transformation.scaler.end(), mappedEntityData);
				mappedEntityData += 4;

				std::copy(state.data.begin(), state.data.end(), mappedEntityData);
				mappedEntityData += 4;

				state.foregroundColor.flatten(color);
				std::copy(std::begin(color), std::end(color), mappedEntityData);
				mappedEntityData += 4;
				std::copy(state.foregroundTransform.begin(), state.foregroundTransform.end(), mappedEntityData);
				mappedEntityData += 4;

				state.backgroundColor.flatten(color);
				std::copy(std::begin(color), std::end(color), mappedEntityData);
				mappedEntityData += 4;
				std::copy(state.backgroundTransform.begin(), state.backgroundTransform.end(), mappedEntityData);
				mappedEntityData += 4;

				state.maskColor.flatten(color);
				std::copy(std::begin(color), std::end(color), mappedEntityData);
				mappedEntityData += 4;
				std::copy(state.maskTransform.begin(), state.maskTransform.end(), mappedEntityData);
				mappedEntityData += 4;

				painterUniforms.unmap();
			}

			OGL33Renderer::RenderProtocol& OGL33Renderer::getProtocol(const GraphicsEntity* const entity, const std::pair<Enums::Brush, Enums::RenderType> settings) {
				auto protocol = protocols.find(settings);
				if (protocol == protocols.end()) {
					std::unique_ptr<OGL33Renderer::RenderProtocol> prot = std::unique_ptr<OGL33Renderer::RenderProtocol>(new OGL33Renderer::RenderProtocol());
					prot->program = createShadersForSettings(settings);

					protocols.insert(std::pair<std::pair<Enums::Brush, Enums::RenderType>, std::unique_ptr<OGL33Renderer::RenderProtocol>>(settings, std::move(prot)));

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

			ogl::ShaderProgram OGL33Renderer::createShadersForSettings(const std::pair<Enums::Brush, Enums::RenderType>& settings) {
				ogl::ShaderProgram program;
				program.init();

				if (settings.second == Enums::RenderType::STANDARD) {
					program.createVertex(processShader({
	#include <MACE/Graphics/OGL/Shaders/RenderTypes/standard.v.glsl>
					}));
				} else {
					MACE__THROW(BadFormat, "OpenGL 3.3 Renderer: Unsupported render type: " + std::to_string(static_cast<unsigned int>(settings.second)));
				}

				if (settings.first == Enums::Brush::COLOR) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/color.f.glsl>
					}));

					program.link();
				} else if (settings.first == Enums::Brush::TEXTURE) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/texture.f.glsl>
					}));

					program.link();

					program.bind();

					program.createUniform("tex");

					program.setUniform("tex", static_cast<int>(Enums::TextureSlot::FOREGROUND));
				} else if (settings.first == Enums::Brush::MASK) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/mask.f.glsl>
					}));

					program.link();

					program.bind();

					program.createUniform("tex");
					program.createUniform("mask");

					//binding the samplers
					program.setUniform("tex", static_cast<int>(Enums::TextureSlot::FOREGROUND));
					program.setUniform("mask", static_cast<int>(Enums::TextureSlot::MASK));
				} else if (settings.first == Enums::Brush::MASKED_BLEND) {
					program.createFragment(processShader({
	#include <MACE/Graphics/OGL/Shaders/Brushes/masked_blend.f.glsl>
					}));

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

				program.createUniform("mc_EntityID");
				entityUniforms.bindToUniformBlock(program, MACE_STRINGIFY_DEFINITION(MACE__ENTITY_DATA_NAME));
				painterUniforms.bindToUniformBlock(program, MACE_STRINGIFY_DEFINITION(MACE__PAINTER_DATA_NAME));

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating shader program for painter!");
				return program;
			}

			const mc::Preprocessor& OGL33Renderer::getShaderPreprocessor() {
				if (shaderPreprocessor.macroNumber() == 0) {
					shaderPreprocessor.defineMacro(mc::Macro("MACE__GL_VERSION_DECLARATION", "#version 330 core"));
					shaderPreprocessor.defineMacro(mc::Macro("MACE__UNIFORM_BUFFER", "layout(std140) uniform"));

					//indirection is the only way to expand macros in other macros
					//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define MACE__DEFINE_SHADER_MACRO(name) if(std::strcmp("" #name ,MACE_STRINGIFY_NAME(name))){shaderPreprocessor.defineMacro( Macro( #name , MACE_STRINGIFY( name ) ));}
					//this macro takes in a C++ macro and converts it into a mc::Macro class for the shaderPreprocessor
					MACE__DEFINE_SHADER_MACRO(MACE__SCENE_ATTACHMENT_INDEX);
					MACE__DEFINE_SHADER_MACRO(MACE__ID_ATTACHMENT_INDEX);

					MACE__DEFINE_SHADER_MACRO(MACE__VAO_DEFAULT_VERTICES_LOCATION);
					MACE__DEFINE_SHADER_MACRO(MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION);

					MACE__DEFINE_SHADER_MACRO(MACE__ENTITY_DATA_NAME);
					MACE__DEFINE_SHADER_MACRO(MACE__PAINTER_DATA_NAME);
#undef MACE__DEFINE_SHADER_MACRO

					shaderPreprocessor.addInclude(vertexLibrary);
					shaderPreprocessor.addInclude(fragmentLibrary);
					shaderPreprocessor.addInclude(positionLibrary);
					shaderPreprocessor.addInclude(entityLibrary);
					shaderPreprocessor.addInclude(coreLibrary);
				}
				return shaderPreprocessor;
			}//getSSLPreprocessor

			OGL33Painter::OGL33Painter(OGL33Renderer* const r, const GraphicsEntity * const entity) : PainterImpl(entity), renderer(r) {}

			void OGL33Painter::init() {
				renderer->loadEntityUniforms(entity);

				renderer->entityUniforms.bind();
				renderer->entityUniforms.bindForRender();

				renderer->painterUniforms.bind();
				renderer->painterUniforms.bindForRender();
			}

			void OGL33Painter::destroy() {}

			void OGL33Painter::loadSettings(const Painter::State& state) {
				renderer->loadPainterUniforms(state);
			}

			void OGL33Painter::draw(const Model& m, const Enums::Brush brush, const Enums::RenderType type) {
				const OGL33Renderer::RenderProtocol& prot = renderer->getProtocol(entity, { brush, type });
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