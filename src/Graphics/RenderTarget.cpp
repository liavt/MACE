/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/RenderTarget.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/Model.h>

//debug purposes
#include <iostream>

namespace mc {
	namespace gfx {
		void Renderer::init() {
			onInit();
		}

		void Renderer::setUp(gfx::WindowModule* win) {
			if (resized) {
				const Vector<Pixels, 2> dimensions = win->getFramebufferSize();

				resize(win, dimensions.x(), dimensions.y());

				resized = false;
			}

			onSetUp(win);
		}//setUp

		void Renderer::queue(Entity* const e, Painter& p) {
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}

			std::shared_ptr<PainterImpl> impl = std::move(createPainterImpl());
			p.impl = impl;
			p.entity = e;
			impl->painter = &p;
			impl->init();
		}//queue

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(WindowModule* win, const Pixels width, const Pixels height) {
			onResize(win, width, height);

			this->currentWidth = width;
			this->currentHeight = height;

			resized = false;
		}//resize

		void Renderer::tearDown(gfx::WindowModule* win) {
			onTearDown(win);
		}//tearDown

		void Renderer::checkInput(gfx::WindowModule* win) {
			const int mouseX = gfx::Input::getMouseX(), mouseY = gfx::Input::getMouseY();
			if (mouseX >= 0 && mouseY >= 0) {
				const auto rootComponent = win->getComponent<MACE__INTERNAL_NS::RootComponent>();
				//std::cout << getEntityAt(static_cast<Pixels>(mouseX), static_cast<Pixels>(mouseY)) << std::endl;
				Entity* hovered = rootComponent->getEntityByID(getEntityAt(static_cast<Pixels>(mouseX), static_cast<Pixels>(mouseY)));

				if (hovered != nullptr && !hovered->needsRemoval()) {
					hovered->hover();
				}
			}
		}//checkInput

		void Renderer::destroy() {
			onDestroy();
		}//destroy()

		EntityID Renderer::getEntityAt(const RelativeTranslation x, const RelativeTranslation y) const {
			return getEntityAt(static_cast<Pixels>(getWidth() * ((x * 0.5f) + 0.5f)), static_cast<Pixels>(getHeight() * ((y * 0.5f) + 0.5f)));
		}

		EntityID Renderer::getEntityAt(const Pixels x, const Pixels y) const {
			if (x > getWidth() || y > getHeight()) {
				return 0;
			}

			EntityID id = 0;
			getEntitiesAt(x, y, 1, 1, &id);

			return id;
		}

		Color Renderer::getPixelAt(const RelativeTranslation x, const RelativeTranslation y, const FrameBufferTarget target) const {
			return getPixelAt(static_cast<Pixels>(getWidth() * ((x * 0.5f) + 0.5f)), static_cast<Pixels>(getHeight() * ((y * 0.5f) + 0.5f)), target);
		}

		Color Renderer::getPixelAt(const Pixels x, const Pixels y, const FrameBufferTarget target) const {
			Color out = Color();
			getPixelsAt(x, y, 1, 1, &out, target);
			return out;
		}

		void Renderer::setRefreshColor(const Color& c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Pixels Renderer::getWidth() const {
			return currentWidth;
		}

		Pixels Renderer::getHeight() const {
			return currentHeight;
		}

		bool Renderer::isResized() const {
			return resized;
		}

		Painter::Painter() : entity(nullptr), impl(nullptr) {}

		Painter::Painter(Entity* const en, const std::shared_ptr<PainterImpl> pimpl) : entity(en), impl(pimpl) {}

		Painter::Painter(Entity* const en, const Painter& p) : impl(p.impl), entity(en) {}

		void Painter::begin() {
			if (impl == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: begin: PainterImpl was nullptr");
			}

			impl->begin();

			setTarget(FrameBufferTarget::COLOR);
		}

		void Painter::end() {
			if (impl == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: end: PainterImpl was nullptr");
			}

			impl->end();
		}


		void Painter::init() {
			if (entity == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Painter Entity is nullptr");
			} else if (impl.get() == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Renderer returned a nullptr RenderImpl for a Painter");
			}

			impl->painter = this;
			impl->init();
		}

		void Painter::destroy() {
			//impl->destroy() is called by the Renderer
		}

		void Painter::clean() {
			impl->clean();
		}

		void Painter::maskImage(const Texture& img, const Texture& mask) {
			push();
			setTexture(img, PainterTextureSlots::FOREGROUND);
			setTexture(mask, PainterTextureSlots::MASK);
			drawQuad(Painter::Brush::MASK);
			pop();
		}

		void Painter::blendImages(const Texture& foreground, const Texture& background, const Progress amount) {
			push();
			setData({amount, 0, 0, 0});
			setTexture(foreground, PainterTextureSlots::FOREGROUND);
			setTexture(background, PainterTextureSlots::BACKGROUND);
			drawQuad(Painter::Brush::BLEND);
			pop();
		}

		void Painter::conditionalMaskImages(const Texture& foreground, const Texture& background, const Texture& mask, const float minimum, const float maximum) {
			push();
			setData({minimum, maximum, 0, 0});
			setTexture(foreground, PainterTextureSlots::FOREGROUND);
			setTexture(background, PainterTextureSlots::BACKGROUND);
			setTexture(mask, PainterTextureSlots::MASK);
			drawQuad(Painter::Brush::CONDITIONAL_MASK);
			pop();
		}

		void Painter::drawQuad(const Painter::Brush brush) {
			draw(entity->getRoot()->getComponent<GraphicsContextComponent>()->getQuad(), brush);
		}

		void Painter::draw(const Model& m, const Painter::Brush brush) {
			MACE_ASSERT(impl != nullptr, "Internal Error: draw: PainterImpl was nullptr");

			impl->loadSettings(state);
			impl->draw(m, brush);
		}

		const Entity* const Painter::getEntity() const {
			return entity;
		}

		void Painter::setTexture(const Texture& t, const TextureSlot slot) {
			t.bindTextureSlot(slot);
			switch (slot) {
			case PainterTextureSlots::FOREGROUND:
				setForegroundColor(t.getHue());
				setForegroundTransform(t.getTransform());
				break;
			case PainterTextureSlots::BACKGROUND:
				setBackgroundColor(t.getHue());
				setBackgroundTransform(t.getTransform());
				break;
			case PainterTextureSlots::MASK:
				setMaskColor(t.getHue());
				setMaskTransform(t.getTransform());
				break;
			default:
				MACE__THROW(OutOfBounds, "setTexture: Unknown Texture slot");
			}
		}

		void Painter::drawModel(const Model& m, const Texture& img) {
			push();
			setTexture(img, PainterTextureSlots::FOREGROUND);
			draw(m, Painter::Brush::TEXTURE);
			pop();
		}

		void Painter::fillModel(const Model& m) {
			draw(m, Painter::Brush::COLOR);
		}

		void Painter::fillRect() {
			push();
			disableRenderFeatures(Painter::RenderFeatures::TEXTURE);
			drawQuad(Painter::Brush::COLOR);
			pop();
		}

		void Painter::drawImage(const Texture& img) {
			push();
			setTexture(img, PainterTextureSlots::FOREGROUND);
			enableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE);
			drawQuad(Painter::Brush::TEXTURE);
			pop();
		}

		void Painter::setForegroundColor(const Color& col) {
			state.foregroundColor = col;
		}

		Color& Painter::getForegroundColor() {
			return state.foregroundColor;
		}

		const Color& Painter::getForegroundColor() const {
			return state.foregroundColor;
		}

		void Painter::setForegroundTransform(const Vector<RelativeUnit, 4> & trans) {
			state.foregroundTransform = trans;
		}

		Vector<RelativeUnit, 4> & Painter::getForegroundTransform() {
			return state.foregroundTransform;
		}

		const Vector<RelativeUnit, 4> & Painter::getForegroundTransform() const {
			return state.foregroundTransform;
		}

		void Painter::setBackgroundColor(const Color& col) {
			state.backgroundColor = col;
		}

		Color& Painter::getBackgroundColor() {
			return state.backgroundColor;
		}

		const Color& Painter::getBackgroundColor() const {
			return state.backgroundColor;
		}

		void Painter::setBackgroundTransform(const Vector<RelativeUnit, 4> & trans) {
			state.backgroundTransform = trans;
		}

		Vector<RelativeUnit, 4> & Painter::getBackgroundTransform() {
			return state.backgroundTransform;
		}

		const Vector<RelativeUnit, 4> & Painter::getBackgroundTransform() const {
			return state.backgroundTransform;
		}

		void Painter::setMaskColor(const Color& col) {
			state.maskColor = col;
		}

		Color& Painter::getMaskColor() {
			return state.maskColor;
		}

		const Color& Painter::getMaskColor() const {
			return state.maskColor;
		}

		void Painter::setMaskTransform(const Vector<RelativeUnit, 4> & trans) {
			state.maskTransform = trans;
		}

		Vector<RelativeUnit, 4> & Painter::getMaskTransform() {
			return state.maskTransform;
		}

		const Vector<RelativeUnit, 4> & Painter::getMaskTransform() const {
			return state.maskTransform;
		}

		void Painter::enableRenderFeatures(const Painter::RenderFeatures feature) {
			state.renderFeatures = state.renderFeatures | feature;
		}

		void Painter::disableRenderFeatures(const Painter::RenderFeatures feature) {
			state.renderFeatures = state.renderFeatures & ~feature;
		}

		void Painter::setRenderFeatures(const Painter::RenderFeatures feature) {
			state.renderFeatures = feature;
		}

		Painter::RenderFeatures& Painter::getRenderFeatures() {
			return state.renderFeatures;
		}

		const Painter::RenderFeatures& Painter::getRenderFeatures() const {
			return state.renderFeatures;
		}

		void Painter::setFilter(const float r, const float g, const float b, const float a) {
			setFilter(Vector<float, 4>({r, g, b, a}));
		}

		void Painter::setFilter(const Vector<float, 4> & col) {
			setFilter(Matrix<float, 4, 4>({
				{col.x(), 0, 0, 0},
				{0, col.y(), 0, 0},
				{0, 0, col.z(), 0},
				{0, 0, 0, col.w()}
					  }));
		}

		void Painter::setFilter(const Matrix<float, 4, 4> & col) {
			state.filter = col;
		}

		Matrix<float, 4, 4> & Painter::getFilter() {
			return state.filter;
		}

		const Matrix<float, 4, 4> & Painter::getFilter() const {
			return state.filter;
		}

		void Painter::setData(const float a, const float b, const float c, const float d) {
			setData({a, b, c, d});
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

		void Painter::setOpacity(const float opacity) {
			state.filter[3][3] = opacity;
		}

		float Painter::getOpacity() {
			return state.filter[3][3];
		}

		const float Painter::getOpacity() const {
			return state.filter[3][3];
		}

		void Painter::setTarget(const FrameBufferTarget& target) {
			impl->setTarget(target);
		}

		void Painter::push() {
			stateStack.push(state);
		}

		void Painter::pop() {
			if (!stateStack.empty()) {
				state = stateStack.top();
				stateStack.pop();
			}
		}

		void Painter::reset() {
			state = Painter::State();
		}

		void Painter::setState(const State& s) {
			state = s;
		}

		Painter::State& Painter::getState() {
			return state;
		}

		const Painter::State& Painter::getState() const {
			return state;
		}

		bool Painter::isInit() const noexcept {
			return impl != nullptr;
		}

		bool Painter::operator==(const Painter& other) const {
			return impl == other.impl && entity == other.entity
				&& state == other.state && stateStack == other.stateStack;
		}

		bool Painter::operator!=(const Painter& other) const {
			return !operator==(other);
		}

		bool PainterImpl::operator==(const PainterImpl& other) const {
			return painter == other.painter;
		}

		bool PainterImpl::operator!=(const PainterImpl& other) const {
			return !operator==(other);
		}

		bool PainterImpl::isInit() const noexcept {
			return painter != nullptr;
		}

		bool Painter::State::operator==(const State& other) const {
			return foregroundColor == other.foregroundColor
				&& backgroundColor == other.backgroundColor && maskColor == other.maskColor
				&& foregroundTransform == other.foregroundTransform && backgroundTransform == other.backgroundTransform
				&& maskTransform == other.maskTransform && data == other.data && filter == other.filter
				&& renderFeatures == other.renderFeatures;
		}

		bool Painter::State::operator!=(const State& other) const {
			return !operator==(other);
		}

		GraphicsEntity::GraphicsEntity() noexcept : painter(this, nullptr) {}

		void GraphicsEntity::init() {
			Entity::init();

			MACE_ASSERT(getRoot()->hasComponent<Renderer>(), "No RenderTarget found at the root Entity!");

			getRoot()->getComponent<Renderer>()->queue(this, painter);

			painter.init();
		}

		void GraphicsEntity::destroy() {
			Entity::destroy();

			painter.destroy();
		}

		Painter& GraphicsEntity::getPainter() {
			makeDirty();

			return painter;
		}

		const Painter& GraphicsEntity::getPainter() const {
			return painter;
		}

		bool GraphicsEntity::operator==(const GraphicsEntity& other) const noexcept {
			return painter == other.painter && Entity::operator==(other);
		}

		bool GraphicsEntity::operator!=(const GraphicsEntity& other) const noexcept {
			return !(operator==(other));
		}

		void GraphicsEntity::onRender() {
			const Beginner beginner(painter);
			onRender(painter);
		}

		void GraphicsEntity::clean() {
			Entity::clean();

			painter.clean();
		}
	}//gfx
}//mc
