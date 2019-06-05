/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#define MACE__COMPONENTS_EXPOSE_MAKE_EASE_FUNCTION//this macro exposes the MACE__MAKE_EASE_FUNCTION macro
#include <MACE/Graphics/Components.h>
#include <MACE/Graphics/Entity2D.h>

namespace mc {
	namespace gfx {
		namespace EaseFunctions {
			//these functions are derived from https://github.com/jesusgollonet/ofpennereasing . Thank you!


			/*See Components.h for MACE__MAKE_EASE_FUNCTION definition and explanation*/
			MACE__MAKE_EASE_FUNCTION(LINEAR) {
				return b + c * (t / d);
			}

			MACE__MAKE_EASE_FUNCTION(BACK_IN) {
				const Progress s = 1.70158f;
				const Progress postFix = t /= d;
				return c * (postFix) * t* ((s + 1.0f) * t - s) + b;
			}

			MACE__MAKE_EASE_FUNCTION(BACK_OUT) {
				const Progress s = 1.70158f;
				t /= d;
				return c * ((t - 1.0f) * t * ((s + 1.0f) * t + s) + 1.0f) + b;
			}

			//shameless resturant promotion
			MACE__MAKE_EASE_FUNCTION(BACK_IN_OUT) {
				const Progress s = 2.5949095f;
				if ((t /= d / 2.0f) < 1.0f) return c / 2.0f * (t * t * ((s + 1.0f) * t - s)) + b;
				const Progress postFix = t -= 2;
				return c / 2.0f * ((postFix) * t * ((s + 1.0f) * t + s) + 2.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(BOUNCE_OUT) {
				if ((t /= d) < (1 / 2.75f)) {
					return c * (7.5625f * t * t) + b;
				} else if (t < (2 / 2.75f)) {
					const Progress postFix = t -= (1.5f / 2.75f);
					return c * (7.5625f * (postFix) * t + .75f) + b;
				} else if (t < (2.5 / 2.75)) {
					const Progress postFix = t -= (2.25f / 2.75f);
					return c * (7.5625f * (postFix) * t + .9375f) + b;
				} else {
					const Progress postFix = t -= (2.625f / 2.75f);
					return c * (7.5625f * (postFix) * t + .984375f) + b;
				}
			}

			//BOUNCE_IN is defined after BOUNCE_OUT just because it requires BOUNCE_OUT to be declared
			MACE__MAKE_EASE_FUNCTION(BOUNCE_IN) {
				return c - EaseFunctions::BOUNCE_OUT(d - t, 0.0f, c, d) + b;
			}

			MACE__MAKE_EASE_FUNCTION(BOUNCE_IN_OUT) {
				if (t < d / 2.0f) return EaseFunctions::BOUNCE_IN(t * 2.0f, 0.0f, c, d) * .5f + b;
				else return EaseFunctions::BOUNCE_OUT(t * 2.0f - d, 0.0f, c, d) * .5f + c * .5f + b;
			}

			MACE__MAKE_EASE_FUNCTION(CIRCLE_IN) {
				t /= d;
				return -c * (std::sqrt(1.0f - t * t) - 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CIRCLE_OUT) {
				t /= d;
				return c * std::sqrt(1.0f - (t - 1.0f) * t) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CIRCLE_IN_OUT) {
				if ((t /= d / 2.0f) < 1) {
					return -c / 2.0f * (std::sqrt(1.0f - t * t) - 1.0f) + b;
				}

				t -= 2.0f;
				return c / 2.0f * (std::sqrt(1.0f - t * t) + 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CUBIC_IN) {
				t /= d;
				return c * t* t* t + b;
			}

			MACE__MAKE_EASE_FUNCTION(CUBIC_OUT) {
				t /= d;
				return c * ((t - 1.0f) * t * t + 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CUBIC_IN_OUT) {
				if ((t /= d / 2.0f) < 1) {
					return c / 2.0f * t * t * t + b;
				}

				t -= 2.0f;
				return c / 2.0f * (t * t * t + 2.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(ELASTIC_IN) {
				if (t == 0.0f) {
					return b;
				} else if ((t /= d) == 1.0f) {
					return b + c;
				}

				const Progress p = d * .3f;
				const Progress s = p / 4.0f;
				--t;
				const Progress postFix = c * static_cast<Progress>(std::pow(2.0f, 10.0f * t));
				return -static_cast<Progress>(postFix * static_cast<Progress>(std::sin((t * d - s)) * (2.0f * math::pi<Progress>()) / p)) + b;
			}

			MACE__MAKE_EASE_FUNCTION(ELASTIC_OUT) {
				if (t == 0.0f) {
					return b;
				} else if ((t /= d) == 1.0f) {
					return b + c;
				}

				const Progress p = d * .3f;
				const Progress s = p / 4.0f;
				return static_cast<Progress>(c * std::pow(2.0f, -10.0f * t) * std::sin((t * d - s) * (2.0f * math::pi<Progress>()) / p) + c + b);
			}

			MACE__MAKE_EASE_FUNCTION(ELASTIC_IN_OUT) {
				if (t == 0.0f) {
					return b;
				} else if ((t /= d / 2.0f) == 2.0f) {
					return b + c;
				}

				const Progress p = d * (.3f * 1.5f);
				const Progress s = p / 4.0f;

				if (t < 1.0f) {
					--t;
					const Progress postFix = c * static_cast<Progress>(std::pow(2.0f, 10.0f * t));
					return -.5f * (postFix * static_cast<Progress>(std::sin((t * d - s) * (2.0f * math::pi<Progress>()) / p))) + b;
				}
				--t;
				const Progress postFix = c * static_cast<Progress>(std::pow(2.0f, -10.0f * (t)));
				return postFix * std::sin((t * d - s) * (2.0f * math::pi<Progress>()) / p) * 0.5f + c + b;
			}

			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_IN) {
				return (t == 0.0f) ? b : c * static_cast<Progress>(std::pow(2.0f, 10.0f * (t / d - 1.0f))) + b;
			}

			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_OUT) {
				return (t == d) ? b + c : c * static_cast<Progress>(-std::pow(2.0f, -10.0f * t / d) + 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_IN_OUT) {
				if (t == 0.0f) return b;
				if (t == d) return b + c;
				if ((t /= d / 2.0f) < 1.0f) return c / 2 * static_cast<Progress>(std::pow(2.0f, 10.0f * (t - 1.0f))) + b;
				return c / 2.0f * static_cast<Progress>(-std::pow(2.0f, -10.0f * --t) + 2.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUADRATIC_IN) {
				t /= d;
				return c * t* t + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUADRATIC_OUT) {
				t /= d;
				return -c * t * (t - 2.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUADRATIC_IN_OUT) {
				if ((t /= d / 2.0f) < 1.0f) return ((c / 2.0f) * (t * t)) + b;
				--t;
				return -c / 2.0f * (((t - 1.0f) * (t)) - 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUARTIC_IN) {
				t /= d;
				return c * (t) * t* t* t + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUARTIC_OUT) {
				t /= d;
				return -c * ((t - 1.0f) * t * t * t - 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUARTIC_IN_OUT) {
				t /= d;
				if ((t / 2.0f) < 1.0f) {
					return c / 2.0f * t * t * t * t + b;
				}
				return -c / 2.0f * (t * t * t * t - 2.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUINTIC_IN) {
				t /= d;
				return c * t* t* t* t* t + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUINTIC_OUT) {
				t /= d;
				return c * ((t - 1.0f) * t * t * t * t + 1.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUINTIC_IN_OUT) {
				t /= d;
				if ((t / 2.0f) < 1.0f) {
					return c / 2.0f * t * t * t * t * t + b;
				}
				t -= 2.0f;
				return c / 2.0f * (t * t * t * t * t + 2.0f) + b;
			}

			MACE__MAKE_EASE_FUNCTION(SINUSOIDAL_IN) {
				return -c * std::cos(t / d * (math::pi<Progress>() / 2.0f)) + c + b;
			}

			MACE__MAKE_EASE_FUNCTION(SINUSOIDAL_OUT) {
				return c * std::sin(t / d * (math::pi<Progress>() / 2.0f)) + b;
			}

			MACE__MAKE_EASE_FUNCTION(SINUSOIDAL_IN_OUT) {
				return -c / 2.0f * (std::cos(math::pi<Progress>() * t / d) - 1.0f) + b;
			}
		}

		void Progressable::addProgress(const Progress prog) {
			setProgress(getProgress() + prog);
		}

		void Progressable::removeProgress(const Progress prog) {
			setProgress(getProgress() - prog);
		}

		AlignmentComponent::AlignmentComponent(const VerticalAlign vert, const HorizontalAlign horz)
			: vertAlign(vert), horzAlign(horz) {}

		void AlignmentComponent::setVerticalAlign(const VerticalAlign & align) {
			if (vertAlign != align) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				vertAlign = align;
			}
		}

		const VerticalAlign& AlignmentComponent::getVerticalAlign() const {
			return vertAlign;
		}

		VerticalAlign& AlignmentComponent::getVerticalAlign() {
			return vertAlign;
		}

		void AlignmentComponent::setHorizontalAlign(const HorizontalAlign & align) {
			if (horzAlign != align) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				horzAlign = align;
			}
		}

		const HorizontalAlign& AlignmentComponent::getHorizontalAlign() const {
			return horzAlign;
		}

		HorizontalAlign& AlignmentComponent::getHorizontalAlign() {
			return horzAlign;
		}

		bool AlignmentComponent::operator==(const AlignmentComponent & other) const {
			return Component::operator==(other)
				&& horzAlign == other.horzAlign && vertAlign == other.vertAlign;
		}

		bool AlignmentComponent::operator!=(const AlignmentComponent & other) const {
			return !operator==(other);
		}

		void AlignmentComponent::clean(Metrics & metrics) {
			const RelativeScale width = metrics.transform.scaler[0], height = metrics.transform.scaler[1];

			switch (horzAlign) {
			default:
			case HorizontalAlign::CENTER:
				metrics.transform.translation[0] = 0.0f;
				break;
			case HorizontalAlign::RIGHT:
				metrics.transform.translation[0] = 1.0f - width;
				break;
			case HorizontalAlign::LEFT:
				metrics.transform.translation[0] = width - 1.0f;
				break;
			}

			switch (vertAlign) {
			default:
			case VerticalAlign::CENTER:
				metrics.transform.translation[1] = 0.0f;
				break;
			case VerticalAlign::BOTTOM:
				metrics.transform.translation[1] = height - 1.0f;
				break;
			case VerticalAlign::TOP:
				metrics.transform.translation[1] = 1.0f - height;
				break;
			}
		}

		EaseComponent::EaseComponent(const EaseUpdateCallback callback, const EaseSettings easeSettings, const Progress start, const Progress dest)
			: Component(), settings(easeSettings)
			, duration(std::chrono::milliseconds(easeSettings.ms) / std::chrono::seconds(1)),
			updateCallback(callback), startingProgress(start), progress(start), destination(dest), currentRepetition(0) {}

		EaseComponent::EaseComponent(Progressable * progressable, const EaseSettings settings, const Progress start, const Progress dest) : EaseComponent([progressable](Entity*, Progress prog) {
			progressable->setProgress(prog);
		}, settings, start, dest) {}

		void EaseComponent::setProgress(const Progress prog) {
			if (progress != prog) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				progress = prog;
			}
			//b(startingProgress), c(destination)
		}

		Progress& EaseComponent::getProgress() {
			return progress;
		}

		const Progress& EaseComponent::getProgress() const {
			return progress;
		}

		Progress EaseComponent::getMinimum() {
			return startingProgress;
		}

		const Progress EaseComponent::getMinimum() const {
			return startingProgress;
		}

		Progress EaseComponent::getMaximum() {
			return destination;
		}

		const Progress EaseComponent::getMaximum() const {
			return destination;
		}

		bool EaseComponent::operator==(const EaseComponent & other) const {
			return Component::operator==(other)
				&& startTime == other.startTime && currentRepetition == other.currentRepetition && startingProgress == other.startingProgress && destination == other.destination && duration == other.duration && progress == other.progress && settings == other.settings;
		}

		bool EaseComponent::operator!=(const EaseComponent & other) const {
			return !operator==(other);
		}

		void EaseComponent::init() {
			startTime = std::chrono::steady_clock::now();
			progress = startingProgress;
		}

		bool EaseComponent::update() {
			//there is a chance that elapsed will be higher than duration, so std::min fixes that
			progress = std::min(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime) / duration, 1.0f);

			if (settings.reverseOnRepeat && currentRepetition % 2) {
				updateCallback(parent, settings.ease(progress, destination, startingProgress - destination, 1.0f));
			} else {
				updateCallback(parent, settings.ease(progress, startingProgress, destination - startingProgress, 1.0f));
			}

			//if progress is greater then or equal to 100%, destroy this component
			if (progress >= 1.0f) {
				++currentRepetition;
				init();
			}

			if (settings.repeats < 0) {
				return false;
			}

			return currentRepetition >= settings.repeats;
		}

		void EaseComponent::render() {}

		void EaseComponent::destroy() {
			progress = destination;
			settings.done(parent);
		}

		void CallbackComponent::init() {
			initCallback(parent);
		}

		bool CallbackComponent::update() {
			return updateCallback(parent);
		}

		void CallbackComponent::render() {
			renderCallback(parent);
		}

		void CallbackComponent::destroy() {
			destroyCallback(parent);
		}

		void CallbackComponent::hover() {
			hoverCallback(parent);
		}

		void CallbackComponent::clean(Metrics & metrics) {
			cleanCallback(parent, metrics);
		}

		MACE_GETTER_SETTER_DEF(CallbackComponent, InitCallback, initCallback, CallbackComponent::CallbackPtr);

		MACE_GETTER_SETTER_DEF(CallbackComponent, UpdateCallback, updateCallback, CallbackComponent::UpdatePtr);

		MACE_GETTER_SETTER_DEF(CallbackComponent, RenderCallback, renderCallback, CallbackComponent::CallbackPtr);

		MACE_GETTER_SETTER_DEF(CallbackComponent, HoverCallback, hoverCallback, CallbackComponent::CallbackPtr);

		MACE_GETTER_SETTER_DEF(CallbackComponent, CleanCallback, cleanCallback, CallbackComponent::CleanPtr);

		MACE_GETTER_SETTER_DEF(CallbackComponent, DestroyCallback, destroyCallback, CallbackComponent::CallbackPtr);

		bool CallbackComponent::operator==(const CallbackComponent & other) const {
			return Component::operator==(other);
		}

		bool CallbackComponent::operator!=(const CallbackComponent & other) const {
			return !operator==(other);
		}

		unsigned int FPSComponent::getUpdatesPerSecond() const {
			return updatesPerSecond;
		}

		unsigned int FPSComponent::getFramesPerSecond() const {
			return framesPerSecond;
		}

		unsigned int FPSComponent::getCleansPerSecond() const {
			return cleansPerSecond;
		}

		unsigned int FPSComponent::getHoversPerSecond() const {
			return hoversPerSecond;
		}

		void FPSComponent::setTickCallback(const TickCallback callback) {
			tickCallback = callback;
		}

		FPSComponent::TickCallback FPSComponent::getTickCallback() {
			return tickCallback;
		}

		const FPSComponent::TickCallback FPSComponent::getTickCallback() const {
			return tickCallback;
		}

		bool FPSComponent::operator==(const FPSComponent & other) const {
			return Component::operator==(other) && updatesPerSecond == other.updatesPerSecond
				&& framesPerSecond == other.framesPerSecond && cleansPerSecond == other.framesPerSecond
				&& hoversPerSecond == other.hoversPerSecond
				&& lastTime == other.lastTime && nbUpdates == other.nbUpdates && nbFrames == other.nbFrames
				&& nbCleans == other.nbCleans && nbHovers == other.nbHovers;
		}

		bool FPSComponent::operator!=(const FPSComponent & other) const {
			return !operator==(other);
		}

		void FPSComponent::init() {
			lastTime = std::chrono::steady_clock::now();
		}

		bool FPSComponent::update() {
			++nbUpdates;
			if (std::chrono::steady_clock::now() - lastTime >= std::chrono::seconds(1)) {
				updatesPerSecond = nbUpdates;
				framesPerSecond = nbFrames;
				cleansPerSecond = nbCleans;
				hoversPerSecond = nbHovers;

				nbFrames = 0;
				nbUpdates = 0;
				nbCleans = 0;
				nbHovers = 0;

				lastTime = std::chrono::steady_clock::now();

				tickCallback(this, parent);
			}
			return false;
		}

		void FPSComponent::render() {
			++nbFrames;
		}

		void FPSComponent::clean(Metrics&) {
			++nbCleans;
		}

		void FPSComponent::hover() {
			++nbHovers;
		}

		void FPSComponent::destroy() {}

		bool NineSliceDesc::operator==(const NineSliceDesc & other) const {
			return topLeft == other.topLeft && top == other.top && topRight == other.topRight && right == other.right && bottomRight == other.bottomRight && bottom == other.bottom && bottomLeft == other.bottomLeft && left == other.left && center == other.center;
		}

		bool NineSliceDesc::operator!=(const NineSliceDesc & other) const {
			return !operator==(other);
		}

		NineSliceComponent::NineSliceComponent(const NineSliceDesc & desc) : desc(desc), en(nullptr), cornerWidth(0.0f), cornerHeight(0.0f) {}

		NineSliceDesc& NineSliceComponent::getDesc() {
			parent->makeDirty();

			return desc;
		}

		const NineSliceDesc& NineSliceComponent::getDesc() const {
			return desc;
		}

		void NineSliceComponent::setDesc(const NineSliceDesc & de) {
			if (this->desc != de) {
				parent->makeDirty();
				this->desc = de;
			}
		}

		void NineSliceComponent::init() {
			en = dynamic_cast<GraphicsEntity*>(parent);

#ifdef MACE_DEBUG_CHECK_NULLPTR
			if (en == nullptr) {
				MACE__THROW(InvalidType, "NineSliceComponent added to a non-graphical Entity");
			}
#endif

			if (desc.topLeft.getDimensions() != desc.bottomRight.getDimensions()
				|| desc.topLeft.getDimensions() != desc.bottomLeft.getDimensions()
				|| desc.topLeft.getDimensions() != desc.topRight.getDimensions()) {
				MACE__THROW(AssertionFailed, "NineSliceDesc must contain 4 corners of equal size");
			}
		}

		void NineSliceComponent::render() {
			Painter& painter = en->getPainter();
			Transformation& transform = painter.getTransformation();
			const float flipSides = desc.flipSides ? -1.0f : 1.0f;

			const Beginner beginner(&painter);
			painter.push();
			painter.reset();
			painter.disableRenderFeatures(Painter::RenderFeatures::FILTER);
			painter.enableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE | Painter::RenderFeatures::STORE_ID);
			transform.scaler = {1.0f - (cornerWidth * 2.0f), 1.0f - (cornerHeight * 2.0f), 1.0f};
			painter.drawImage(desc.center);
			transform.translation.x() = -1.0f + cornerWidth;
			transform.scaler.x() = cornerWidth * flipSides;
			painter.drawImage(desc.left);
			transform.translation.x() *= -1.0f;
			transform.scaler.x() *= flipSides;
			painter.drawImage(desc.right);
			transform.scaler.y() = cornerHeight * flipSides;
			transform.translation.y() = -1.0f + cornerHeight;
			painter.drawImage(desc.bottomRight);
			transform.scaler.y() *= flipSides;
			transform.translation.y() *= -1.0f;
			painter.drawImage(desc.topRight);
			transform.scaler.x() *= flipSides;
			transform.translation.x() *= -1.0f;
			painter.drawImage(desc.topLeft);
			transform.scaler.y() *= flipSides;
			transform.translation.y() *= -1.0f;
			painter.drawImage(desc.bottomLeft);
			transform.translation.x() = 0.0f;
			transform.scaler.x() = (1.0f - (cornerWidth * 2.0f)) * flipSides;
			painter.drawImage(desc.bottom);
			transform.translation.y() *= -1.0f;
			transform.translation.x() *= flipSides;
			painter.drawImage(desc.top);
			painter.pop();
		}

		void NineSliceComponent::clean(Metrics & metrics) {
			const WindowModule* window = gfx::getCurrentWindow();
			const Vector<float, 2> windowRatios = window->getContext()->getRenderer()->getWindowRatios();
			const Vector<float, 3> scale = metrics.transform.scaler * metrics.inherited.scaler;
			cornerWidth = window->convertPixelsToRelativeXCoordinates(desc.topLeft.getWidth()) / scale.x() * windowRatios.x();
			cornerHeight = window->convertPixelsToRelativeYCoordinates(desc.topLeft.getHeight()) / scale.y() * windowRatios.y();
		}

		TextureFramesComponent::TextureFramesComponent(const std::vector<Texture> & tex, const FrameCallback call) : callback(call), frames(tex), progress(0) {}

		void TextureFramesComponent::setProgress(const Progress prog) {
			if (progress != prog) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				progress = prog;
			}
		}

		Progress& TextureFramesComponent::getProgress() {
			return progress;
		}

		const Progress& TextureFramesComponent::getProgress() const {
			return progress;
		}

		Progress TextureFramesComponent::getMinimum() {
			return 0;
		}

		const Progress TextureFramesComponent::getMinimum() const {
			return 0;
		}

		Progress TextureFramesComponent::getMaximum() {
			return static_cast<Progress>(frames.size());
		}

		const Progress TextureFramesComponent::getMaximum() const {
			return static_cast<Progress>(frames.size());
		}

		void TextureFramesComponent::init() {
			progress = 0;
		}

		bool TextureFramesComponent::update() {
			callback(frames.at(math::clamp(static_cast<Index>(math::floor(progress)), Index(0), frames.size() - 1)), parent, this);

			return false;
		}

		bool TextureFramesComponent::operator==(const TextureFramesComponent & other) const {
			return Component::operator==(other) && frames == other.frames && progress == other.progress;
		}

		bool TextureFramesComponent::operator!=(const TextureFramesComponent & other) const {
			return !operator==(other);
		}

		BoundsComponent::BoundsComponent(const Vector<RelativeTranslation, 2> x, const Vector<RelativeTranslation, 2> y, const Vector<RelativeTranslation, 2> z, const BoundsReachedCallback boundsReached)
			: boundsX(x), boundsY(y), boundsZ(z), callback(boundsReached) {}

		BoundsComponent::BoundsComponent(const RelativeTranslation minX, const RelativeTranslation maxX, const RelativeTranslation minY, const RelativeTranslation maxY, const RelativeTranslation minZ, const RelativeTranslation maxZ, const BoundsReachedCallback boundsReached)
			: BoundsComponent({minX, maxX}, {minY, maxY}, {minZ, maxZ}, boundsReached) {}

		BoundsComponent::BoundsReachedCallback BoundsComponent::getCallback() {
			return callback;
		}

		const BoundsComponent::BoundsReachedCallback BoundsComponent::getCallback() const {
			return callback;
		}

		void BoundsComponent::setCallback(const BoundsReachedCallback boundsReached) {
			callback = boundsReached;
		}

		Vector<RelativeTranslation, 2>& BoundsComponent::getBoundsX() {
			return boundsX;
		}

		const Vector<RelativeTranslation, 2>& BoundsComponent::getBoundsX() const {
			return boundsX;
		}

		void BoundsComponent::setBoundsX(const Vector<RelativeTranslation, 2> & vec) {
			if (boundsX != vec) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				boundsX = vec;
			}
		}

		Vector<RelativeTranslation, 2>& BoundsComponent::getBoundsY() {
			return boundsY;
		}

		const Vector<RelativeTranslation, 2>& BoundsComponent::getBoundsY() const {
			return boundsY;
		}

		void BoundsComponent::setBoundsY(const Vector<RelativeTranslation, 2> & vec) {
			if (boundsY != vec) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				boundsY = vec;
			}
		}

		Vector<RelativeTranslation, 2>& BoundsComponent::getBoundsZ() {
			return boundsZ;
		}

		const Vector<RelativeTranslation, 2>& BoundsComponent::getBoundsZ() const {
			return boundsZ;
		}

		void BoundsComponent::setBoundsZ(const Vector<RelativeTranslation, 2> & vec) {
			if (boundsZ != vec) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				boundsZ = vec;
			}
		}

		bool BoundsComponent::operator==(const BoundsComponent & other) {
			return boundsX == other.boundsX && boundsY == other.boundsY && boundsZ == other.boundsZ;
		}

		bool BoundsComponent::operator!=(const BoundsComponent & other) {
			return !operator==(other);
		}

		bool BoundsComponent::operator>(const BoundsComponent & other) {
			return boundsX > other.boundsX&& boundsY > other.boundsY&& boundsZ > other.boundsZ;
		}

		bool BoundsComponent::operator>=(const BoundsComponent & other) {
			return operator>(other) || operator==(other);
		}

		bool BoundsComponent::operator<(const BoundsComponent & other) {
			return !operator>=(other);
		}

		bool BoundsComponent::operator<=(const BoundsComponent & other) {
			return !operator>(other);
		}

		void BoundsComponent::clean(Metrics & metrics) {
			Transformation& transform = metrics.transform;
			RelativeTranslation& x = transform.translation.x(), y = transform.translation.y(), z = transform.translation.z();
			const RelativeScale& width = transform.scaler.x(), height = transform.scaler.y(), depth = transform.scaler.z();
			if (x - width < boundsX.x()) {
				x = boundsX.x() + width;
				callback(parent, this, metrics);
			} else if (x + width > boundsX.y()) {
				x = boundsX.y() - width;
				callback(parent, this, metrics);
			}

			if (y - height < boundsY.x()) {
				y = boundsY.x() + height;
				callback(parent, this, metrics);
			} else if (parent->getY() + parent->getHeight() > boundsY.y()) {
				y = boundsY.y() - height;
				callback(parent, this, metrics);
			}

			if (z - depth < boundsZ.x()) {
				z = boundsZ.x() + depth;
				callback(parent, this, metrics);
			} else if (z + depth > boundsZ.y()) {
				z = boundsZ.y() - depth;
				callback(parent, this, metrics);
			}
		}

		TweenComponent::TweenComponent(Entity * const en, const Transformation dest, const EaseSettings settings) : TweenComponent(en, en->getTransformation(), dest, settings) {}

		TweenComponent::TweenComponent(Entity * const en, const Transformation start, const Transformation dest, const EaseSettings settings) : EaseComponent([start, dest, en](Entity*, float prog) {
			Transformation current = Transformation();
			current.translation = math::lerp(start.translation, dest.translation, prog);
			current.rotation = math::lerp(start.rotation, dest.rotation, prog);
			current.scaler = math::lerp(start.scaler, dest.scaler, prog);
			en->setTransformation(current);
		}, settings), entity(en) {}

		Entity* const TweenComponent::getEntity() {
			return entity;
		}

		const Entity* const TweenComponent::getEntity() const {
			return entity;
		}

		bool TweenComponent::operator==(const TweenComponent & other) const {
			return EaseComponent::operator==(other) && entity == other.entity;
		}

		bool TweenComponent::operator!=(const TweenComponent & other) const {
			return !operator==(other);
		}

		ComponentQueue::ComponentQueue(std::queue<std::shared_ptr<Component>> com) :components(com) {}

		ComponentQueue::ComponentQueue() : ComponentQueue(std::queue<std::shared_ptr<Component>>()) {}

		void ComponentQueue::addComponent(Component & com) {
			addComponent(&com);
		}

		void ComponentQueue::addComponent(Component * com) {
			addComponent(std::shared_ptr<Component>(com, [](Component*) {}));
		}

		void ComponentQueue::addComponent(std::shared_ptr<Component> com) {
			components.push(com);
		}

		MACE_GETTER_SETTER_DEF(ComponentQueue, Components, components, std::queue<std::shared_ptr<Component>>&)

			bool ComponentQueue::operator==(const ComponentQueue & other) const {
			return Component::operator==(other) && components == other.components;
		}

		bool ComponentQueue::operator!=(const ComponentQueue & other) const {
			return !operator==(other);
		}

		void ComponentQueue::init() {
			if (!components.empty()) {
				components.front()->init();
			}
		}

		bool ComponentQueue::update() {
			if (components.empty()) {
				return true;
			}

			const bool result = components.front()->update();
			if (result) {
				components.pop();

				if (components.empty()) {
					return true;
				}

				components.front()->init();

				update();
			}

			return false;
		}

		void ComponentQueue::render() {
			if (!components.empty()) {
				components.front()->render();
			}
		}

		void ComponentQueue::destroy() {}

		void ComponentQueue::hover() {
			if (!components.empty()) {
				components.front()->hover();
			}
		}

		void ComponentQueue::clean(Metrics & metrics) {
			if (!components.empty()) {
				components.front()->clean(metrics);
			}
		}

		bool EaseSettings::operator==(const EaseSettings & other) const {
			return ms == other.ms && repeats == other.repeats;
		}

		bool EaseSettings::operator!=(const EaseSettings & other) const {
			return !operator==(other);
		}

		PaddingComponent::PaddingComponent(const RelativeScale top, const RelativeScale right, const RelativeScale bottom, const RelativeScale left, const RelativeScale front, const RelativeScale back) : paddingTop(top), paddingBottom(bottom), paddingRight(right), paddingLeft(left), paddingFront(front), paddingBack(back) {}

		PaddingComponent::PaddingComponent(const RelativeScale top, const RelativeScale right, const RelativeScale bottom, const RelativeScale left, const RelativeScale z) : PaddingComponent(top, right, bottom, left, z, z) {}

		PaddingComponent::PaddingComponent(const RelativeScale top, const RelativeScale right, const RelativeScale bottom, const RelativeScale left) : PaddingComponent(top, right, bottom, left, 0.0f) {}

		PaddingComponent::PaddingComponent(const RelativeScale top, const RelativeScale x, const RelativeScale bottom) : PaddingComponent(top, x, bottom, x) {}

		PaddingComponent::PaddingComponent(const RelativeScale y, const RelativeScale x) : PaddingComponent(y, x, y) {}

		PaddingComponent::PaddingComponent(const RelativeScale padding) : PaddingComponent(padding, padding) {}

		void PaddingComponent::clean(Metrics & metrics) {
			Vector<RelativeScale, 3>& translation = metrics.transform.scaler;
			Vector<RelativeScale, 3>& scaler = metrics.transform.scaler;

			translation.x() -= paddingLeft - paddingRight;
			scaler.x() += paddingLeft + paddingRight;

			translation.y() -= paddingBottom - paddingTop;
			scaler.y() += paddingBottom + paddingTop;

			translation.z() -= paddingFront - paddingBack;
			scaler.z() += paddingFront + paddingBack;
		}

		UninheritScaleComponent::UninheritScaleComponent() {}

		void UninheritScaleComponent::clean(Metrics & metrics) {
			metrics.transform.scaler /= metrics.inherited.scaler;
			metrics.transform.translation /= metrics.inherited.scaler;
		}
	}//gfx
}//mc
