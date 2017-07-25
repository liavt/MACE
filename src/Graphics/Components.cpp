/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define MACE__COMPONENTS_EXPOSE_MAKE_EASE_FUNCTION//this macro exposes the MACE__MAKE_EASE_FUNCTION macro
#include <MACE/Graphics/Components.h>
#include <ctime>
#include <cmath>

namespace mc {
	namespace gfx {
		namespace EaseFunctions {
			//these fucntions are derived from https://github.com/jesusgollonet/ofpennereasing . Thank you!


			/*See Components.h for MACE__MAKE_EASE_FUNCTION definition and explanation*/
			MACE__MAKE_EASE_FUNCTION(LINEAR) {
				return b + c*(t / d);
			}

			MACE__MAKE_EASE_FUNCTION(BACK_IN) {
				const float s = 1.70158f;
				const float postFix = t /= d;
				return c*(postFix)*t*((s + 1)*t - s) + b;
			}

			//shameless resturant promotion
			MACE__MAKE_EASE_FUNCTION(BACK_OUT) {
				const float s = 1.70158f;
				t /= d;
				return c*((t - 1)*t*((s + 1)*t + s) + 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(BACK_IN_OUT) {
				const float s = 2.5949095f;
				if ((t /= d / 2) < 1) return c / 2 * (t*t*((s + 1)*t - s)) + b;
				const float postFix = t -= 2;
				return c / 2 * ((postFix)*t*((s + 1)*t + s) + 2) + b;
			}

			MACE__MAKE_EASE_FUNCTION(BOUNCE_OUT) {
				if ((t /= d) < (1 / 2.75f)) {
					return c*(7.5625f*t*t) + b;
				} else if (t < (2 / 2.75f)) {
					const float postFix = t -= (1.5f / 2.75f);
					return c*(7.5625f*(postFix)*t + .75f) + b;
				} else if (t < (2.5 / 2.75)) {
					const float postFix = t -= (2.25f / 2.75f);
					return c*(7.5625f*(postFix)*t + .9375f) + b;
				} else {
					const float postFix = t -= (2.625f / 2.75f);
					return c*(7.5625f*(postFix)*t + .984375f) + b;
				}
			}

			//BOUNCE_IN is defined after BOUNCE_OUT just because it requires BOUNCE_OUT to be eclared
			MACE__MAKE_EASE_FUNCTION(BOUNCE_IN) {
				return c - EaseFunctions::BOUNCE_OUT(d - t, 0, c, d) + b;
			}

			MACE__MAKE_EASE_FUNCTION(BOUNCE_IN_OUT) {
				if (t < d / 2) return EaseFunctions::BOUNCE_IN(t * 2, 0, c, d) * .5f + b;
				else return EaseFunctions::BOUNCE_OUT(t * 2 - d, 0, c, d) * .5f + c*.5f + b;
			}

			MACE__MAKE_EASE_FUNCTION(CIRCLE_IN) {
				t /= d;
				return -c * (std::sqrt(1 - t*t) - 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CIRCLE_OUT) {
				t /= d;
				return c * std::sqrt(1 - (t - 1)*t) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CIRCLE_IN_OUT) {
				if ((t /= d / 2) < 1) {
					return -c / 2 * (std::sqrt(1 - t*t) - 1) + b;
				}

				t -= 2;
				return c / 2 * (std::sqrt(1 - t*t) + 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CUBIC_IN) {
				t /= d;
				return c*t*t*t + b;
			}

			MACE__MAKE_EASE_FUNCTION(CUBIC_OUT) {
				t /= d;
				return c*((t - 1)*t*t + 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(CUBIC_IN_OUT) {
				if ((t /= d / 2) < 1) {
					return c / 2 * t*t*t + b;
				}

				t -= 2;
				return c / 2 * (t*t*t + 2) + b;
			}

			MACE__MAKE_EASE_FUNCTION(ELASTIC_IN) {
				if (t == 0) {
					return b;
				} else if ((t /= d) == 1) {
					return b + c;
				}

				const float p = d*.3f;
				const float s = p / 4;
				--t;
				const float postFix = c*static_cast<float>(std::pow(2, 10 * t));
				return -static_cast<float>(postFix * static_cast<float>(std::sin((t*d - s))*(2 * static_cast<float>(math::pi())) / p)) + b;
			}

			MACE__MAKE_EASE_FUNCTION(ELASTIC_OUT) {
				if (t == 0) {
					return b;
				} else if ((t /= d) == 1) {
					return b + c;
				}

				const float p = d*.3f;
				const float s = p / 4;
				return static_cast<float>(c*std::pow(2, -10 * t) * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p) + c + b);
			}

			MACE__MAKE_EASE_FUNCTION(ELASTIC_IN_OUT) {
				if (t == 0) {
					return b;
				} else if ((t /= d / 2) == 2) {
					return b + c;
				}

				const float p = d*(.3f*1.5f);
				const float s = p / 4;

				if (t < 1) {
					--t;
					const float postFix = c*static_cast<float>(std::pow(2, 10 * t));
					return -.5f*(postFix* static_cast<float>(std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p))) + b;
				}
				--t;
				const float postFix = c*static_cast<float>(std::pow(2, -10 * (t)));
				return postFix * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p)*0.5f + c + b;
			}

			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_IN) {
				return (t == 0) ? b : c * static_cast<float>(std::pow(2, 10 * (t / d - 1))) + b;
			}

			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_OUT) {
				return (t == d) ? b + c : c * static_cast<float>(-std::pow(2, -10 * t / d) + 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_IN_OUT) {
				if (t == 0) return b;
				if (t == d) return b + c;
				if ((t /= d / 2) < 1) return c / 2 * static_cast<float>(std::pow(2, 10 * (t - 1))) + b;
				return c / 2 * static_cast<float>(-std::pow(2, -10 * --t) + 2) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUADRATIC_IN) {
				t /= d;
				return c*t*t + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUADRATIC_OUT) {
				t /= d;
				return -c *t*(t - 2) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUADRATIC_IN_OUT) {
				if ((t /= d / 2) < 1) return ((c / 2)*(t*t)) + b;
				--t;
				return -c / 2 * (((t - 1)*(t)) - 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUARTIC_IN) {
				t /= d;
				return c*(t)*t*t*t + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUARTIC_OUT) {
				t /= d;
				return -c * ((t - 1)*t*t*t - 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUARTIC_IN_OUT) {
				t /= d;
				if ((t / 2) < 1) {
					return c / 2 * t*t*t*t + b;
				}
				return -c / 2 * (t*t*t*t - 2) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUINTIC_IN) {
				t /= d;
				return c*t*t*t*t*t + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUINTIC_OUT) {
				t /= d;
				return c*((t - 1)*t*t*t*t + 1) + b;
			}

			MACE__MAKE_EASE_FUNCTION(QUINTIC_IN_OUT) {
				t /= d;
				if ((t / 2) < 1) {
					return c / 2 * t*t*t*t*t + b;
				}
				t -= 2;
				return c / 2 * (t*t*t*t*t + 2) + b;
			}

			MACE__MAKE_EASE_FUNCTION(SINUSODIAL_IN) {
				return -c * std::cos(t / d * (static_cast<float>(math::pi()) / 2)) + c + b;
			}

			MACE__MAKE_EASE_FUNCTION(SINUSODIAL_OUT) {
				return c * std::sin(t / d * (static_cast<float>(math::pi()) / 2)) + b;
			}

			MACE__MAKE_EASE_FUNCTION(SINUSODIAL_IN_OUT) {
				return -c / 2 * (std::cos(static_cast<float>(math::pi())*t / d) - 1) + b;
			}
		}

		bool Selectable::isClicked() const {
			return selectableProperties.getBit(Selectable::CLICKED);
		}

		bool Selectable::isDisabled() const {
			return selectableProperties.getBit(Selectable::DISABLED);
		}

		bool Selectable::isHovered() const {
			return selectableProperties.getBit(Selectable::HOVERED);
		}

		void Selectable::click() {
			selectableProperties.setBit(Selectable::CLICKED, true);

			onClick();
		}

		void Selectable::disable() {
			selectableProperties.setBit(Selectable::DISABLED, true);

			onDisable();
		}

		void Selectable::enable() {
			selectableProperties.setBit(Selectable::DISABLED, false);

			onEnable();
		}

		void Selectable::trigger() {
			onTrigger();
		}

		void Selectable::onClick() {}


		void Selectable::onEnable() {}

		void Selectable::onDisable() {}

		void Selectable::onTrigger() {}

		AlignmentComponent::AlignmentComponent(const Enums::VerticalAlign vert, const Enums::HorizontalAlign horz)
			: vertAlign(vert), horzAlign(horz) {}

		void AlignmentComponent::setVerticalAlign(const Enums::VerticalAlign align) {
			if (vertAlign != align) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				vertAlign = align;
			}
		}

		const Enums::VerticalAlign AlignmentComponent::getVerticalAlign() const {
			return vertAlign;
		}

		void AlignmentComponent::setHorizontalAlign(Enums::HorizontalAlign align) {
			if (horzAlign != align) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				horzAlign = align;
			}
		}

		const Enums::HorizontalAlign AlignmentComponent::getHorizontalAlign() const {
			return horzAlign;
		}

		bool AlignmentComponent::operator==(const AlignmentComponent & other) const {
			return Component::operator==(other)
				&& horzAlign == other.horzAlign && vertAlign == other.vertAlign;
		}

		bool AlignmentComponent::operator!=(const AlignmentComponent & other) const {
			return !operator==(other);
		}

		void AlignmentComponent::clean() {
			const Entity::Metrics m = parent->getMetrics();

			const float width = m.scale[0], height = m.scale[1];

			switch (horzAlign) {
				default:
				case Enums::HorizontalAlign::CENTER:
					parent->setX(0.0f);
					//e.setX((-width / 2) + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
				case Enums::HorizontalAlign::RIGHT:
					parent->setX(1.0f - (width / 2.0f));
					break;
				case Enums::HorizontalAlign::LEFT:
					parent->setX((width / 2.0f) - 1.0f);
					break;
			}

			switch (vertAlign) {
				default:
				case Enums::VerticalAlign::CENTER:
					parent->setX(0.0f);
					break;
				case Enums::VerticalAlign::BOTTOM:
					parent->setY((height / 2.0f) - 1.0f);
					break;
				case Enums::VerticalAlign::TOP:
					parent->setY(1.0f - (height / 2.0f));
					break;
			}
		}

		EaseComponent::EaseComponent(const float duration, const float startingProgress, const float destination, const EaseUpdateCallback callback, const EaseFunction easeFunction, const EaseDoneCallback doneCallback)
			: Component(), t(0), b(startingProgress), c(destination), d(duration), updateCallback(callback), ease(easeFunction), done(doneCallback) {}

		bool EaseComponent::operator==(const EaseComponent & other) const {
			return Component::operator==(other)
				&& t == other.t && b == other.b && c == other.c && d == other.d
				&& updateCallback == other.updateCallback && ease == other.ease
				&& done == other.done;
		}

		bool EaseComponent::operator!=(const EaseComponent & other) const {
			return !operator==(other);
		}

		void EaseComponent::init() {}

		bool EaseComponent::update() {
			updateCallback(parent, ease(t++, b, c - b, d));

			if (t >= d) {
				return true;
			}

			return false;
		}

		void EaseComponent::render() {}

		void EaseComponent::destroy() {
			done(parent);
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

		void CallbackComponent::clean() {
			cleanCallback(parent);
		}

		void CallbackComponent::setInitCallback(const CallbackPtr func) {
			initCallback = func;
		}

		CallbackComponent::CallbackPtr CallbackComponent::getInitCallback() {
			return initCallback;
		}

		const CallbackComponent::CallbackPtr CallbackComponent::getInitCallback() const {
			return initCallback;
		}

		void CallbackComponent::setUpdateCallback(const UpdatePtr func) {
			updateCallback = func;
		}

		CallbackComponent::UpdatePtr CallbackComponent::getUpdateCallback() {
			return updateCallback;
		}

		const CallbackComponent::UpdatePtr CallbackComponent::getUpdateCallback() const {
			return updateCallback;
		}

		void CallbackComponent::setRenderCallback(const CallbackPtr func) {
			renderCallback = func;
		}

		CallbackComponent::CallbackPtr CallbackComponent::getRenderCallback() {
			return renderCallback;
		}

		const CallbackComponent::CallbackPtr CallbackComponent::getRenderCallback() const {
			return renderCallback;
		}

		void CallbackComponent::setHoverCallback(const CallbackPtr func) {
			hoverCallback = func;
		}
		CallbackComponent::CallbackPtr CallbackComponent::getHoverCallback() {
			return hoverCallback;
		}
		const CallbackComponent::CallbackPtr CallbackComponent::getHoverCallback() const {
			return hoverCallback;
		}

		void CallbackComponent::setCleanCallback(const CallbackPtr func) {
			cleanCallback = func;
		}

		CallbackComponent::CallbackPtr CallbackComponent::getCleanCallback() {
			return cleanCallback;
		}

		const CallbackComponent::CallbackPtr CallbackComponent::getCleanCallback() const {
			return cleanCallback;
		}

		bool CallbackComponent::operator==(const CallbackComponent & other) const {
			return Component::operator==(other)
				&& destroyCallback == other.destroyCallback && renderCallback == other.renderCallback
				&& initCallback == other.initCallback && hoverCallback == other.hoverCallback
				&& cleanCallback == other.cleanCallback && updateCallback == other.updateCallback;
		}

		bool CallbackComponent::operator!=(const CallbackComponent & other) const {
			return !operator==(other);
		}

		void CallbackComponent::setDestroyCallback(const CallbackPtr func) {
			destroyCallback = func;
		}

		CallbackComponent::CallbackPtr CallbackComponent::getDestroyCallback() {
			return destroyCallback;
		}

		const CallbackComponent::CallbackPtr CallbackComponent::getDestroyCallback() const {
			return destroyCallback;
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

		void FPSComponent::setTickCallback(const TickCallbackPtr callback) {
			tickCallback = callback;
		}

		FPSComponent::TickCallbackPtr FPSComponent::getTickCallback() {
			return tickCallback;
		}

		const FPSComponent::TickCallbackPtr FPSComponent::getTickCallback() const {
			return tickCallback;
		}

		bool FPSComponent::operator==(const FPSComponent & other) const {
			return Component::operator==(other) && updatesPerSecond == other.updatesPerSecond
				&& framesPerSecond == other.framesPerSecond && cleansPerSecond == other.framesPerSecond
				&& hoversPerSecond == other.hoversPerSecond && tickCallback == other.tickCallback
				&& lastTime == other.lastTime && nbUpdates == other.nbUpdates && nbFrames == other.nbFrames
				&& nbCleans == other.nbCleans && nbHovers == other.nbHovers;
		}

		bool FPSComponent::operator!=(const FPSComponent & other) const {
			return !operator==(other);
		}

		void FPSComponent::init() {
			lastTime = std::time(nullptr);
		}

		bool FPSComponent::update() {
			++nbUpdates;
			if (std::time(nullptr) - lastTime >= 1.0) {
				updatesPerSecond = nbUpdates;
				framesPerSecond = nbFrames;
				cleansPerSecond = nbCleans;
				hoversPerSecond = nbHovers;

				nbFrames = 0;
				nbUpdates = 0;
				nbCleans = 0;
				nbHovers = 0;

				++lastTime;

				tickCallback(this, parent);
			}
			return false;
		}

		void FPSComponent::render() {
			++nbFrames;
		}

		void FPSComponent::clean() {
			++nbCleans;
		}

		void FPSComponent::hover() {
			++nbHovers;
		}

		void FPSComponent::destroy() {}

		AnimationComponent::AnimationComponent(const Texture & tex) : texture(tex) {}

		void AnimationComponent::setTexture(const Texture & tex) {
			texture = tex;
		}

		Texture & AnimationComponent::getTexture() {
			return texture;
		}

		const Texture & AnimationComponent::getTexture() const {
			return texture;
		}

		bool AnimationComponent::operator==(const AnimationComponent & other) const {
			return Component::operator==(other) && texture == other.texture;
		}

		bool AnimationComponent::operator!=(const AnimationComponent & other) const {
			return !operator==(other);
		}
	}//gfx
}//mc
