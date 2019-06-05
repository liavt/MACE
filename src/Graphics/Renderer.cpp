/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Entity2D.h>

//debug purposes
#include <iostream>

namespace mc {
	namespace gfx {
		void Renderer::init(gfx::WindowModule* win) {
			onInit(win);
		}

		void Renderer::setUp(gfx::WindowModule* win) {
			if (resized) {
				const Vector<Pixels, 2> dimensions = win->getFramebufferSize();

				resize(win, dimensions.x(), dimensions.y());

				resized = false;
			}

			onSetUp(win);
		}//setUp

		void Renderer::queue(GraphicsEntity* const e, Painter& p) {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Input pointer to a GraphicsEntity must not be null in Renderer::queue()");
			}
#endif

			onQueue(e);

			//it adds 1 so 0 (NULL) represents a non-initalized value

			for (EntityID i = 0; i < renderQueue.size(); ++i) {
				if (renderQueue[i].entity == nullptr || renderQueue[i].entity->needsRemoval() || renderQueue[i].entity == e) {
					renderQueue[i].entity = e;
					//0 is a null ID, so we increment by one
					p.id = i + 1;
					p.entity = e;
					p.impl = renderQueue[i].painterImpl;
					renderQueue[i].painterImpl->painter = &p;
				}
			}

			std::shared_ptr<PainterImpl> impl = std::move(createPainterImpl());
			p.impl = impl;
			p.id = static_cast<EntityID>(renderQueue.size()) + 1;
			p.entity = e;
			impl->painter = &p;
			impl->init();

			renderQueue.push_back({e, std::move(impl)});
		}//queue

		void Renderer::remove(const EntityID id) {
#ifdef MACE_DEBUG_CHECK_ARGS
			if (id <= 0 || id > renderQueue.size()) {
				MACE__THROW(OutOfBounds, "Invalid GraphicsEntity ID to remove");
			}
#endif

			renderQueue[id - 1].entity = nullptr;
		}

		void Renderer::flagResize() {
			resized = true;
		}//flagResize

		void Renderer::resize(WindowModule* win, const Pixels width, const Pixels height) {
			const gfx::WindowModule::LaunchConfig& config = context->getWindow()->getLaunchConfig();

			windowRatios = {
				static_cast<float>(config.width) / static_cast<float>(width),
				static_cast<float>(config.height) / static_cast<float>(height)
			};

			onResize(win, width, height);

			resized = false;
		}//resize

		void Renderer::tearDown(gfx::WindowModule* win) {
			onTearDown(win);
		}//tearDown

		void Renderer::checkInput(gfx::WindowModule*) {
			const int mouseX = gfx::Input::getMouseX(), mouseY = gfx::Input::getMouseY();
			if (mouseX >= 0 && mouseY >= 0) {
				GraphicsEntity* hovered = getEntityAt(static_cast<Pixels>(mouseX), static_cast<Pixels>(mouseY));

				if (hovered != nullptr && !hovered->needsRemoval()) {
					hovered->hover();
				}
			}
		}//checkInput

		void Renderer::destroy() {
			onDestroy();

			for (Index i = 0; i < renderQueue.size(); ++i) {
				renderQueue[i].painterImpl->destroy();
			}

			renderQueue.clear();
		}//destroy()

		GraphicsEntity* Renderer::getEntityAt(const RelativeTranslation x, const RelativeTranslation y) {
			return getEntityAt(static_cast<Pixels>(getWidth() * ((x * 0.5f) + 0.5f)), static_cast<Pixels>(getHeight() * ((y * 0.5f) + 0.5f)));
		}

		const GraphicsEntity * Renderer::getEntityAt(const RelativeTranslation x, const RelativeTranslation y) const {
			return getEntityAt(static_cast<Pixels>(getWidth() * ((x * 0.5f) + 0.5f)), static_cast<Pixels>(getHeight() * ((y * 0.5f) + 0.5f)));
		}

		GraphicsEntity * Renderer::getEntityAt(const Pixels x, const Pixels y) {
			if (x > getWidth() || y > getHeight()) {
				return nullptr;
			}

			EntityID id = 0;
			getEntitiesAt(x, y, 1, 1, &id);

			return getEntityByID(id);
		}

		const GraphicsEntity* Renderer::getEntityAt(const Pixels x, const Pixels y) const {
			if (x > getWidth() || y > getHeight()) {
				return nullptr;
			}

			EntityID id = 0;
			getEntitiesAt(x, y, 1, 1, &id);

			return getEntityByID(id);
		}

		Color Renderer::getPixelAt(const RelativeTranslation x, const RelativeTranslation y, const FrameBufferTarget target) const {
			return getPixelAt(static_cast<Pixels>(getWidth() * ((x * 0.5f) + 0.5f)), static_cast<Pixels>(getHeight() * ((y * 0.5f) + 0.5f)), target);
		}

		Color Renderer::getPixelAt(const Pixels x, const Pixels y, const FrameBufferTarget target) const {
			Color out = Color();
			getPixelsAt(x, y, 1, 1, &out, target);
			return out;
		}

		void Renderer::setRefreshColor(const Color & c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Pixels Renderer::getWidth() const {
			return static_cast<Pixels>(static_cast<float>(context->getWindow()->getLaunchConfig().width) * windowRatios[0]);
		}

		Pixels Renderer::getHeight() const {
			return static_cast<Pixels>(static_cast<float>(context->getWindow()->getLaunchConfig().width) * windowRatios[1]);
		}

		unsigned int Renderer::getSamples() const {
			return samples;
		}//getSamples()

		Vector<float, 2> Renderer::getWindowRatios() const {
			return windowRatios;
		}

		RenderQueue Renderer::getRenderQueue() const {
			return renderQueue;
		}

		GraphicsEntity* Renderer::getEntityByID(const EntityID inId) {
			//0 is a NULL ID, so we decrement by one to get the actual index
			if (inId == 0) {
				return nullptr;
			}

			const EntityID id = inId - 1;

			if (id > renderQueue.size()) {
				return nullptr;
			}

			GraphicsEntity* out = renderQueue[id].entity;
			if (out == nullptr) {
				return nullptr;
			}

			if (out->needsRemoval()) {
				renderQueue[id].entity = nullptr;
				return nullptr;
			}

			return out;
		}

		const GraphicsEntity* Renderer::getEntityByID(const EntityID inId) const {
			//0 is a NULL ID, so we decrement by one to get the actual index
			if (inId == 0) {
				return nullptr;
			}

			const EntityID id = inId - 1;

			if (id > renderQueue.size()) {
				return nullptr;
			}

			GraphicsEntity* out = renderQueue[id].entity;
			if (out == nullptr) {
				return nullptr;
			}

			if (out->needsRemoval()) {
				return nullptr;
			}

			return out;
		}

		bool Renderer::isResized() const {
			return resized;
		}

		GraphicsContext* Renderer::getContext() {
			return context;
		}

		const GraphicsContext* Renderer::getContext() const {
			return context;
		}

		Painter::Painter() : entity(nullptr), id(0), impl(nullptr) {}

		Painter::Painter(GraphicsEntity * const en, const EntityID i, const std::shared_ptr<PainterImpl> pimpl) : entity(en), id(i), impl(pimpl) {}

		Painter::Painter(GraphicsEntity * const en, const Painter & p) : id(p.id), impl(p.impl), entity(en) {}

		void Painter::begin() {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (impl == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: begin: PainterImpl was nullptr");
			} else if (id == 0) {
				MACE__THROW(OutOfBounds, "Internal Error: begin: Invalid Painter ID");
			}
#endif
			impl->begin();

			setTarget(FrameBufferTarget::COLOR);
		}

		void Painter::end() {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (impl == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: end: PainterImpl was nullptr");
			} else if (id == 0) {
				MACE__THROW(OutOfBounds, "Internal Error: end: Invalid Painter ID");
			}
#endif
			impl->end();
		}


		void Painter::init() {
#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (id == 0) {
				MACE__THROW(InitializationFailed, "Internal Error: Renderer returned ID 0 (Null)");
			}
#endif
#ifdef MACE_DEBUG_CHECK_NULLPTR
			if (entity == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Painter Entity is nullptr");
			} else if (impl.get() == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: Renderer returned a nullptr RenderImpl for a Painter");
			}
#endif
			impl->painter = this;
			//impl->init() is called by the Renderer
		}

		void Painter::destroy() {
			//impl->destroy() is called by the Renderer
		}

		void Painter::clean() {
			impl->clean();
		}

		void Painter::maskImage(const Texture & img, const Texture & mask) {
			push();
			setTexture(img, TextureSlot::FOREGROUND);
			setTexture(mask, TextureSlot::MASK);
			drawQuad(Painter::Brush::MASK);
			pop();
		}

		void Painter::blendImages(const Texture & foreground, const Texture & background, const Progress amount) {
			push();
			setData({amount, 0, 0, 0});
			setTexture(foreground, TextureSlot::FOREGROUND);
			setTexture(background, TextureSlot::BACKGROUND);
			drawQuad(Painter::Brush::BLEND);
			pop();
		}

		void Painter::conditionalMaskImages(const Texture & foreground, const Texture & background, const Texture & mask, const float minimum, const float maximum) {
			push();
			setData({minimum, maximum, 0, 0});
			setTexture(foreground, TextureSlot::FOREGROUND);
			setTexture(background, TextureSlot::BACKGROUND);
			setTexture(mask, TextureSlot::MASK);
			drawQuad(Painter::Brush::CONDITIONAL_MASK);
			pop();
		}

		void Painter::drawQuad(const Painter::Brush brush) {
			draw(Model::getQuad(), brush);
		}

		void Painter::draw(const Model & m, const Painter::Brush brush) {
#ifdef MACE_DEBUG_CHECK_NULLPTR
			if (impl == nullptr) {
				MACE__THROW(NullPointer, "Internal Error: draw: PainterImpl was nullptr");
			}
#endif

			impl->loadSettings(state);
			impl->draw(m, brush);
		}

		const GraphicsEntity* const Painter::getEntity() const {
			return entity;
		}

		void Painter::setTexture(const Texture & t, const TextureSlot slot) {
			t.bindTextureSlot(slot);
			switch (slot) {
			case TextureSlot::FOREGROUND:
				setForegroundColor(t.getHue());
				setForegroundTransform(t.getTransform());
				break;
			case TextureSlot::BACKGROUND:
				setBackgroundColor(t.getHue());
				setBackgroundTransform(t.getTransform());
				break;
			case TextureSlot::MASK:
				setMaskColor(t.getHue());
				setMaskTransform(t.getTransform());
				break;
			default:
				MACE__THROW(OutOfBounds, "setTexture: Unknown Texture slot");
			}
		}

		void Painter::drawModel(const Model & m, const Texture & img) {
			push();
			setTexture(img, TextureSlot::FOREGROUND);
			draw(m, Painter::Brush::TEXTURE);
			pop();
		}

		void Painter::fillModel(const Model & m) {
			draw(m, Painter::Brush::COLOR);
		}

		void Painter::fillRect(const RelativeTranslation x, const RelativeTranslation y, const RelativeScale w, const RelativeScale h) {
			push();
			translate(x, y);
			scale(w, h);
			disableRenderFeatures(Painter::RenderFeatures::TEXTURE);
			drawQuad(Painter::Brush::COLOR);
			pop();
		}

		void Painter::fillRect(const Vector<RelativeTranslation, 2> & pos, const Vector<RelativeScale, 2> & size) {
			fillRect(pos.x(), pos.y(), size.x(), size.y());
		}

		void Painter::fillRect(const Vector<RelativeUnit, 4> & dim) {
			fillRect(dim.x(), dim.y(), dim.z(), dim.w());
		}

		void Painter::drawImage(const Texture & img) {
			push();
			setTexture(img, TextureSlot::FOREGROUND);
			enableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE);
			drawQuad(Painter::Brush::TEXTURE);
			pop();
		}

		void Painter::setForegroundColor(const Color & col) {
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

		Vector<RelativeUnit, 4>& Painter::getForegroundTransform() {
			return state.foregroundTransform;
		}

		const Vector<RelativeUnit, 4>& Painter::getForegroundTransform() const {
			return state.foregroundTransform;
		}

		void Painter::setBackgroundColor(const Color & col) {
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

		Vector<RelativeUnit, 4>& Painter::getBackgroundTransform() {
			return state.backgroundTransform;
		}

		const Vector<RelativeUnit, 4>& Painter::getBackgroundTransform() const {
			return state.backgroundTransform;
		}

		void Painter::setMaskColor(const Color & col) {
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

		Vector<RelativeUnit, 4>& Painter::getMaskTransform() {
			return state.maskTransform;
		}

		const Vector<RelativeUnit, 4>& Painter::getMaskTransform() const {
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

		Matrix<float, 4, 4>& Painter::getFilter() {
			return state.filter;
		}

		const Matrix<float, 4, 4>& Painter::getFilter() const {
			return state.filter;
		}

		void Painter::setData(const float a, const float b, const float c, const float d) {
			setData({a, b, c, d});
		}

		void Painter::setData(const Vector<float, 4> & col) {
			state.data = col;
		}

		Vector<float, 4>& Painter::getData() {
			return state.data;
		}

		const Vector<float, 4>& Painter::getData() const {
			return state.data;
		}

		void Painter::setTransformation(const Transformation & trans) {
			state.transformation = trans;
		}

		Transformation& Painter::getTransformation() {
			return state.transformation;
		}

		const Transformation& Painter::getTransformation() const {
			return state.transformation;
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

		void Painter::setTarget(const FrameBufferTarget & target) {
			impl->setTarget(target);
		}

		void Painter::translate(const Vector<RelativeTranslation, 3> & vec) {
			translate(vec.x(), vec.y(), vec.z());
		}

		void Painter::translate(const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z) {
			state.transformation.translate(x, y, z);
		}

		void Painter::rotate(const Vector<RelativeRadian, 3> & vec) {
			rotate(vec.x(), vec.y(), vec.z());
		}

		void Painter::rotate(const RelativeRadian x, const RelativeRadian y, const RelativeRadian z) {
			state.transformation.rotate(x, y, z);
		}

		void Painter::scale(const Vector<RelativeScale, 3> & vec) {
			scale(vec.x(), vec.y(), vec.z());
		}

		void Painter::scale(const RelativeScale x, const RelativeScale y, const RelativeScale z) {
			state.transformation.scale(x, y, z);
		}

		void Painter::resetTransform() {
			state.transformation.reset();
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

		void Painter::setState(const State & s) {
			state = s;
		}

		Painter::State& Painter::getState() {
			return state;
		}

		const Painter::State& Painter::getState() const {
			return state;
		}

		const EntityID& Painter::getID() const {
			return id;
		}

		bool Painter::operator==(const Painter & other) const {
			return impl == other.impl && id == other.id && entity == other.entity
				&& state == other.state && stateStack == other.stateStack;
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

		bool Painter::State::operator==(const State & other) const {
			return transformation == other.transformation && foregroundColor == other.foregroundColor
				&& backgroundColor == other.backgroundColor && maskColor == other.maskColor
				&& foregroundTransform == other.foregroundTransform && backgroundTransform == other.backgroundTransform
				&& maskTransform == other.maskTransform && data == other.data && filter == other.filter
				&& renderFeatures == other.renderFeatures;
		}

		bool Painter::State::operator!=(const State & other) const {
			return !operator==(other);
		}

		GraphicsEntity::GraphicsEntity() noexcept : Entity(), painter(this, 0, nullptr) {}

		void GraphicsEntity::init() {
			gfx::getCurrentWindow()->getContext()->getRenderer()->queue(this, painter);
			painter.init();

			Entity::init();
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

		bool GraphicsEntity::operator==(const GraphicsEntity & other) const noexcept {
			return painter == other.painter && Entity::operator==(other);
		}

		bool GraphicsEntity::operator!=(const GraphicsEntity & other) const noexcept {
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
