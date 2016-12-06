/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/GraphicsConstants.h>
#include <MACE/Graphics/Buffer.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Utility/Preprocessor.h>
//we need to include cstring for memcpy
#include <cstring>

namespace mc {
	namespace gfx {

		int RenderImpl::index = -1;


		namespace {
			RenderQueue renderQueue = RenderQueue();
			std::vector<RenderImpl*> protocols = std::vector<RenderImpl*>();

			Size originalWidth = 0;
			Size originalHeight = 0;

			//this variable is used for both ssl and Renderer. Each iteration through the queue, this is incremented. It is then passed to the shader, and the shader returns which entity was hovered over
			Index entityIndex = 0;
		}//anon namespace

		void Renderer::init(const Size width, const Size height) {
			originalWidth = width;
			originalHeight = height;

			ssl::init(width, height);
		}//init
		void Renderer::setUp(os::WindowModule* win) {
			ssl::setUp(win);

			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->setUp(win, &renderQueue);
			}
		}//setUp
		void Renderer::resize(const Size width, const Size height) {
			glViewport(0, 0, width, height);

			ssl::resize(width, height);

			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->resize(width, height);
			}
		}
		Size Renderer::numberOfProtocols() {
			return protocols.size();
		}
		//tearDown
		void Renderer::tearDown(os::WindowModule* win) {
			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->tearDown(win, &renderQueue);
			}

			ssl::tearDown(win);
		}//tearDown
		//resize
		void Renderer::renderFrame(os::WindowModule* win) {
			setUp(win);
			for( RenderQueue::iterator pair = renderQueue.begin(); pair != renderQueue.end(); ++pair ) {
				GraphicsEntity* en = pair->second;
				en->setProperty(Entity::HOVERED, false);

				entityIndex = pair - renderQueue.begin();
				protocols[pair->first]->render(win, en);
			}
			tearDown(win);
			renderQueue.clear();
		}//renderFrame
		void Renderer::destroy() {
			while( !protocols.empty() ) {
				RenderImpl* protocol = protocols.back();
				protocol->destroy();
				delete protocol;
				protocols.pop_back();
			}

			ssl::destroy();
		}//destroy()

		void Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
			glClearColor(r, g, b, a);
		}//setRefreshColor(r,g,b,a)

		void Renderer::setRefreshColor(const Color & c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Size Renderer::getOriginalWidth() {
			return originalWidth;
		}//getOriginalWidth()

		Size Renderer::getOriginalHeight() {
			return originalHeight;
		}//getOriginalHeight()

		void Renderer::pushEntity(Index protocol, GraphicsEntity * entity) {
			renderQueue.push_back(std::pair<Index, GraphicsEntity*>(protocol, entity));
		}//pushEntity(protocol, entity)

		void Renderer::pushProtocol(RenderImpl * protocol) {
			protocols.push_back(protocol);
		}//pushProtocol(protocol)



		RenderImpl::RenderImpl() {}//RenderImpl()

		//ssl
		namespace ssl {
			//constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

			//how many floats in the windowData uniform buffer
#define MACE_WINDOW_DATA_BUFFER_SIZE 4
//which binding location the windowData uniform buffer should be bound to
#define MACE_WINDOW_DATA_LOCATION 0

//how many floats in the paintData uniform buffer
#define MACE_PAINT_DATA_BUFFER_SIZE 6
//which binding location the paintdata uniform buffer should be bound to
#define MACE_PAINT_DATA_LOCATION 1

//how many floats in the entityData uniform buffer. Needs to be multiplied by sizeof(GLfloat)
#define MACE_ENTITY_DATA_BUFFER_SIZE 52
//which binding location the paintdata uniform buffer should be bound to
#define MACE_ENTITY_DATA_LOCATION 2

//ssl resources
//preprocessor
			namespace {
				Preprocessor sslPreprocessor = Preprocessor("");

				mc::IncludeString vertexLibrary = mc::IncludeString({
				#	include <MACE/Graphics/Shaders/include/ssl_vertex.glsl>
				}, "ssl_vertex");

				mc::IncludeString fragmentLibrary = mc::IncludeString({
				#	include <MACE/Graphics/Shaders/include/ssl_frag.glsl>
				}, "ssl_frag");
				mc::IncludeString colorLibrary = mc::IncludeString({
				#	include <MACE/Graphics/Shaders/include/ssl_color.glsl>
				}, "ssl_color");
				mc::IncludeString positionLibrary = mc::IncludeString({
				#	include <MACE/Graphics/Shaders/include/ssl_position.glsl>
				}, "ssl_position");
				mc::IncludeString mouseLibrary = mc::IncludeString({
				#	include <MACE/Graphics/Shaders/include/ssl_window.glsl>
				}, "ssl_window");
				mc::IncludeString sslLibrary = mc::IncludeString({
				#	include <MACE/Graphics/Shaders/include/ssl.glsl>
				}, "ssl");

				//ssl buffer objects
				UniformBuffer windowData = UniformBuffer();
				UniformBuffer paintData = UniformBuffer();

				//fbo resources
				FrameBuffer frameBuffer = FrameBuffer();
				RenderBuffer depthBuffer = RenderBuffer();

				Texture sceneTexture = Texture();
				Texture idTexture = Texture();

				Index protocol;
				Image scene = Image();

				//this function goes into the anonymous namespace because it technically doesn't belong in the ssl namespace. it should remain to this source file
				void generateFramebuffer(const Size& width, const Size& height) {

					sceneTexture.setData(NULL, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);

					idTexture.setData(NULL, width, height, GL_UNSIGNED_INT, GL_RED_INTEGER, GL_R32UI);

					depthBuffer.init();
					depthBuffer.bind();
					depthBuffer.setStorage(GL_DEPTH_COMPONENT, width, height);

					//for our custom FBO
					frameBuffer.init();
					frameBuffer.attachTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneTexture.getID(), 0);
					frameBuffer.attachTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, idTexture.getID(), 0);
					frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

					GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
					frameBuffer.setDrawBuffers(2, buffers);

					if( frameBuffer.checkStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
						throw InitializationError("Error initializing framebuffer! This GPU may be unsupported!");
					}

					checkGLError();

					glViewport(0, 0, width, height);
				}//generateFrambuffer
			}//anon namespace



			void init(const Size &, const Size &) {
				paintData.init();
				windowData.init();

				windowData.setLocation(MACE_WINDOW_DATA_LOCATION);
				windowData.bind();
				float defaultWindowData[MACE_WINDOW_DATA_BUFFER_SIZE] = { static_cast<float>(originalWidth), static_cast<float>(originalHeight),static_cast<float>(originalWidth),static_cast<float>(originalHeight) };
				windowData.setData(sizeof(GLfloat)*MACE_WINDOW_DATA_BUFFER_SIZE, defaultWindowData);
				windowData.unbind();

				paintData.setLocation(MACE_PAINT_DATA_LOCATION);
				paintData.bind();
				//we set it to null, because during the actual rendering we set the data
				paintData.setData(sizeof(GLfloat)*MACE_PAINT_DATA_BUFFER_SIZE, nullptr, GL_STREAM_DRAW);
				paintData.unbind();

				sceneTexture.init();
				sceneTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				sceneTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				idTexture.init();
				idTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				idTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				//better to access originalWidth and originalHeigh directly than via a parameter.
				generateFramebuffer(originalWidth, originalHeight);

				scene.setHeight(1);
				scene.setWidth(1);
				scene.setX(0.5f);
				scene.setY(0.5f);
				scene.setProperty(Entity::STRETCH_X, true);
				scene.setProperty(Entity::STRETCH_Y, true);
				scene.setTexture(sceneTexture);

				Renderer::registerProtocol<Entity2D>();
				protocol = RenderProtocol<Entity2D>::index;

				scene.init();

				//gl states
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				checkGLError();
			}//init

			void setUp(os::WindowModule *) {
				frameBuffer.bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}//setUp

			void tearDown(os::WindowModule * win) {
				int mouseX, mouseY;

				{
					//we want mouseX and mouseY as floats, so we temporarly create some doubles (hence the {}) and then we cast it float.
					double tempMouseX, tempMouseY;

					glfwGetCursorPos(win->getGLFWWindow(), &tempMouseX, &tempMouseY);

					mouseX = static_cast<int>(mc::math::floor(tempMouseX));
					mouseY = static_cast<int>(mc::math::floor(tempMouseY));
				}

				Index pixel = 0;
				glReadBuffer(GL_COLOR_ATTACHMENT1);
				glReadPixels(mouseX, mouseY, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);

				if( pixel > 0 ) {
					//the entity id stored is 1 plus the actual one, to differeniate from an error read (0) or an actual id. so we decrement it to get the actual inex
					--pixel;
					renderQueue[pixel].second->setProperty(Entity::HOVERED, true);
				}

				frameBuffer.unbind();

				scene.clean();

				protocols[protocol]->render(win, &scene);
			}//tearDown

			void destroy() {
				windowData.destroy();
				paintData.destroy();

				depthBuffer.destroy();
				frameBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();

				scene.destroy();
			}//destroy

			void bindEntity(const GraphicsEntity * entity, ShaderProgram& prog) {
				const Texture& tex = entity->getTexture();

				paintData.bind();
				const Color& paint = tex.getPaint();
				const float data[6] = { paint.r,paint.g,paint.b,paint.a,tex.getOpacity(), static_cast<float>(entityIndex + 1) };
				paintData.setData(sizeof(data), data, GL_STATIC_DRAW);
				paintData.bindForRender();

				//if we use the non const version, it will make it dirty again. we dont want this function to make it dirty.
				UniformBuffer& entityData = const_cast<UniformBuffer&>(entity->getBuffer());

				entityData.setLocation(MACE_ENTITY_DATA_LOCATION);
				entityData.bind();
				entityData.bindToUniformBlock(prog.getProgramID(), "ssl_BaseEntityBuffer");
				fillBuffer(entityData, entity);
				entityData.bindForRender();

				windowData.bindForRender();

				tex.bind();
			}//bindEntity

			void bindShaderProgram(ShaderProgram & prog) {
				windowData.bindToUniformBlock(prog.getProgramID(), "ssl_WindowData");
				paintData.bindToUniformBlock(prog.getProgramID(), "ssl_PaintData");
			}//bindShaderProgram

			void resize(const Size & width, const Size & height) {
				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we dont do anything
				if( width != 0 && height != 0 ) {
					windowData.bind();
					float newSize[2] = { static_cast<float>(width),static_cast<float>(height) };
					windowData.setDataRange(sizeof(float) * 2, sizeof(float) * 2, newSize);
					windowData.unbind();

					depthBuffer.destroy();
					frameBuffer.destroy();

					generateFramebuffer(width, height);
				}
			}//resize

			void bindBuffer(UniformBuffer & buf) {
				buf.bind();
				//we set it to null, because during the actual rendering we set the data
				buf.setData(sizeof(GLfloat)*MACE_ENTITY_DATA_BUFFER_SIZE, nullptr);
				buf.unbind();
			}

			void fillBuffer(UniformBuffer & buf, const Entity * entity) {
				if( !entity->getProperty(Entity::INIT) ) {
					throw InitializationError("Internal error: Entity is not initializd.");
				}

				//setting all of these values is quite slow, need to change it
				const TransformMatrix& transform = entity->getTransformation();

				//setting uniform costs quite a bit of performance when done constantly. We cache the current setting and only change it if its different

				Vector<float, 3> inheritedTranslation = { 0,0,0 };
				Vector<float, 3> inheritedScale = { 1,1,1 };
				Matrix<float, 4, 4> inheritedRotation = math::identity<float, 4>();

				if( entity->hasParent() ) {
					const Entity* const parent = entity->getParent();
					const TransformMatrix& parentTransform = parent->getTransformation();

					inheritedTranslation = parentTransform.translation;
					inheritedScale = parentTransform.scaler;
					inheritedRotation = math::rotate(parentTransform.rotation);
				}

				const float stretch_x = entity->getProperty(Entity::STRETCH_X) ? 1.0f : 0.0f;
				const float stretch_y = entity->getProperty(Entity::STRETCH_Y) ? 1.0f : 0.0f;

				//the following are containers for the flatten() call
				float flattenedData[16];

				//now we set the uniform buffer defining the transformations of the entity
				buf.bind();
				//holy crap thats a lot of flags. this is the fastest way to map the buffer. the difference is MASSIVE. try it.
				float* mappedEntityData = static_cast<float*>(buf.mapRange(0, sizeof(GLfloat)*MACE_ENTITY_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));//we need to cast it to a Byte* so we can do pointer arithmetic on it	
				std::memcpy((mappedEntityData), transform.translation.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 3;
				std::memcpy(mappedEntityData, &stretch_x, sizeof(stretch_x));
				++mappedEntityData;
				std::memcpy(mappedEntityData, transform.scaler.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 3;
				std::memcpy(mappedEntityData, &stretch_y, sizeof(stretch_y));
				++mappedEntityData;
				std::memcpy(mappedEntityData, (math::rotate(transform.rotation)).flatten(flattenedData), sizeof(Matrix<float, 4, 4>));
				mappedEntityData += 16;
				std::memcpy(mappedEntityData, inheritedTranslation.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 4;
				std::memcpy(mappedEntityData, inheritedScale.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 4;
				std::memcpy(mappedEntityData, (inheritedRotation).flatten(flattenedData), sizeof(Matrix<float, 4, 4>));

				buf.unmap();
			}

			std::string processShader(const std::string & shader, const GLenum& type) {
				Preprocessor shaderPreprocessor = Preprocessor(shader, getSSLPreprocessor());
				shaderPreprocessor.defineMacro(mc::Macro("__SHADER_TYPE__", std::to_string(type)));

				const std::string processedShader = shaderPreprocessor.preprocess();

				return processedShader;
			}//processShader

			const mc::Preprocessor& getSSLPreprocessor() {
				if( sslPreprocessor.macroNumber() == 0 ) {
					sslPreprocessor.defineOSMacros();
					sslPreprocessor.defineStandardMacros();

					sslPreprocessor.defineMacro(mc::Macro("__SSL__", "1"));

					sslPreprocessor.addInclude(vertexLibrary);
					sslPreprocessor.addInclude(fragmentLibrary);
					sslPreprocessor.addInclude(colorLibrary);
					sslPreprocessor.addInclude(positionLibrary);
					sslPreprocessor.addInclude(mouseLibrary);
					sslPreprocessor.addInclude(sslLibrary);
				}
				return sslPreprocessor;
			}//getSSLPreprocessor

#undef MACE_WINDOW_DATA_BUFFER_SIZE
#undef MACE_WINDOW_DATA_LOCATION
#undef MACE_PAINT_DATA_BUFFER_SIZE
#undef MACE_PAINT_DATA_LOCATION
#undef MACE_ENTITY_DATA_BUFFER_SIZE
#undef MACE_ENTITY_DATA_LOCATION

		}//ssl

	}//gfx
}//mc

