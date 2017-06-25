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
		}

		void Renderer::setUp(os::WindowModule* win) {
			if (resized) {
				int width, height;

				glfwGetFramebufferSize(win->getGLFWWindow(), &width, &height);

				resize(width, height);

				resized = false;
			}

			onSetUp(win);
		}//setUp

		Index Renderer::queue(GraphicsEntity * e) {
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}

			onQueue(e);

			//it adds 1 so 0 represents a non-initalized value
			return pushEntity(e) + 1;
		}//queue

		void Renderer::remove(const Index id) {
			renderQueue[id - 1] = nullptr;
		}

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(const Size width, const Size height) {
			onResize(width, height);

			resized = false;
		}//resize

		void Renderer::tearDown(os::WindowModule* win) {
			onTearDown(win);
		}//tearDown

		void Renderer::checkInput(os::WindowModule*) {
			GraphicsEntity* hovered = getEntityAt(os::Input::getMouseX(), os::Input::getMouseY());

			if (hovered != nullptr) {
				hovered->hover();
			}

		}//checkInput

		void Renderer::destroy() {
			onDestroy();
		}//destroy()

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

		bool Renderer::isResized() const {
			return resized;
		}

		Index Renderer::pushEntity(GraphicsEntity * entity) {
			for (Index i = 0; i < renderQueue.size(); ++i) {
				if (renderQueue[i] == nullptr) {
					renderQueue[i] = entity;
					return i;
				}
			}
			renderQueue.push_back(entity);
			return renderQueue.size() - 1;
		}//pushEntity(protocol, entity)

		PainterImpl::PainterImpl(const GraphicsEntity * const en) : entity(en) {
			if (en == nullptr) {
				MACE__THROW(NullPointer, "Input to Painter was nullptr");
			}
		}

		const GraphicsEntity * const PainterImpl::getEntity() const {
			return entity;
		}

		void PainterImpl::fillRect(const float x, const float y, const float w, const float h) {
			pushTransform();
			translate(x, y);
			scale(w, h);
			loadSettings(transformation, primary, secondary);
			draw(Painter::Brush::COLOR, Painter::RenderType::QUAD);
			popTransform();
		}

		void PainterImpl::fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			fillRect(pos.x(), pos.y(), size.x(), size.y());
		}

		void PainterImpl::fillRect(const Vector<float, 4>& dim) {
			fillRect(dim.x(), dim.y(), dim.z(), dim.w());
		}

		void PainterImpl::drawImage(const ColorAttachment & img, const float x, const float y, const float w, const float h) {
			pushTransform();
			translate(x, y);
			scale(w, h);
			loadSettings(transformation, img.getPaint(), secondary);
			img.bind();
			draw(Painter::Brush::TEXTURE, Painter::RenderType::QUAD);
			popTransform();
		}

		void PainterImpl::drawImage(const ColorAttachment & img, const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			drawImage(img, pos.x(), pos.y(), size.x(), size.y());
		}

		void PainterImpl::drawImage(const ColorAttachment & img, const Vector<float, 4>& dim) {
			drawImage(img, dim.x(), dim.y(), dim.z(), dim.w());
		}

		void PainterImpl::setColor(const Color & col) {
			primary = col;
		}

		Color & PainterImpl::getColor() {
			return primary;
		}

		const Color & PainterImpl::getColor() const {
			return primary;
		}

		void PainterImpl::setSecondaryColor(const Color & col) {
			secondary = col;
		}

		Color & PainterImpl::getSecondaryColor() {
			return secondary;
		}

		const Color & PainterImpl::getSecondaryColor() const {
			return secondary;
		}

		void PainterImpl::resetColor() {
			primary = Colors::INVISIBLE;
			secondary = Colors::INVISIBLE;
		}

		void PainterImpl::setTransformation(const TransformMatrix & trans) {
			transformation = trans;
		}

		TransformMatrix & PainterImpl::getTransformation() {
			return transformation;
		}

		const TransformMatrix & PainterImpl::getTransformation() const {
			return transformation;
		}

		void PainterImpl::translate(const Vector<float, 3>& vec) {
			translate(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::translate(const float x, const float y, const float z) {
			transformation.translate(x, y, z);
		}

		void PainterImpl::rotate(const Vector<float, 3>& vec) {
			rotate(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::rotate(const float x, const float y, const float z) {
			transformation.rotate(x, y, z);
		}

		void PainterImpl::scale(const Vector<float, 3>& vec) {
			scale(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::scale(const float x, const float y, const float z) {
			transformation.scale(x, y, z);
		}

		void PainterImpl::resetTransform() {
			transformation.reset();
		}

		void PainterImpl::pushTransform() {
			transformationStack.push(transformation);
		}

		void PainterImpl::popTransform() {
			transformation = transformationStack.front();
			transformationStack.pop();
		}

		void PainterImpl::reset() {
			resetColor();
			resetTransform();
		}

		bool PainterImpl::operator==(const PainterImpl & other) const {
			return entity == other.entity && transformation == other.transformation && primary == other.primary && secondary == other.secondary;
		}

		bool PainterImpl::operator!=(const PainterImpl & other) const {
			return !operator==(other);
		}

		Painter::Painter(GraphicsEntity * const en) {
			impl = getRenderer()->getPainter(en);
			if (impl.get() == nullptr) {
				MACE__THROW(NullPointer, "Renderer returned a nullptr to a Painter");
			}
		}

		void Painter::init() {
			impl->init();
		}

		void Painter::destroy() {
			impl->destroy();
		}

		std::shared_ptr<PainterImpl> Painter::getImplementation() {
			return impl;
		}

		const std::shared_ptr<PainterImpl> Painter::getImplementation() const {
			return impl;
		}

		std::shared_ptr<PainterImpl> Painter::operator*() {
			return impl;
		}

		const std::shared_ptr<PainterImpl> Painter::operator*() const {
			return impl;
		}

		std::shared_ptr<PainterImpl> Painter::operator->() {
			return impl;
		}

		const std::shared_ptr<PainterImpl> Painter::operator->() const {
			return impl;
		}
	}//gfx
}//mc
