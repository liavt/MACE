/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Entity2D.h>

namespace mc {
	namespace gfx {
		namespace {
			std::unique_ptr<Renderer> renderer = std::unique_ptr<Renderer>(nullptr);
		}

		Renderer * getRenderer() {
			return renderer.get();
		}

		void setRenderer(Renderer * r) {
			renderer.reset(r);
		}

		void Renderer::init(const Size width, const Size height) {
			originalWidth = width;
			originalHeight = height;

			onInit(width, height);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error generating framebuffer for renderer");
		}//init

		void Renderer::initEntity(GraphicsEntity * en, const Index protocol) {
			protocols[protocol]->initEntity(en);
		}

		void Renderer::setUp(os::WindowModule* win) {
			if (resized) {
				int width, height;

				glfwGetFramebufferSize(win->getGLFWWindow(), &width, &height);

				resize(width, height);

				resized = false;
			}

			onSetUp(win);

			for (Index i = 0; i < protocols.size(); ++i) {
				protocols[i]->setUp(win, &renderQueue);
			}
		}//setUp

		void Renderer::queue(GraphicsEntity * e, const Index protocol) {
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}

			pushEntity(protocol, e);
		}//queue

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(const Size width, const Size height) {
			onResize(width, height);

			for (Index i = 0; i < protocols.size(); ++i) {
				protocols[i]->resize(width, height);
			}

			resized = false;
		}//resize

		Size Renderer::numberOfProtocols() {
			//size() returns size_t which could be larger than unsigned in on some systems, causing problems. static_cast will fix it
			return static_cast<Size>(protocols.size());
		}//numberOfProtocols

		void Renderer::tearDown(os::WindowModule* win) {
			for (Index i = 0; i < protocols.size(); ++i) {
				protocols[i]->tearDown(win, &renderQueue);
			}

			onTearDown(win);

			ogl::checkGLError(__LINE__, __FILE__, "Internal Error: Error destroying renderer");
		}//tearDown

		void Renderer::renderFrame(os::WindowModule* win) {
			setUp(win);
			entityIndex = 0;
			for (RenderQueue::iterator pair = renderQueue.begin(); pair != renderQueue.end(); ++pair) {
				protocols[pair->first]->renderEntity(win, pair->second);
				++entityIndex;
			}
			tearDown(win);
		}//renderFrame

		void Renderer::checkInput(os::WindowModule* win) {
			onInputCheck(win);
		}//checkInput

		void Renderer::cleanEntity(GraphicsEntity * en, const Index protocol) {
			protocols[protocol]->cleanEntity(en);
		}

		void Renderer::destroy() {
			while (!protocols.empty()) {
				RenderImpl* protocol = protocols.back();
				protocol->destroy();
				delete protocol;
				protocols.pop_back();
			}

			onDestroy();
		}//destroy()

		void Renderer::destroyEntity(GraphicsEntity * en, const Index protocol) {
			protocols[protocol]->destroyEntity(en);
		}

		void Renderer::clearBuffers() {
			renderQueue.clear();
		}//clearBuffers()

		void Renderer::setRefreshColor(const Color & c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Size Renderer::getOriginalWidth() const {
			return originalWidth;
		}//getOriginalWidth()

		Size Renderer::getOriginalHeight() const {
			return originalHeight;
		}//getOriginalHeight()

		Size Renderer::getWidth() const {
			return static_cast<Size>(static_cast<float>(Renderer::getOriginalWidth()) * windowRatios[0]);
		}

		Size Renderer::getHeight() const {
			return static_cast<Size>(static_cast<float>(Renderer::getOriginalHeight()) * windowRatios[1]);
		}

		Size Renderer::getSamples() const {
			return samples;
		}//getSamples()

		Vector<float, 2> Renderer::getWindowRatios() const {
			return windowRatios;
		}

		RenderQueue Renderer::getRenderQueue() const {
			return renderQueue;
		}

		Index Renderer::getEntityIndex() const {
			return entityIndex;
		}

		bool Renderer::isResized() const {
			return resized;
		}

		void Renderer::pushEntity(Index protocol, GraphicsEntity * entity) {
			renderQueue.push_back(std::pair<Index, GraphicsEntity*>(protocol, entity));
		}//pushEntity(protocol, entity)

		void Renderer::pushProtocol(RenderImpl * protocol) {
			protocols.push_back(protocol);
		}//pushProtocol(protocol)

		Painter::Painter(GraphicsEntity * const en) : entity(en) {
#ifdef MACE_DEBUG
			if (entity == nullptr) {
				MACE__THROW(NullPointer, "Input to Painter is nullptr!");
			}
#endif
		}

		GraphicsEntity * Painter::getEntity() {
			return entity;
		}

		const GraphicsEntity * Painter::getEntity() const {
			return entity;
		}

		bool Painter::operator==(const Painter & other) const {
			return entity == other.entity;
		}

		bool Painter::operator!=(const Painter & other) const {
			return !operator==(other);
		}
	}//gfx
}//mc
