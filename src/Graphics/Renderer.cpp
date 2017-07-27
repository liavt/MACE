/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Entity2D.h>

#include <iostream>

namespace mc {
	namespace gfx {
		void Renderer::init(gfx::WindowModule* win) {
			onInit(win);
		}

		void Renderer::setUp(gfx::WindowModule* win) {
			if (resized) {
				Vector<int, 2> dimensions = win->getFramebufferSize();

				resize(win, dimensions.x(), dimensions.y());

				resized = false;
			}

			onSetUp(win);
		}//setUp

		Index Renderer::queue(GraphicsEntity * const e) {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}
#endif

			onQueue(e);

			//it adds 1 so 0 (NULL) represents a non-initalized value
			return pushEntity(e) + 1;
		}//queue

		void Renderer::remove(const Index id) {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (id == 0 || id > renderQueue.size()) {
				MACE__THROW(IndexOutOfBounds, "Internal Error: Invalid GraphicsEntity ID to remove");
			}
#endif

			renderQueue[id - 1] = nullptr;
		}

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(WindowModule* win, const Size width, const Size height) {
			onResize(win, width, height);

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

		Index Renderer::pushEntity(GraphicsEntity * const entity) {
			for (Index i = 0; i < renderQueue.size(); ++i) {
				if (renderQueue[i] == nullptr) {
					renderQueue[i] = entity;
					return i;
				}
			}
			renderQueue.push_back(entity);
			return renderQueue.size() - 1;
		}//pushEntity(protocol, entity)

		PainterImpl::PainterImpl(Painter* const p) : painter(p) {}

		Painter::Painter(GraphicsEntity * const en) : entity(en) {}

		Painter::Painter(const Painter & p) : impl(p.impl), entity(p.entity), id(p.id) {}

		void Painter::begin() {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (impl == nullptr) {
				MACE__THROW(NullPointer, "Painter was not initialized correctly");
			} else if (id == 0) {
				MACE__THROW(IndexOutOfBounds, "Invalid Painter ID");
			}
#endif
			impl->begin();
		}

		void Painter::end() {
			impl->end();
		}

		void Painter::maskImage(const Texture & img, const Texture & mask) {
			setTexture(img, Enums::TextureSlot::FOREGROUND);
			setTexture(mask, Enums::TextureSlot::MASK);
			drawQuad(Enums::Brush::MASK);
		}

		void Painter::blendImages(const Texture & foreground, const Texture & background, const float amount) {
			setData({ amount, 0, 0, 0 });
			setTexture(foreground, Enums::TextureSlot::FOREGROUND);
			setTexture(background, Enums::TextureSlot::BACKGROUND);
			drawQuad(Enums::Brush::BLEND);
		}

		void Painter::blendImagesMasked(const Texture & foreground, const Texture & background, const Texture & mask, const float minimum, const float maximum) {
			setData({ minimum, maximum, 0, 0 });
			setTexture(foreground, Enums::TextureSlot::FOREGROUND);
			setTexture(background, Enums::TextureSlot::BACKGROUND);
			setTexture(mask, Enums::TextureSlot::MASK);
			drawQuad(Enums::Brush::MASKED_BLEND);
		}

		void Painter::drawQuad(const Enums::Brush brush, const Enums::RenderType type) {
			draw(Model::getQuad(), brush, type);
		}

		void Painter::draw(const Model& m, const Enums::Brush brush, const Enums::RenderType type) {
			impl->loadSettings(state, dirtyFlags);
			impl->draw(m, brush, type);
			dirtyFlags = 0;
		}

		const GraphicsEntity * const Painter::getEntity() const {
			return entity;
		}

		void Painter::setTexture(const Texture & t, const Enums::TextureSlot& slot) {
			t.bind(static_cast<unsigned int>(slot));
			if (slot == Enums::TextureSlot::FOREGROUND) {
				//the member functions set DirtyFlags accordingly
				setForegroundColor(t.getHue());
				setForegroundTransform(t.getTransform());
			} else if (slot == Enums::TextureSlot::BACKGROUND) {
				setBackgroundColor(t.getHue());
				setBackgroundTransform(t.getTransform());
			} else if (slot == Enums::TextureSlot::MASK) {
				setMaskColor(t.getHue());
				setMaskTransform(t.getTransform());
			} else {
				MACE__THROW(IndexOutOfBounds, "Unknown Texture slot");
			}

		}

		void Painter::drawModel(const Model & m, const Texture & img, const Enums::RenderType type) {
			setTexture(img, Enums::TextureSlot::FOREGROUND);
			draw(m, Enums::Brush::TEXTURE, type);
		}

		void Painter::fillModel(const Model & m, const Enums::RenderType type) {
			draw(m, Enums::Brush::COLOR, type);
		}

		void Painter::fillRect(const float x, const float y, const float w, const float h) {
			translate(x, y);
			scale(w, h);
			drawQuad(Enums::Brush::COLOR);
		}

		void Painter::fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size) {
			fillRect(pos.x(), pos.y(), size.x(), size.y());
		}

		void Painter::fillRect(const Vector<float, 4>& dim) {
			fillRect(dim.x(), dim.y(), dim.z(), dim.w());
		}

		void Painter::drawImage(const Texture & img) {
			setTexture(img, Enums::TextureSlot::FOREGROUND);
			drawQuad(Enums::Brush::TEXTURE);
		}

		void Painter::setForegroundColor(const Color & col) {
			if (state.foregroundColor != col) {
				state.foregroundColor = col;
				dirtyFlags |= Painter::FOREGROUND_COLOR;
			}
		}

		Color & Painter::getForegroundColor() {
			dirtyFlags |= Painter::FOREGROUND_COLOR;
			return state.foregroundColor;
		}

		const Color & Painter::getForegroundColor() const {
			return state.foregroundColor;
		}

		void Painter::setForegroundTransform(const Vector<float, 4>& trans) {
			if (state.foregroundTransform != trans) {
				state.foregroundTransform = trans;
				dirtyFlags |= Painter::FOREGROUND_TRANSFORM;
			}
		}

		Vector<float, 4>& Painter::getForegroundTransform() {
			dirtyFlags |= Painter::FOREGROUND_TRANSFORM;
			return state.foregroundTransform;
		}

		const Vector<float, 4>& Painter::getForegroundTransform() const {
			return state.foregroundTransform;
		}

		void Painter::setBackgroundColor(const Color & col) {
			if (state.backgroundColor != col) {
				state.backgroundColor = col;
				dirtyFlags |= Painter::BACKGROUND_COLOR;
			}
		}

		Color & Painter::getBackgroundColor() {
			dirtyFlags |= Painter::BACKGROUND_COLOR;
			return state.backgroundColor;
		}

		const Color & Painter::getBackgroundColor() const {
			return state.backgroundColor;
		}

		void Painter::setBackgroundTransform(const Vector<float, 4>& trans) {
			if (state.backgroundTransform != trans) {
				state.backgroundTransform = trans;
				dirtyFlags |= Painter::BACKGROUND_TRANSFORM;
			}
		}

		Vector<float, 4>& Painter::getBackgroundTransform() {
			dirtyFlags |= Painter::BACKGROUND_TRANSFORM;
			return state.backgroundTransform;
		}

		const Vector<float, 4>& Painter::getBackgroundTransform() const {
			return state.backgroundTransform;
		}

		void Painter::setMaskColor(const Color & col) {
			if (state.maskColor != col) {
				state.maskColor = col;
				dirtyFlags |= Painter::MASK_COLOR;
			}
		}

		Color & Painter::getMaskColor() {
			dirtyFlags |= Painter::MASK_COLOR;
			return state.maskColor;
		}

		const Color & Painter::getMaskColor() const {
			return state.maskColor;
		}

		void Painter::setMaskTransform(const Vector<float, 4>& trans) {
			if (state.maskTransform != trans) {
				state.maskTransform = trans;
				dirtyFlags |= Painter::MASK_TRANSFORM;
			}
		}

		Vector<float, 4>& Painter::getMaskTransform() {
			dirtyFlags |= Painter::MASK_TRANSFORM;
			return state.maskTransform;
		}

		const Vector<float, 4>& Painter::getMaskTransform() const {
			return state.maskTransform;
		}

		void Painter::setFilter(const float r, const float g, const float b, const float a) {
			setFilter(Vector<float, 4>({ r, g, b, a }));
		}

		void Painter::setFilter(const Vector<float, 4>& col) {
			setFilter(Matrix<float, 4, 4>({
				{col.x(), 0, 0, 0},
				{0, col.y(), 0, 0},
				{0, 0, col.z(), 0},
				{0, 0, 0, col.w()}
			}));
		}

		void Painter::setFilter(const Matrix<float, 4, 4> & col) {
			if (state.filter != col) {
				state.filter = col;
				dirtyFlags |= Painter::FILTER;
			}
		}

		Matrix<float, 4, 4>& Painter::getFilter() {
			dirtyFlags |= Painter::FILTER;
			return state.filter;
		}

		const Matrix<float, 4, 4>& Painter::getFilter() const {
			return state.filter;
		}

		void Painter::setData(const float a, const float b, const float c, const float d) {
			setData({ a, b, c, d });
		}

		void Painter::setData(const Vector<float, 4> & col) {
			if (state.data != col) {
				state.data = col;
				dirtyFlags |= Painter::DATA;
			}
		}

		Vector<float, 4> & Painter::getData() {
			dirtyFlags |= Painter::DATA;
			return state.data;
		}

		const Vector<float, 4> & Painter::getData() const {
			return state.data;
		}

		void Painter::setTransformation(const TransformMatrix & trans) {
			if (state.transformation != trans) {
				state.transformation = trans;
				dirtyFlags |= Painter::TRANSFORMATION;
			}
		}

		TransformMatrix & Painter::getTransformation() {
			dirtyFlags |= Painter::TRANSFORMATION;
			return state.transformation;
		}

		const TransformMatrix & Painter::getTransformation() const {
			return state.transformation;
		}

		void Painter::setOpacity(const float opacity) {
			if (state.filter[3][3] != opacity) {
				state.filter[3][3] = opacity;
				dirtyFlags |= Painter::FILTER;
			}
		}

		float Painter::getOpacity() {
			dirtyFlags |= Painter::FILTER;
			return state.filter[3][3];
		}

		const float Painter::getOpacity() const {
			return state.filter[3][3];
		}

		void Painter::translate(const Vector<float, 3>& vec) {
			translate(vec.x(), vec.y(), vec.z());
		}

		void Painter::translate(const float x, const float y, const float z) {
			if (x != 0.0f || y != 0.0f || z != 0.0f) {
				state.transformation.translate(x, y, z);
				dirtyFlags |= Painter::TRANSLATION;
			}
		}

		void Painter::rotate(const Vector<float, 3>& vec) {
			rotate(vec.x(), vec.y(), vec.z());
		}

		void Painter::rotate(const float x, const float y, const float z) {
			if (x != 0.0f || y != 0.0f || z != 0.0f) {
				state.transformation.rotate(x, y, z);
				dirtyFlags |= Painter::ROTATION;
			}
		}

		void Painter::scale(const Vector<float, 3>& vec) {
			scale(vec.x(), vec.y(), vec.z());
		}

		void Painter::scale(const float x, const float y, const float z) {
			if (x != 1.0f || y != 1.0f || z != 1.0f) {
				state.transformation.scale(x, y, z);
				dirtyFlags |= Painter::SCALE;
			}
		}

		void Painter::resetTransform() {
			state.transformation.reset();
			dirtyFlags |= Painter::TRANSFORMATION;
		}

		void Painter::push() {
			stateStack.push(state);
		}

		void Painter::pop() {
			state = stateStack.top();
			stateStack.pop();
			dirtyFlags |= Painter::ALL;
		}

		void Painter::reset() {
			state = Painter::State();
			dirtyFlags |= Painter::ALL;
		}

		const Index & Painter::getID() const {
			return id;
		}

		Painter Painter::operator=(const Painter& right) {
			return Painter(right);
		}

		bool Painter::operator==(const Painter & other) const {
			return impl == other.impl&& id == other.id && entity == other.entity
				&& dirtyFlags == other.dirtyFlags && state == other.state && stateStack == other.stateStack;
		}

		bool Painter::operator!=(const Painter & other) const {
			return !operator==(other);
		}

		bool PainterImpl::operator==(const PainterImpl & other) const {
			return painter == other.painter;
		}

		bool PainterImpl::operator!=(const PainterImpl & other) const {
			return !operator==(other);
		}

		void Painter::init() {
			Renderer* r = gfx::getCurrentWindow()->getContext()->getRenderer();
			id = r->queue(entity);
			impl = r->createPainterImpl(this);
#ifdef MACE_DEBUG_CHECK_NULLPTR
			if (impl.get() == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Renderer returned a nullptr to a Painter");
			}
#endif

#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (id == 0 || entity == nullptr) {
				MACE__THROW(InitializationFailed, "Internal Error: Invalid PainterImpl settings");
			}
#endif
			impl->init();
		}

		void Painter::destroy() {
			impl->destroy();
			id = 0;
		}

		void Painter::clean() {
			impl->clean();
		}

		bool Painter::State::operator==(const State & other) const {
			return transformation == other.transformation && foregroundColor == other.foregroundColor
				&& backgroundColor == other.backgroundColor && maskColor == other.maskColor
				&& foregroundTransform == other.foregroundTransform && backgroundTransform == other.backgroundTransform
				&& maskTransform == other.maskTransform && data == other.data && filter == other.filter;
		}

		bool Painter::State::operator!=(const State & other) const {
			return !operator==(other);
		}

		GraphicsEntity::GraphicsEntity() noexcept : Entity() {}

		GraphicsEntity::~GraphicsEntity() noexcept {}

		void GraphicsEntity::init() {
			painter.init();

			Entity::init();
		}

		void GraphicsEntity::destroy() {
			Entity::destroy();

			painter.destroy();
		}

		Painter & GraphicsEntity::getPainter() {
			makeDirty();
			return painter;
		}

		const Painter & GraphicsEntity::getPainter() const {
			return painter;
		}

		bool GraphicsEntity::operator==(const GraphicsEntity & other) const noexcept {
			return painter == other.painter&&Entity::operator==(other);
		}

		bool GraphicsEntity::operator!=(const GraphicsEntity & other) const noexcept {
			return !(operator==(other));
		}

		void GraphicsEntity::onRender() {
			painter.begin();
			onRender(painter);
			painter.end();
		}

		void GraphicsEntity::clean() {
			painter.clean();

			Entity::clean();
		}
	}//gfx
}//mc
