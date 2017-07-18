/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Entity2D.h>

//for testing purposes
#include <iostream>

namespace mc {
	namespace gfx {
		void Renderer::init(os::WindowModule* win) {
			onInit(win->getLaunchConfig().width, win->getLaunchConfig().height);
		}

		void Renderer::setUp(os::WindowModule* win) {
			if (resized) {
				Vector<int, 2> dimensions = win->getFramebufferSize();

				resize(dimensions.x(), dimensions.y());

				resized = false;
			}

			onSetUp(win);
		}//setUp

		Index Renderer::queue(GraphicsEntity * e) {
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}

			onQueue(e);

			//it adds 1 so 0 (NULL) represents a non-initalized value
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

		Size Renderer::getWidth() const {
			return static_cast<Size>(static_cast<float>(context->getWindow()->getLaunchConfig().width) * windowRatios[0]);
		}

		Size Renderer::getHeight() const {
			return static_cast<Size>(static_cast<float>(context->getWindow()->getLaunchConfig().width) * windowRatios[1]);
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

		GraphicsContext * Renderer::getContext() {
			return context;
		}

		const GraphicsContext * Renderer::getContext() const {
			return context;
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

		void PainterImpl::maskImage(const Texture & img, const Texture & mask) {
			push();
			setColor(img.getPaint());
			loadSettings();
			img.bind(0);
			mask.bind(1);
			draw(Painter::Brush::MASK, Painter::RenderType::QUAD);
			pop();
		}

		void PainterImpl::blendImagesMasked(const Texture & foreground, const Texture & background, const Texture & mask, const float minimum, const float maximum) {
			push();
			setColor(foreground.getPaint());
			setSecondaryColor(background.getPaint());
			setData({ minimum, maximum, 0, 0 });
			loadSettings();
			foreground.bind(0);
			background.bind(1);
			mask.bind(2);
			draw(Painter::Brush::MASKED_BLEND, Painter::RenderType::QUAD);
			pop();
		}

		const GraphicsEntity * const PainterImpl::getEntity() const {
			return entity;
		}

		void PainterImpl::fillRect(const float x, const float y, const float w, const float h) {
			push();
			translate(x, y);
			scale(w, h);
			loadSettings();
			draw(Painter::Brush::COLOR, Painter::RenderType::QUAD);
			pop();
		}

		void PainterImpl::fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			fillRect(pos.x(), pos.y(), size.x(), size.y());
		}

		void PainterImpl::fillRect(const Vector<float, 4>& dim) {
			fillRect(dim.x(), dim.y(), dim.z(), dim.w());
		}

		void PainterImpl::drawImage(const Texture & img) {
			push();
			setColor(img.getPaint());
			loadSettings();
			img.bind(0);
			draw(Painter::Brush::TEXTURE, Painter::RenderType::QUAD);
			pop();
		}

		void PainterImpl::setColor(const Color & col) {
			state.primaryColor = col;
		}

		Color & PainterImpl::getColor() {
			return state.primaryColor;
		}

		const Color & PainterImpl::getColor() const {
			return state.primaryColor;
		}

		void PainterImpl::setSecondaryColor(const Color & col) {
			state.secondaryColor = col;
		}

		Color & PainterImpl::getSecondaryColor() {
			return state.secondaryColor;
		}

		const Color & PainterImpl::getSecondaryColor() const {
			return state.secondaryColor;
		}

		void PainterImpl::setData(const Vector<float, 4> & col) {
			state.data = col;
		}

		Vector<float, 4> & PainterImpl::getData() {
			return state.data;
		}

		const Vector<float, 4> & PainterImpl::getData() const {
			return state.data;
		}

		void PainterImpl::resetColor() {
			state.primaryColor = Colors::INVISIBLE;
			state.secondaryColor = Colors::INVISIBLE;
			state.data = { 0.0f, 0.0f, 0.0f, 0.0f };
		}

		void PainterImpl::setTransformation(const TransformMatrix & trans) {
			state.transformation = trans;
		}

		TransformMatrix & PainterImpl::getTransformation() {
			return state.transformation;
		}

		const TransformMatrix & PainterImpl::getTransformation() const {
			return state.transformation;
		}

		void PainterImpl::translate(const Vector<float, 3>& vec) {
			translate(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::translate(const float x, const float y, const float z) {
			state.transformation.translate(x, y, z);
		}

		void PainterImpl::rotate(const Vector<float, 3>& vec) {
			rotate(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::rotate(const float x, const float y, const float z) {
			state.transformation.rotate(x, y, z);
		}

		void PainterImpl::scale(const Vector<float, 3>& vec) {
			scale(vec.x(), vec.y(), vec.z());
		}

		void PainterImpl::scale(const float x, const float y, const float z) {
			state.transformation.scale(x, y, z);
		}

		void PainterImpl::resetTransform() {
			state.transformation.reset();
		}

		void PainterImpl::push() {
			stateStack.push(state);
		}

		void PainterImpl::pop() {
			state = stateStack.top();
			stateStack.pop();
		}

		void PainterImpl::reset() {
			resetColor();
			resetTransform();
		}

		bool PainterImpl::operator==(const PainterImpl & other) const {
			return entity == other.entity && state == other.state;
		}

		bool PainterImpl::operator!=(const PainterImpl & other) const {
			return !operator==(other);
		}

		Painter::Painter(GraphicsEntity * const en) {
			impl = os::getCurrentWindow()->getContext()->getRenderer()->createPainterImpl(en);
#ifdef MACE_DEBUG
			if (impl.get() == nullptr) {
				MACE__THROW(NullPointer, "Renderer returned a nullptr to a Painter");
			}
#endif
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

		bool Painter::State::operator==(const State & other) const {
			return transformation == other.transformation && primaryColor == other.primaryColor && secondaryColor == other.secondaryColor && data == other.data;
		}

		bool Painter::State::operator!=(const State & other) const {
			return !operator==(other);
		}
	}//gfx
}//mc
