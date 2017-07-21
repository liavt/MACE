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
		void Renderer::init(gfx::WindowModule* win) {
			onInit(win->getLaunchConfig().width, win->getLaunchConfig().height);
		}

		void Renderer::setUp(gfx::WindowModule* win) {
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

		void Renderer::tearDown(gfx::WindowModule* win) {
			onTearDown(win);
		}//tearDown

		void Renderer::checkInput(gfx::WindowModule*) {
			GraphicsEntity* hovered = getEntityAt(gfx::Input::getMouseX(), gfx::Input::getMouseY());

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

		void Painter::maskImage(const Texture & img, const Texture & mask) {
			push();
			setTexture(img, 0);
			setTexture(mask, 1);
			draw(Model::getQuad(), Enums::Brush::MASK);
			pop();
		}

		void Painter::blendImagesMasked(const Texture & foreground, const Texture & background, const Texture & mask, const float minimum, const float maximum) {
			push();
			setData({ minimum, maximum, 0, 0 });
			setTexture(foreground, 0);
			setTexture(background, 1);
			setTexture(mask, 2);
			draw(Model::getQuad(), Enums::Brush::MASKED_BLEND);
			pop();
		}

		void Painter::draw(const Model& m, const Enums::Brush brush, const Enums::RenderType type) {
			impl->loadSettings(state);
			impl->draw(m, brush, type);
		}

		const GraphicsEntity * const Painter::getEntity() const {
			return impl->entity;
		}

		void Painter::setTexture(const Texture & t, const unsigned int slot) {
			t.bind(slot);
			if (slot == 0) {
				setColor(t.getPaint());
			} else if (slot == 1) {
				setSecondaryColor(t.getPaint());
			}

		}

		void Painter::drawModel(const Model & m, const Texture & img, const Enums::RenderType type) {
			push();
			setTexture(img);
			draw(m, Enums::Brush::TEXTURE, type);
			pop();
		}

		void Painter::fillModel(const Model & m, const Enums::RenderType type) {
			draw(m, Enums::Brush::COLOR, type);
		}

		void Painter::fillRect(const float x, const float y, const float w, const float h) {
			push();
			translate(x, y);
			scale(w, h);
			draw(Model::getQuad(), Enums::Brush::COLOR);
			pop();
		}

		void Painter::fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			fillRect(pos.x(), pos.y(), size.x(), size.y());
		}

		void Painter::fillRect(const Vector<float, 4>& dim) {
			fillRect(dim.x(), dim.y(), dim.z(), dim.w());
		}

		void Painter::drawImage(const Texture & img) {
			push();
			setTexture(img);
			draw(Model::getQuad(), Enums::Brush::TEXTURE);
			pop();
		}

		void Painter::setColor(const Color & col) {
			state.primaryColor = col;
		}

		Color & Painter::getColor() {
			return state.primaryColor;
		}

		const Color & Painter::getColor() const {
			return state.primaryColor;
		}

		void Painter::setSecondaryColor(const Color & col) {
			state.secondaryColor = col;
		}

		Color & Painter::getSecondaryColor() {
			return state.secondaryColor;
		}

		const Color & Painter::getSecondaryColor() const {
			return state.secondaryColor;
		}

		void Painter::setData(const Vector<float, 4> & col) {
			state.data = col;
		}

		Vector<float, 4> & Painter::getData() {
			return state.data;
		}

		const Vector<float, 4> & Painter::getData() const {
			return state.data;
		}

		void Painter::resetColor() {
			state.primaryColor = Colors::INVISIBLE;
			state.secondaryColor = Colors::INVISIBLE;
			state.data = { 0.0f, 0.0f, 0.0f, 0.0f };
		}

		void Painter::setTransformation(const TransformMatrix & trans) {
			state.transformation = trans;
		}

		TransformMatrix & Painter::getTransformation() {
			return state.transformation;
		}

		const TransformMatrix & Painter::getTransformation() const {
			return state.transformation;
		}

		void Painter::translate(const Vector<float, 3>& vec) {
			translate(vec.x(), vec.y(), vec.z());
		}

		void Painter::translate(const float x, const float y, const float z) {
			state.transformation.translate(x, y, z);
		}

		void Painter::rotate(const Vector<float, 3>& vec) {
			rotate(vec.x(), vec.y(), vec.z());
		}

		void Painter::rotate(const float x, const float y, const float z) {
			state.transformation.rotate(x, y, z);
		}

		void Painter::scale(const Vector<float, 3>& vec) {
			scale(vec.x(), vec.y(), vec.z());
		}

		void Painter::scale(const float x, const float y, const float z) {
			state.transformation.scale(x, y, z);
		}

		void Painter::resetTransform() {
			state.transformation.reset();
		}

		void Painter::push() {
			stateStack.push(state);
		}

		void Painter::pop() {
			state = stateStack.top();
			stateStack.pop();
		}

		void Painter::reset() {
			resetColor();
			resetTransform();
		}

		bool Painter::operator==(const Painter & other) const {
			return impl == other.impl && state == other.state && stateStack == other.stateStack;
		}

		bool Painter::operator!=(const Painter & other) const {
			return !operator==(other);
		}

		bool PainterImpl::operator==(const PainterImpl & other) const {
			return entity == other.entity;
		}

		bool PainterImpl::operator!=(const PainterImpl & other) const {
			return !operator==(other);
		}

		Painter::Painter(GraphicsEntity * const en) {
			impl = gfx::getCurrentWindow()->getContext()->getRenderer()->createPainterImpl(en);
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

		bool Painter::State::operator==(const State & other) const {
			return transformation == other.transformation && primaryColor == other.primaryColor && secondaryColor == other.secondaryColor && data == other.data;
		}

		bool Painter::State::operator!=(const State & other) const {
			return !operator==(other);
		}
	}//gfx
}//mc
