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

#include <MACE/Graphics/OGLRenderer.h>
#include <MACE/Utility/Preprocessor.h>

//we need to include algorithim for std::copy
#include <algorithm>
//cstring is for strcmp
#include <cstring>
//std::begin and std::end
#include <iterator>

//debug purposes
#include <iostream>

namespace mc {
	namespace gfx {

		//magic constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

#define MACE__MATRIX_SIZE sizeof(float) * 4 * 4

		//how many floats in the uniform buffer
#define MACE__ENTITY_DATA_BUFFER_SIZE sizeof(float)*16
		//which binding location the uniform buffer goes to
#define MACE__ENTITY_DATA_LOCATION 0

#define MACE__PAINTER_DATA_BUFFER_SIZE MACE__MATRIX_SIZE + (2 * sizeof(Color))
#define MACE__PAINTER_DATA_LOCATION 1

#define MACE__SCENE_ATTACHMENT_INDEX 0
#define MACE__ID_ATTACHMENT_INDEX 1

		//location of vbo of vertices and texture coordinates in the vao for each protocl
#define MACE__VAO_VERTICES_LOCATION 0
#define MACE__VAO_TEX_COORD_LOCATION 1

		IncludeString vertexLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/mc_vertex.glsl>
		}, "mc_vertex");
		/**
		@todo Remove discard from shader
		*/
		IncludeString fragmentLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/mc_frag.glsl>
		}, "mc_frag");
		IncludeString positionLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/mc_position.glsl>
		}, "mc_position");
		IncludeString entityLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/mc_entity.glsl>
		}, "mc_entity");
		IncludeString coreLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/mc_core.glsl>
		}, "mc_core");

		GLRenderer::GLRenderer() : sslPreprocessor(""), frameBuffer(), depthBuffer(), sceneTexture(), idTexture(), clearColor(Colors::BLACK) {}

		void GLRenderer::onResize(const Size width, const Size height) {
			//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we make it 1 instead

			ogl::setViewport(0, 0, width == 0 ? 1 : width, height == 0 ? 1 : height);

			depthBuffer.destroy();

			frameBuffer.destroy();

			generateFramebuffer(width == 0 ? 1 : width, height == 0 ? 1 : height);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error resizing framebuffer for renderer");
		}

		void GLRenderer::onInit(const Size width, const Size height) {
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

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error enabling blending and multisampling for renderer");

			generateFramebuffer(width, height);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error generating framebuffer for renderer");

			entityUniforms.init();
			entityUniforms.bind();
			//we set it to null, because during the actual rendering we set the data
			entityUniforms.setData(MACE__ENTITY_DATA_BUFFER_SIZE, nullptr);

			entityUniforms.setLocation(MACE__ENTITY_DATA_LOCATION);

			painterUniforms.init();
			painterUniforms.bind();
			painterUniforms.setData(MACE__PAINTER_DATA_BUFFER_SIZE, nullptr);

			painterUniforms.setLocation(MACE__PAINTER_DATA_LOCATION);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating UniformBuffer for renderer");
		}

		void GLRenderer::onSetUp(os::WindowModule *) {
			//frameBuffer.unbind();

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

			constexpr Enum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
				GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX };
			frameBuffer.setDrawBuffers(2, drawBuffers);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to set up ssl");

		}

		void GLRenderer::onTearDown(os::WindowModule * win) {
			frameBuffer.unbind();

			ogl::FrameBuffer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			int width, height;
			glfwGetWindowSize(win->getGLFWWindow(), &width, &height);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.getID());
			ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
			ogl::FrameBuffer::setDrawBuffer(GL_BACK);

			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to tear down ssl");

			glfwSwapBuffers(win->getGLFWWindow());
		}

		void GLRenderer::onDestroy() {
			depthBuffer.destroy();

			frameBuffer.destroy();

			sceneTexture.destroy();
			idTexture.destroy();

			entityUniforms.destroy();
			painterUniforms.destroy();

			for (std::map<RenderSettings, RenderProtocol>::iterator iter = protocols.begin(); iter != protocols.end(); ++iter) {
				iter->second.program.destroy();
				iter->second.vao.destroy();
			}

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error destroying renderer");
		}

		void GLRenderer::onQueue(GraphicsEntity *) {}

		void GLRenderer::setRefreshColor(const float r, const float g, const float b, const float a) {
			clearColor = Color(r, g, b, a);
		}

		GraphicsEntity * GLRenderer::getEntityAt(const int x, const int y) {
			frameBuffer.bind();

			Index pixel = 0;
			frameBuffer.setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
			frameBuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
			ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
			//it is inverted for some reason
			frameBuffer.readPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);

			if (pixel > 0) {
				if (pixel > renderQueue.size()) {
					MACE__THROW(AssertionFailed, "Internal Error: Pixel read from framebuffer is larger than the render queue!");
				}

				return renderQueue[--pixel];
			}

			frameBuffer.unbind();

			return nullptr;
		}

		std::shared_ptr<PainterImpl> GLRenderer::getPainter(const GraphicsEntity * const entity) const {
			return std::shared_ptr<PainterImpl>(new GLPainter(entity));
		}

		void GLRenderer::generateFramebuffer(const Size& width, const Size& height) {
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

			constexpr Enum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			frameBuffer.setDrawBuffers(2, buffers);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting draw buffers in FrameBuffer for the renderer");

			glViewport(0, 0, width, height);

			windowRatios[0] = static_cast<float>(originalWidth) / static_cast<float>(width);
			windowRatios[1] = static_cast<float>(originalHeight) / static_cast<float>(height);
		}

		std::string GLRenderer::processShader(const std::string & shader) {
			Preprocessor shaderPreprocessor = Preprocessor(shader, getSSLPreprocessor());
			return shaderPreprocessor.preprocess();
		}

		void GLRenderer::loadEntityUniforms(const GraphicsEntity * const entity) {
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

		void GLRenderer::loadPainterUniforms(TransformMatrix transform, Color prim, Color second) {
			float modelMatrix[16] = {};
			transform.get().flattenTransposed(modelMatrix);

			float color[4] = {};
			prim.flatten(color);

			//now we set the uniforms defining the transformations of the entity
			painterUniforms.bind();
			//holy crap thats a lot of flags. this is the fastest way to map the sslBuffer. the difference is MASSIVE. try it.
			float* mappedEntityData = static_cast<float*>(painterUniforms.mapRange(0, MACE__PAINTER_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
			std::copy(std::begin(modelMatrix), std::end(modelMatrix), mappedEntityData);
			mappedEntityData += 4 * 4;//pointer arithmetic!
			std::copy(std::begin(color), std::end(color), mappedEntityData);
			mappedEntityData += 4;
			second.flatten(color);
			std::copy(std::begin(color), std::end(color), mappedEntityData);
			painterUniforms.unmap();
		}

		GLRenderer::RenderProtocol& GLRenderer::getProtocol(const GraphicsEntity* const entity, const RenderSettings settings) {
			if (protocols.find(settings) == protocols.end()) {
				GLRenderer::RenderProtocol prot{};
				prot.program = getShadersForSettings(settings);
				prot.vao = getVAOForSettings(settings);
				protocols[settings] = prot;
			}

			loadEntityUniforms(entity);

			protocols[settings].program.bind();
			protocols[settings].program.setUniform("mc_EntityID", entity->getID());

			return protocols[settings];
		}

		ogl::ShaderProgram GLRenderer::getShadersForSettings(const RenderSettings settings) {
			ogl::ShaderProgram program;
			program.init();

			if (settings.type == GLRenderer::RenderType::QUAD) {
				program.createVertex(processShader({
#include <MACE/Graphics/Shaders/quad.v.glsl>
				}));
			}

			if (settings.brush == GLRenderer::Brush::TEXTURE) {
				program.createFragment(processShader({
#include <MACE/Graphics/Shaders/texture.f.glsl>
				}));
			}

			program.link();

			program.createUniform("mc_EntityID");
			entityUniforms.bindToUniformBlock(program, "mc_BaseEntityBuffer");
			painterUniforms.bindToUniformBlock(program, "mc_PainterSettingsBuffer");
			return program;
		}

		ogl::VertexArray GLRenderer::getVAOForSettings(const RenderSettings settings) {
			ogl::VertexArray vao;
			vao.init();

			if (settings.type == GLRenderer::RenderType::QUAD) {
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

				vao.bind();
				vao.loadVertices(4, squareVertices, MACE__VAO_VERTICES_LOCATION, 3);
				vao.storeDataInAttributeList(4, squareTextureCoordinates, MACE__VAO_TEX_COORD_LOCATION, 2);
				vao.loadIndices(6, squareIndices);
			}
			return vao;
		}

		const mc::Preprocessor& GLRenderer::getSSLPreprocessor() {
			if (sslPreprocessor.macroNumber() == 0) {
				sslPreprocessor.defineOSMacros();
				sslPreprocessor.defineStandardMacros();

				sslPreprocessor.defineMacro(mc::Macro("__SSL__", "1"));

				//C-style casts are unsafe. Problem is that this is a C API. You must use a C-style cast in order to do this correctly.
				sslPreprocessor.defineMacro(mc::Macro("GL_VENDOR", (const char*)(glGetString(GL_VENDOR))));
				sslPreprocessor.defineMacro(mc::Macro("GL_RENDERER", (const char*)(glGetString(GL_RENDERER))));
				sslPreprocessor.defineMacro(mc::Macro("GL_VERSION", (const char*)(glGetString(GL_VERSION))));
				sslPreprocessor.defineMacro(mc::Macro("GL_SHADING_LANGUAGE_VERSION", (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION))));

				if (GLEW_VERSION_1_1) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_1", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#error GLSL is not supported on this system."));
				}
				if (GLEW_VERSION_1_2) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_2", "1"));
				}
				if (GLEW_VERSION_1_2_1) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_2_1", "1"));
				}
				if (GLEW_VERSION_1_3) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_3", "1"));
				}
				if (GLEW_VERSION_1_4) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_4", "1"));
				}
				if (GLEW_VERSION_1_5) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_1_5", "1"));
				}

				if (GLEW_VERSION_2_0) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_2_0", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 110"));
				}
				if (GLEW_VERSION_2_1) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_2_1", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 120"));
				}

				if (GLEW_VERSION_3_0) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_0", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_VERSION", "#version 130 core"));
				}
				if (GLEW_VERSION_3_1) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_1", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 140 core"));
				}
				if (GLEW_VERSION_3_2) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_2", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 150 core"));
				}
				if (GLEW_VERSION_3_3) {
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION_3_3", "1"));
					sslPreprocessor.defineMacro(mc::Macro("SSL_GL_VERSION_DECLARATION", "#version 330 core"));
				}

				/*
				in order to define a bunch of opengl macros, we need to check if they exist, just in case this system doesnt support
				a certain macro. the following is a special macro which only defines a macro in sslPreprocessor if it is defined in
				reality
				*/

				//indirection is the only way to expand macros in other macros
				//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define MACE__DEFINE_MACRO(name) if(std::strcmp("" #name ,MACE_STRINGIFY_NAME(name))){sslPreprocessor.defineMacro( Macro( #name , MACE_STRINGIFY( name ) ));}
				/*Shader macros*/
				MACE__DEFINE_MACRO(GL_VERTEX_SHADER);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_ATTRIBUTES);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_INPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_OUTPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_IMAGE_UNIFORMS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS);

				MACE__DEFINE_MACRO(GL_FRAGMENT_SHADER);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_ATTRIBUTES);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_INPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_OUTPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_IMAGE_UNIFORMS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS);

				MACE__DEFINE_MACRO(GL_GEOMETRY_SHADER);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_ATTRIBUTES);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_INPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_IMAGE_UNIFORMS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS);

				MACE__DEFINE_MACRO(GL_TESS_CONTROL_SHADER);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_ATTRIBUTES);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS);

				MACE__DEFINE_MACRO(GL_TESS_EVALUATION_SHADER);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_ATTRIBUTES);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS);

				MACE__DEFINE_MACRO(GL_COMPUTE_SHADER);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_ATTRIBUTES);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_UNIFORM_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_INPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_OUTPUT_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_IMAGE_UNIFORMS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS);

				MACE__DEFINE_MACRO(GL_MAX_UNIFORM_BUFFER_BINDINGS);
				MACE__DEFINE_MACRO(GL_MAX_COMBINED_UNIFORM_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS);
				MACE__DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS);
				MACE__DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS);
				MACE__DEFINE_MACRO(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS);
				MACE__DEFINE_MACRO(GL_MAX_COMBINED_ATOMIC_COUNTERS);
				MACE__DEFINE_MACRO(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
				MACE__DEFINE_MACRO(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS);
				MACE__DEFINE_MACRO(GL_MAX_IMAGE_UNITS);
				MACE__DEFINE_MACRO(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES);

				MACE__DEFINE_MACRO(GL_FALSE);
				MACE__DEFINE_MACRO(GL_TRUE);
				MACE__DEFINE_MACRO(NULL);

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

		GLPainter::GLPainter(const GraphicsEntity * const entity) : PainterImpl(entity), renderer(dynamic_cast<GLRenderer*>(getRenderer())) {
			if (renderer == nullptr) {
				//this should never happen unless someone extended Renderer and returned a GLPainter for some reason...
				MACE__THROW(NullPointer, "Internal Error: GLPainter cant be used without a GLRenderer");
			}

		}

		void GLPainter::init() {
			renderer->entityUniforms.bind();
			renderer->entityUniforms.bindForRender();

			renderer->painterUniforms.bind();
			renderer->painterUniforms.bindForRender();
		}

		void GLPainter::destroy() {}

		void GLPainter::loadSettings(TransformMatrix transform, Color prim, Color second) {
			renderer->loadPainterUniforms(transform, prim, second);
		}

		void GLPainter::drawImage(const ColorAttachment & img, const float x, const float y, const float w, const float h) {
			loadSettings(transformation, img.getPaint(), secondary);

			GLRenderer::RenderProtocol prot = renderer->getProtocol(entity, { GLRenderer::Brush::TEXTURE, GLRenderer::RenderType::QUAD });
			prot.vao.bind();
			prot.program.bind();

			img.bind();

			prot.vao.draw(GL_TRIANGLES);
		}
	}//gfx
}//mc

#if defined(_MSC_VER) && _MSC_VER >= 1400 
 //pop the disable of warning 4996 which is a useless warning in our scenario
#	pragma warning( pop ) 
#endif 