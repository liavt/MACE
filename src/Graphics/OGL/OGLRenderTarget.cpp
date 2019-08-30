/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/

//std::copy raises a dumb warning on this version of msvc which does not contribute anything at all
//the reason why this check does not use MACE_MSVC is because that would require including Constants.h which is what raises the warning
#if defined(_MSC_VER) && _MSC_VER >= 1400 
#	pragma warning( push ) 
#	pragma warning( disable: 4996 ) 
#endif 

#include <MACE/Graphics/OGL/OGLRenderTarget.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Model.h>
#include <MACE/Core/System.h>

//we need to include algorithim for std::copy
#include <algorithm>

#define MACE__CHECK_ENTITY_INIT(entity) if(!entity->isInit()) return

namespace mc {
	namespace internal {
		namespace ogl {
			Renderer::Renderer(std::shared_ptr<Context> context) : Dispatchable(context) {}

			void Renderer::onInit() {
				eventManager.addListener<gfx::WindowResizedEvent>(parent, [this](gfx::WindowModule*, Vector<Pixels, 2> dims) {
					dispatch([this, dims]() {
						MACE__BEGIN_OGL_FUNCTION;
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

						ogl::setViewport(0, 0, math::max<Pixels>(1, dims.x()), math::max<Pixels>(1, dims.y()));

						generateFramebuffer(math::max<Pixels>(1, dims.x()), math::max<Pixels>(1, dims.y()));

						ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error resizing framebuffer for renderer");
					});
				});

				eventManager.addListener<gfx::PreRenderEvent>(parent, [this](auto) {
					preRender();
				});

				eventManager.addListener<gfx::PostRenderEvent>(parent, [this](auto win) {
					postRender(win);
				});
			}

			void Renderer::preRender() {
				dispatch([this]() {
					MACE__BEGIN_OGL_FUNCTION;
					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: An error occured before onSetUp");

					ogl::resetBlending();

					frameBuffer.bind();

					MACE_CONSTEXPR const Enum buffers[] = {
						GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
						GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
						GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX
					};

					FrameBuffer::setDrawBuffers(3, buffers);

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

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to clear framebuffer");
				});
			}

			void Renderer::postRender(gfx::WindowModule* win) {
				dispatch([this, win]() {
					MACE__BEGIN_OGL_FUNCTION;
					ogl::checkGLError(__LINE__, __FILE__, "Error occured during rendering");

					//frameBuffer.unbind();

					const Vector<Pixels, 2> size = win->getWindowSize();

					if (GLAD_GL_ARB_direct_state_access) {
						glNamedFramebufferReadBuffer(frameBuffer.getID(), GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
						//glNamedFramebufferDrawBuffer(frameBuffer.getID(), GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX);
						//glNamedFramebufferDrawBuffer(0, GL_FRONT);
					} else {
						glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.getID());
						ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX);
					}

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					ogl::FrameBuffer::setDrawBuffer(GL_FRONT);

					if (GLAD_GL_ARB_invalidate_subdata) {
						MACE_CONSTEXPR const Enum buffers[] = {
							GL_COLOR
						};

						glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, os::getArraySize<GLsizei>(buffers), buffers);
					}

					if (GLAD_GL_ARB_direct_state_access) {
						glBlitNamedFramebuffer(frameBuffer.getID(), 0, 0, 0, size[0], size[1], 0, 0, size[0], size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
					} else {
						glBlitFramebuffer(0, 0, size[0], size[1], 0, 0, size[0], size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
					}

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Failed to tear down renderer");

					//glfwSwapBuffers(win->getGLFWWindow());

					if (GLAD_GL_ARB_invalidate_subdata) {
						MACE_CONSTEXPR const Enum buffers[] = {
							GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX,
							GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX,
							GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX
						};

						if (GLAD_GL_ARB_direct_state_access) {
							glInvalidateNamedFramebufferData(frameBuffer.getID(), os::getArraySize<GLsizei>(buffers), buffers);
						} else {
							//our framebuffer was bound to GL_READ_FRAMEBUFFER in the previous if statement for blitting
							glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, os::getArraySize<GLsizei>(buffers), buffers);
						}
					}

					glFlush();

					ogl::forceCheckGLError(__LINE__, __FILE__, "An OpenGL error occurred during a rendering frame");
				});
			}

			Renderer::~Renderer() {
				dispatch([this]() {
					MACE__BEGIN_OGL_FUNCTION;

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
				});
			}

			void Renderer::onQueue(gfx::Entity*) {}

			void Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
				clearColor = Color(r, g, b, a);
			}

			void Renderer::generateFramebuffer(const Pixels width, const Pixels height) {
				MACE__BEGIN_OGL_FUNCTION;

				{
					Object* renderBuffers[] = {
						&sceneBuffer,
						&idBuffer,
						&dataBuffer,
						&depthStencilBuffer
					};

					RenderBuffer::init(renderBuffers, 4);
				}

				depthStencilBuffer.setStorage(GL_DEPTH_STENCIL, width, height);
				depthStencilBuffer.setName("Depth-Stencil buffer");

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating depth buffers for renderer");

				sceneBuffer.setStorage(GL_RGBA, width, height);
				sceneBuffer.setName("Scene buffer");

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating scene texture for renderer");

				idBuffer.setStorage(GL_R32UI, width, height);
				idBuffer.setName("ID buffer");

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating id texture for renderer");

				dataBuffer.setStorage(GL_RGBA, width, height);
				dataBuffer.setName("Data buffer");

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating data texture for renderer");

				frameBuffer.init();
				frameBuffer.setName("Render Target");
				frameBuffer.bind();

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating FrameBuffer for the renderer");

				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX, sceneBuffer);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX, idBuffer);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX, dataBuffer);
				frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilBuffer);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error attaching texture to FrameBuffer for the renderer");

				switch (frameBuffer.checkStatus(GL_FRAMEBUFFER)) {
				case GL_FRAMEBUFFER_UNDEFINED:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_UNDEFINED: The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist. ");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: One of the framebuffer attachments are incomplete!");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The framebuffer is missing at least one image");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER. ");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi. ");
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions. ");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. It can also be that the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures. ");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
					MACE__THROW(internal::ogl::Framebuffer, "GL_FRAMEBUFFER_LAYER_TARGETS: Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target. ");
					break;
				MACE_LIKELY case GL_FRAMEBUFFER_COMPLETE:
					MACE_FALLTHROUGH;
				default:
					//success
					break;
				}

				context->setTarget(gfx::FrameBufferTarget::COLOR);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error setting draw buffers in FrameBuffer for the renderer");

				ogl::setViewport(0, 0, width, height);

				ogl::resetBlending();

				ogl::forceCheckGLError(__LINE__, __FILE__,
					("Internal Error: Renderer: Error resizing framebuffer for width " + std::to_string(width) + " and height " + std::to_string(height)).c_str());
			}

			void Renderer::getEntitiesAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, gfx::EntityID* arr) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glNamedFramebufferReadBuffer(frameBuffer.getID(), GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				} else {
					frameBuffer.bind();

					ogl::FrameBuffer::setReadBuffer(GL_COLOR_ATTACHMENT0 + MACE__ID_ATTACHMENT_INDEX);
				}
				//opengl y-axis is inverted from window coordinates
				ogl::FrameBuffer::readPixels(x, getHeight() - y, w, h, GL_RED_INTEGER, GL_UNSIGNED_INT, arr);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error getting Entity from index buffer");
			}

			void Renderer::getPixelsAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, Color* arr, const gfx::FrameBufferTarget target) const {
				Enum colorAttachment;
				switch (target) {
				case gfx::FrameBufferTarget::COLOR:
					MACE_FALLTHROUGH;
				default:
					colorAttachment = GL_COLOR_ATTACHMENT0 + MACE__SCENE_ATTACHMENT_INDEX;
					break;
				case gfx::FrameBufferTarget::DATA:
					colorAttachment = GL_COLOR_ATTACHMENT0 + MACE__DATA_ATTACHMENT_INDEX;
					break;
				}

				if (GLAD_GL_ARB_direct_state_access) {
					glNamedFramebufferReadBuffer(frameBuffer.getID(), colorAttachment);
				} else {
					frameBuffer.bind();

					ogl::FrameBuffer::setReadBuffer(colorAttachment);
				}

				//opengl y-axis is inverted from window coordinates
				ogl::FrameBuffer::readPixels(x, getHeight() - y, w, h, GL_RGBA, GL_FLOAT, arr);

				ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error reading pixels");
			}

			std::shared_ptr<gfx::PainterImpl> Renderer::createPainterImpl() {
				return std::shared_ptr<gfx::PainterImpl>(new Painter(context, this));
			}

			Painter::Painter(std::shared_ptr<Context> con, Renderer* const r) : renderer(r), Dispatchable(con) {}

			void Painter::init() {
				dispatch([this]() {
					MACE__CHECK_ENTITY_INIT(painter->getEntity());
					MACE__BEGIN_OGL_FUNCTION;

					Object* buffers[] = {
						&uniformBuffers.entityData,
						&uniformBuffers.painterData
					};

					UniformBuffer::init(buffers, 2);

					createPainterData();
					createEntityData();

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error creating painter");
				});
			}

			void Painter::createEntityData() {
				MACE__BEGIN_OGL_FUNCTION;

				MACE_STATIC_ASSERT(sizeof(float) >= sizeof(gfx::EntityID), "This system doesn't not support the required size for EntityID");

				savedMetrics = painter->getEntity()->getMetrics();

				const Transformation& transform = savedMetrics.transform;
				const Transformation& inherited = savedMetrics.inherited;

				float entityDataBuffer[MACE__ENTITY_DATA_BUFFER_SIZE / sizeof(float)] = {};

				transform.translation.flatten(entityDataBuffer);
				//offset by 4
				transform.rotation.flatten(entityDataBuffer + 4);
				transform.scaler.flatten(entityDataBuffer + 8);
				inherited.translation.flatten(entityDataBuffer + 12);
				inherited.rotation.flatten(entityDataBuffer + 16);
				inherited.scaler.flatten(entityDataBuffer + 20);
				//this crazy line puts a float directly into a GLuint, as GLSL expects a uint instead of a float
				*reinterpret_cast<GLuint*>(entityDataBuffer + 24) = static_cast<GLuint>(painter->getEntity()->getID());

				uniformBuffers.entityData.createStorage(MACE__ENTITY_DATA_BUFFER_SIZE, entityDataBuffer, MACE__ENTITY_DATA_STORAGE_FLAGS);

				uniformBuffers.entityData.setName(MACE_STRINGIFY_DEFINITION(MACE__ENTITY_DATA_NAME));
			}

			void Painter::createPainterData() {
				MACE__BEGIN_OGL_FUNCTION;

				savedState = painter->getState();//create default state

				float painterDataBuffer[MACE__PAINTER_DATA_BUFFER_SIZE / sizeof(float)] = {0};
				savedState.data.flatten(painterDataBuffer);
				savedState.foregroundColor.flatten(painterDataBuffer + 4);
				savedState.foregroundTransform.flatten(painterDataBuffer + 8);
				savedState.backgroundColor.flatten(painterDataBuffer + 12);
				savedState.backgroundTransform.flatten(painterDataBuffer + 16);
				savedState.maskColor.flatten(painterDataBuffer + 20);
				savedState.maskTransform.flatten(painterDataBuffer + 24);
				savedState.filter.flatten(painterDataBuffer + 28);

				uniformBuffers.painterData.createStorage(MACE__PAINTER_DATA_BUFFER_SIZE, painterDataBuffer, MACE__PAINTER_DATA_STORAGE_FLAGS);

				uniformBuffers.painterData.setName(MACE_STRINGIFY_DEFINITION(MACE__PAINTER_DATA_NAME));
			}

			Painter::~Painter() {
				dispatch([this]() {
					MACE__BEGIN_OGL_FUNCTION;

					Object* buffers[] = {
						&uniformBuffers.entityData,
						&uniformBuffers.painterData
					};

					UniformBuffer::destroy(buffers, 2);
				});
			}

			void Painter::begin() {}

			void Painter::end() {}

			void Painter::setTarget(const gfx::FrameBufferTarget& target) {
				dispatch([this, &target]() {
					context->setTarget(target);
				});
			}

			void Painter::clean() {
				dispatch([this]() {
					MACE__CHECK_ENTITY_INIT(painter->getEntity());
					MACE__BEGIN_OGL_FUNCTION;

					const gfx::Metrics metrics = painter->getEntity()->getMetrics();

					if (metrics == savedMetrics) {
						return;
					}

					const Transformation& transform = metrics.transform;
					const Transformation& inherited = metrics.inherited;

					//now we set the data defining the transformations of the entity
					uniformBuffers.entityData.bind();

#define MACE__DATA_RANGE_ENTRY(name, offset, size) if(name != savedMetrics.name) {uniformBuffers.entityData.setDataRange(sizeof(float) * offset, sizeof(float) * size, name.begin());}
					MACE__DATA_RANGE_ENTRY(transform.translation, 0, 3);
					MACE__DATA_RANGE_ENTRY(transform.rotation, 4, 3);
					MACE__DATA_RANGE_ENTRY(transform.scaler, 8, 3);
					MACE__DATA_RANGE_ENTRY(inherited.translation, 12, 3);
					MACE__DATA_RANGE_ENTRY(inherited.rotation, 16, 3);
					MACE__DATA_RANGE_ENTRY(inherited.scaler, 20, 3);
#undef MACE__DATA_RANGE_ENTRY

					savedMetrics = metrics;

					checkGLError(__LINE__, __FILE__, "Internal Error: Failed to update GPU-side buffer in Entity clean");
				});
			}

			void Painter::loadSettings(const gfx::Painter::State& state) {
				dispatch([this, &state]() {
					if (state == savedState) {
						return;
					}
					
					MACE__CHECK_ENTITY_INIT(painter->getEntity());

					MACE__BEGIN_OGL_FUNCTION;

					uniformBuffers.painterData.bind();

#define MACE__DATA_RANGE_ENTRY(name, offset, size, cond) if(state.name != savedState.name && cond){uniformBuffers.painterData.setDataRange(sizeof(float) * offset, sizeof(float) * size, state.name.begin());}
					MACE__DATA_RANGE_ENTRY(data, 0, 4, true);
					MACE__DATA_RANGE_ENTRY(foregroundColor, 4, 4, true);
					MACE__DATA_RANGE_ENTRY(foregroundTransform, 8, 4, MACE__HAS_RENDER_FEATURE(state.renderFeatures, TEXTURE_TRANSFORM));
					MACE__DATA_RANGE_ENTRY(backgroundColor, 12, 4, true);
					MACE__DATA_RANGE_ENTRY(backgroundTransform, 16, 4, MACE__HAS_RENDER_FEATURE(state.renderFeatures, TEXTURE_TRANSFORM));
					MACE__DATA_RANGE_ENTRY(maskColor, 20, 4, true);
					MACE__DATA_RANGE_ENTRY(maskTransform, 24, 4, MACE__HAS_RENDER_FEATURE(state.renderFeatures, TEXTURE_TRANSFORM));
					MACE__DATA_RANGE_ENTRY(filter, 28, 16, MACE__HAS_RENDER_FEATURE(state.renderFeatures, FILTER));
#undef MACE__DATA_RANGE_ENTRY

					savedState = state;

					ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error loading painter settings");
				});
			}

			void Painter::draw(const gfx::Model& m, const gfx::Painter::Brush brush) {
				dispatch([this, &m, brush]() {
					MACE__CHECK_ENTITY_INIT(painter->getEntity());
					MACE__BEGIN_OGL_FUNCTION;

					context->bindProtocol(this, {brush, savedState.renderFeatures});

					m.draw();

					checkGLError(__LINE__, __FILE__, "Internal Error: An error occured while drawing a model");
				});
			}
		}//ogl
	}//internal
}//mc

#if defined(_MSC_VER) && _MSC_VER >= 1400 
 //pop the disable of warning 4996 which is a useless warning in our scenario
#	pragma warning( pop ) 
#endif 

#undef MACE__CHECK_ENTITY_INIT