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

#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Utility/Preprocessor.h>

//we need to include algorithim for memcpy
#include <algorithm>
//cstring is for strcmp
#include <cstring>
#include <iostream>

namespace mc {
	namespace gfx {

		namespace {
			RenderQueue renderQueue = RenderQueue();
			std::vector<RenderImpl*> protocols = std::vector<RenderImpl*>();

			Size originalWidth = 0;
			Size originalHeight = 0;

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;

			//this variable is used for both ssl and Renderer. Each iteration through the queue, this is incremented. It is then passed to the shader, and the shader returns which entity was hovered over
			Index entityIndex = 0;
		}//anon namespace

		void Renderer::init(const Size width, const Size height) {
			originalWidth = width;
			originalHeight = height;

			ssl::init(width, height);
		}//init

		void Renderer::initEntity(GraphicsEntity * en, const Index protocol) {
			protocols[protocol]->initEntity(en);
		}

		void Renderer::setUp(os::WindowModule* win) {
			if( resized ) {
				int width, height;

				glfwGetFramebufferSize(win->getGLFWWindow(), &width, &height);

				gfx::Renderer::resize(width, height);

				resized = false;
			}

			ssl::setUp(win);

			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->setUp(win, &renderQueue);
			}
		}//setUp

		void Renderer::queue(GraphicsEntity * e, const Index protocol) {
			if( e == nullptr ) {
				throw NullPointerError("Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}

			pushEntity(protocol, e);
		}//queue

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(const Size width, const Size height) {
			ssl::resize(width, height);

			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->resize(width, height);
			}

			resized = false;
		}//resize

		Size Renderer::numberOfProtocols() {
			//size() returns size_t which could be larger than unsigned in on some systems, causing problems. static_cast will fix it
			return static_cast<Size>(protocols.size());
		}//numberOfProtocols

		void Renderer::tearDown(os::WindowModule* win) {
			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->tearDown(win, &renderQueue);
			}

			ssl::tearDown(win);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error destroying renderer");
		}//tearDown

		void Renderer::renderFrame(os::WindowModule* win) {
			setUp(win);
			entityIndex = 0;
			for( RenderQueue::iterator pair = renderQueue.begin(); pair != renderQueue.end(); ++pair ) {
				protocols[pair->first]->renderEntity(win, pair->second);
				++entityIndex;
			}
			tearDown(win);
		}//renderFrame

		void Renderer::checkInput(os::WindowModule* win) {
			ssl::checkInput(win);
		}//checkInput

		void Renderer::cleanEntity(GraphicsEntity * en, const Index protocol) {
			protocols[protocol]->cleanEntity(en);
		}

		void Renderer::destroy() {
			while( !protocols.empty() ) {
				RenderImpl* protocol = protocols.back();
				protocol->destroy();
				delete protocol;
				protocols.pop_back();
			}

			ssl::destroy();
		}//destroy()

		void Renderer::destroyEntity(GraphicsEntity * en, const Index protocol) {
			protocols[protocol]->destroyEntity(en);
		}

		void Renderer::clearBuffers() {
			renderQueue.clear();
		}//clearBuffers()

		void Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
			ogl::FrameBuffer::setClearColor(r, g, b, a);
		}//setRefreshColor(r,g,b,a)

		void Renderer::setRefreshColor(const Color & c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Size Renderer::getOriginalWidth() {
			return originalWidth;
		}//getOriginalWidth()

		Size Renderer::getOriginalHeight() {
			return originalHeight;
		}
		Size Renderer::getWidth() {
			return static_cast<Size>(static_cast<float>(Renderer::getOriginalWidth()) * windowRatios[0]);
		}
		Size Renderer::getHeight() {
			return static_cast<Size>(static_cast<float>(Renderer::getOriginalHeight()) * windowRatios[1]);
		}
		//getOriginalHeight()

		Size Renderer::getSamples() {
			return samples;
		}
		Vector<float, 2> Renderer::getWindowRatios() {
			return windowRatios;
		}
		//getSamples()

		void Renderer::pushEntity(Index protocol, GraphicsEntity * entity) {
			renderQueue.push_back(std::pair<Index, GraphicsEntity*>(protocol, entity));
		}//pushEntity(protocol, entity)

		void Renderer::pushProtocol(RenderImpl * protocol) {
			protocols.push_back(protocol);
		}//pushProtocol(protocol)

		//ssl
		namespace ssl {
			//constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

//how many floats in the entityData uniform sslBuffer.
#define _MACE_ENTITY_DATA_BUFFER_SIZE sizeof(float)*16
//which binding location the paintdata uniform sslBuffer should be bound to
#define _MACE_ENTITY_DATA_LOCATION 15

			namespace {
				//ssl resources
				Preprocessor sslPreprocessor = Preprocessor("");

				//fbo resources
				ogl::FrameBuffer frameBuffer = ogl::FrameBuffer();
				ogl::RenderBuffer depthBuffer = ogl::RenderBuffer();

				ogl::Texture2D sceneTexture = ogl::Texture2D();
				ogl::Texture2D idTexture = ogl::Texture2D();

				IncludeString vertexLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_vertex.glsl>
				}, "ssl_vertex");
				/**
				@todo Remove discard from shader
				*/
				IncludeString fragmentLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_frag.glsl>
				}, "ssl_frag");
				IncludeString positionLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_position.glsl>
				}, "ssl_position");
				IncludeString entityLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_entity.glsl>
				}, "ssl_entity");
				IncludeString coreLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_core.glsl>
				}, "ssl_core");

				//this function goes into the anonymous namespace because it technically doesn't belong in the ssl namespace. it should remain to this source file
				void generateFramebuffer(const Size& width, const Size& height) {

					depthBuffer.init();
					depthBuffer.bind();

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating depth buffers for renderer");

					sceneTexture.setData(nullptr, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);
					idTexture.setData(nullptr, width, height, GL_UNSIGNED_INT, GL_RED_INTEGER, GL_R32UI);
					depthBuffer.setStorage(GL_DEPTH_COMPONENT, width, height);

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting texture for renderer Z-buffers");

					//for our custom FBOs. we render using a z-sslBuffer to figure out which entity is clicked on
					frameBuffer.init();

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating FrameBuffer for the renderer");

					frameBuffer.attachTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneTexture);
					frameBuffer.attachTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, idTexture);
					frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error attaching texture to FrameBuffer for the renderer");

					Enum status;
					if( (status = frameBuffer.checkStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE ) {
						switch( status ) {
						case GL_FRAMEBUFFER_UNDEFINED:
							throw FramebufferError("GL_FRAMEBUFFER_UNDEFINED: The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist. ");
							break;
						case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
							throw FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: One of the framebuffer attachments are incomplete!");
							break;
						case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
							throw FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The framebuffer is missing at least one image");
							break;
						case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
							throw FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER. ");
							break;
						case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
							throw FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi. ");
							break;
						case GL_FRAMEBUFFER_UNSUPPORTED:
							throw FramebufferError("GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions. ");
							break;
						case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
							throw FramebufferError("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. It can also be that the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures. ");
							break;
						case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
							throw FramebufferError("GL_FRAMEBUFFER_LAYER_TARGETS: Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target. ");
							break;
						}
					}

					constexpr Enum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
					frameBuffer.setDrawBuffers(2, buffers);

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting draw buffers in FrameBuffer for the renderer");

					glViewport(0, 0, width, height);

					windowRatios[0] = static_cast<float>(originalWidth) / static_cast<float>(width);
					windowRatios[1] = static_cast<float>(originalHeight) / static_cast<float>(height);
				}//generateFrambuffer
			}//anon namespace



			void init(const Size &, const Size &) {
				sceneTexture.init();
				sceneTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				sceneTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating scene texture for renderer");

				idTexture.init();
				idTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				idTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating id texture for renderer");

				//better to access originalWidth and originalHeight directly than via a parameter.
				generateFramebuffer(originalWidth, originalHeight);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error generating framebuffer for renderer");

				//gl states
				ogl::enable(GL_BLEND);
				ogl::setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				ogl::enable(GL_MULTISAMPLE);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error enabling blending and multisampling for renderer");
			}//init

			void setUp(os::WindowModule *) {
				frameBuffer.bind();
				sceneTexture.bind();
				idTexture.bind();

				constexpr Enum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
				frameBuffer.setDrawBuffers(2, drawBuffers);

				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to set up ssl");
			}//setUp

			void tearDown(os::WindowModule * win) {
				frameBuffer.unbind();
				ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				int width, height;
				glfwGetWindowSize(win->getGLFWWindow(), &width, &height);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.getID());
				ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0);
				ogl::FrameBuffer::setDrawBuffer(GL_BACK);

				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to tear down ssl");

				glfwSwapBuffers(win->getGLFWWindow());
			}//tearDown

			void destroy() {
				depthBuffer.destroy();

				frameBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();
			}//destroy

			void bindEntity(const GraphicsEntity * entity, ogl::ShaderProgram& prog) {
				const ogl::UniformBuffer& entityData = entity->sslBuffer;

				entityData.bind();
				entityData.bindToUniformBlock(prog.getID(), "ssl_BaseEntityBuffer");
				entityData.bindForRender();

				prog.setUniform("ssl_EntityID", entityIndex + 1);
			}//bindEntity

			void bindShaderProgram(ogl::ShaderProgram & prog) {
				prog.createUniform("ssl_EntityID");
			}//bindShaderProgram

			void resize(const Size & width, const Size & height) {
				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we dont do anything

				if( width > 0 && height > 0 ) {
					ogl::setViewport(0, 0, width, height);

					depthBuffer.destroy();

					frameBuffer.destroy();
					generateFramebuffer(width, height);
				}
			}//resize

			void checkInput(os::WindowModule* win) {
				frameBuffer.bind();

				int width, height;
				glfwGetWindowSize(win->getGLFWWindow(), &width, &height);

				Index pixel = 0;
				frameBuffer.setReadBuffer(GL_COLOR_ATTACHMENT1);
				frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT1);
				ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT1);
				//it is inverted for some reason
				frameBuffer.readPixels(os::Input::getMouseX(), height - os::Input::getMouseY(), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);

				if( pixel > 0 ) {
					if(pixel > renderQueue.size()){
						throw AssertionFailedError("Internal Error: Pixel read from framebuffer is larger than the render queue!");
					}
				
					//the entity id stored is 1 plus the actual one, to differeniate from an error read (0) or an actual id. so we decrement it to get the actual index
					renderQueue[--pixel].second->hover();
				}

				frameBuffer.unbind();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to check input from framebuffer in ssl");
			}//checkInput

			void bindBuffer(ogl::UniformBuffer & buf) {
				buf.bind();
				//we set it to null, because during the actual rendering we set the data
				buf.setData(_MACE_ENTITY_DATA_BUFFER_SIZE, nullptr);
				buf.unbind();
			}//bindBuffer

			void fillBuffer(GraphicsEntity * entity) {
				if( !entity->getProperty(Entity::INIT) ) {
					throw InitializationFailedError("Entity is not initializd.");
				}

				ogl::UniformBuffer& buf = entity->sslBuffer;
				const Entity::Metrics metrics = entity->getMetrics();

				//now we set the uniform sslBuffer defining the transformations of the entity
				buf.bind();
				//holy crap thats a lot of flags. this is the fastest way to map the sslBuffer. the difference is MASSIVE. try it.
				float* mappedEntityData = static_cast<float*>(buf.mapRange(0, _MACE_ENTITY_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
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
				std::copy(&entity->opacity, &entity->opacity + sizeof(entity->opacity), mappedEntityData);
				buf.unmap();

				buf.setLocation(_MACE_ENTITY_DATA_LOCATION);
			}//fillBuffer

			std::string processShader(const std::string & shader, const GLenum& type) {
				Preprocessor shaderPreprocessor = Preprocessor(shader, getSSLPreprocessor());
				shaderPreprocessor.defineMacro(mc::Macro("__SHADER_TYPE__", std::to_string(type)));

				return shaderPreprocessor.preprocess();
			}//processShader

			const mc::Preprocessor& getSSLPreprocessor() {
				if( sslPreprocessor.macroNumber() == 0 ) {
					sslPreprocessor.defineOSMacros();
					sslPreprocessor.defineStandardMacros();

					sslPreprocessor.defineMacro(mc::Macro("__SSL__", "1"));

					//C-style casts are unsafe. Problem is that this is a C API. You must use a C-style cast in order to do this correctly.
					sslPreprocessor.defineMacro(mc::Macro("GL_VENDOR", (const char*) (glGetString(GL_VENDOR))));
					sslPreprocessor.defineMacro(mc::Macro("GL_RENDERER", (const char*) (glGetString(GL_RENDERER))));
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION", (const char*) (glGetString(GL_VERSION))));
					sslPreprocessor.defineMacro(mc::Macro("GL_SHADING_LANGUAGE_VERSION", (const char*) (glGetString(GL_SHADING_LANGUAGE_VERSION))));

					if( GLEW_VERSION_1_1 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_1", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#error GLSL is not supported on this system."));
					}
					if( GLEW_VERSION_1_2 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_2", "1"));
					}
					if( GLEW_VERSION_1_2_1 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_2_1", "1"));
					}
					if( GLEW_VERSION_1_3 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_3", "1"));
					}
					if( GLEW_VERSION_1_4 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_4", "1"));
					}
					if( GLEW_VERSION_1_5 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_5", "1"));
					}

					if( GLEW_VERSION_2_0 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_2_0", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 110"));
					}
					if( GLEW_VERSION_2_1 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_2_1", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 120"));
					}

					if( GLEW_VERSION_3_0 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_0", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_VERSION", "#version 130 core"));
					}
					if( GLEW_VERSION_3_1 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_1", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 140 core"));
					}
					if( GLEW_VERSION_3_2 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_2", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 150 core"));
					}
					if( GLEW_VERSION_3_3 ) {
						sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_3", "1"));
						sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 330 core"));
					}

					/*
					in order to define a bunch of opengl macros, we need to check if they exist, just in case this system doesnt support
					a certain macro. the following is a special macro which only defines a macro in sslPreprocessor if it is defined in
					reality
					*/

					//indirection is the only way to expand macros in other macros
#define _MACE_NAME_STRINGIFY(name) "" #name
#define _MACE_STRINGIFY(name) #name
					//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define _MACE_DEFINE_MACRO(name) if(std::strcmp("" #name ,_MACE_NAME_STRINGIFY(name))){sslPreprocessor.defineMacro( Macro( #name , _MACE_STRINGIFY( name ) ));}
					/*Shader macros*/
					_MACE_DEFINE_MACRO(GL_VERTEX_SHADER);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_ATTRIBUTES);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_INPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_OUTPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_IMAGE_UNIFORMS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS);

					_MACE_DEFINE_MACRO(GL_FRAGMENT_SHADER);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_ATTRIBUTES);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_INPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_OUTPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_IMAGE_UNIFORMS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS);

					_MACE_DEFINE_MACRO(GL_GEOMETRY_SHADER);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_ATTRIBUTES);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_INPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_IMAGE_UNIFORMS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS);

					_MACE_DEFINE_MACRO(GL_TESS_CONTROL_SHADER);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_ATTRIBUTES);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS);

					_MACE_DEFINE_MACRO(GL_TESS_EVALUATION_SHADER);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_ATTRIBUTES);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS);

					_MACE_DEFINE_MACRO(GL_COMPUTE_SHADER);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_ATTRIBUTES);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_UNIFORM_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_INPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_OUTPUT_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_IMAGE_UNIFORMS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS);

					_MACE_DEFINE_MACRO(GL_MAX_UNIFORM_BUFFER_BINDINGS);
					_MACE_DEFINE_MACRO(GL_MAX_COMBINED_UNIFORM_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS);
					_MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS);
					_MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS);
					_MACE_DEFINE_MACRO(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS);
					_MACE_DEFINE_MACRO(GL_MAX_COMBINED_ATOMIC_COUNTERS);
					_MACE_DEFINE_MACRO(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
					_MACE_DEFINE_MACRO(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS);
					_MACE_DEFINE_MACRO(GL_MAX_IMAGE_UNITS);
					_MACE_DEFINE_MACRO(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES);

					_MACE_DEFINE_MACRO(GL_FALSE);
					_MACE_DEFINE_MACRO(GL_TRUE);
					_MACE_DEFINE_MACRO(NULL);
#undef _MACE_NAME_STRINGIFY
#undef _MACE_STRINGIFY
#undef _MACE_DEFINE_MACRO

					sslPreprocessor.addInclude(vertexLibrary);
					sslPreprocessor.addInclude(fragmentLibrary);
					sslPreprocessor.addInclude(positionLibrary);
					sslPreprocessor.addInclude(entityLibrary);
					sslPreprocessor.addInclude(coreLibrary);
				}
				return sslPreprocessor;
			}//getSSLPreprocessor
#undef _MACE_ENTITY_DATA_BUFFER_SIZE
#undef _MACE_ENTITY_DATA_LOCATION
		}//ssl

		SimpleQuadRenderer::SimpleQuadRenderer(const bool ssl) : useSSL(ssl) {}

		void SimpleQuadRenderer::init(const char * vertexShader, const char * fragShader) {
			constexpr float squareTextureCoordinates[8] = {
				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,0.0f,
				1.0f,1.0f,
			};

			constexpr unsigned int squareIndices[6] = {
				0,1,3,
				1,2,3
			};

			constexpr float squareVertices[12] = {
				-1.0f,-1.0f,0.0f,
				-1.0f,1.0f,0.0f,
				1.0f,1.0f,0.0f,
				1.0f,-1.0f,0.0f
			};

			shaders2D.init();
			square.init();

			//vao loading
			if( useSSL ) {
				square.loadVertices(4, squareVertices, 15, 3);
			} else {
				square.loadVertices(4, squareVertices, 0, 3);
			}
			square.storeDataInAttributeList(4, squareTextureCoordinates, 1, 2);

			square.loadIndices(6, squareIndices);

			//shader stuff
			shaders2D.createVertex(useSSL ? ssl::processShader(vertexShader, GL_VERTEX_SHADER) : vertexShader);
			shaders2D.createFragment(useSSL ? ssl::processShader(fragShader, GL_FRAGMENT_SHADER) : fragShader);

			shaders2D.link();

			if( useSSL ) {
				ssl::bindShaderProgram(shaders2D);
			}

			ogl::checkGLError(__LINE__, __FILE__, "Failed to create simple quad renderer");
		}

		void SimpleQuadRenderer::init(const char * vertexShader, const std::string & fragShader) {
			init(vertexShader, fragShader.c_str());
		}

		void SimpleQuadRenderer::init(const std::string & vertexShader, const char * fragShader) {
			init(vertexShader.c_str(), fragShader);
		}

		void SimpleQuadRenderer::init(const std::string & vertexShader, const std::string & fragShader) {
			init(vertexShader.c_str(), fragShader.c_str());
		}

		void SimpleQuadRenderer::destroy() {
			shaders2D.destroy();
			square.destroy();
		}

		void SimpleQuadRenderer::bind() const {
			square.bind();
			shaders2D.bind();
		}

		void SimpleQuadRenderer::draw(const GraphicsEntity * en) {
			bind();

			if( useSSL ) {
				ssl::bindEntity(en, shaders2D);
			}

			square.draw();
		}

		void SimpleQuadRenderer::setShader(const ogl::ShaderProgram & shader) {
			shaders2D = shader;
		}

		ogl::ShaderProgram & SimpleQuadRenderer::getShader() {
			return shaders2D;
		}

		const ogl::ShaderProgram & SimpleQuadRenderer::getShader() const {
			return shaders2D;
		}

		void SimpleQuadRenderer::setVertexArray(const ogl::VertexArray & vertices) {
			square = vertices;
		}

		ogl::VertexArray & SimpleQuadRenderer::getVertexArray() {
			return square;
		}

		const ogl::VertexArray & SimpleQuadRenderer::getVertexArray() const {
			return square;
		}

		bool SimpleQuadRenderer::operator==(const SimpleQuadRenderer & other) const {
			return shaders2D == other.shaders2D&&square == other.square;
		}

		bool SimpleQuadRenderer::operator!=(const SimpleQuadRenderer & other) const {
			return !operator==(other);
		}

	}//gfx
}//mc

#if defined(_MSC_VER) && _MSC_VER >= 1400 
//pop the disable of warning 4996 which is a useless warning
#	pragma warning( pop ) 
#endif 
