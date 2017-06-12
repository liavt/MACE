/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Components.h>
#include <ctime>

namespace mc {
	namespace gfx {
		namespace EaseFunctions {
			//these fucntions are derived from https://github.com/jesusgollonet/ofpennereasing . Thank you!


			const EaseFunction LINEAR = [](float t, const float b, const float c, const float d) -> float {
				return b + c*(t / d);
			};

			const EaseFunction BACK_IN = [](float t, const float b, const float c, const float d) -> float {
				const float s = 1.70158f;
				const float postFix = t /= d;
				return c*(postFix)*t*((s + 1)*t - s) + b;
			};

			//shameless resturant promotion
			const EaseFunction BACK_OUT = [](float t, const float b, const float c, const float d) -> float {
				const float s = 1.70158f;
				t /= d;
				return c*((t - 1)*t*((s + 1)*t + s) + 1) + b;
			};

			const EaseFunction BACK_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				const float s = 2.5949095f;
				if ((t /= d / 2) < 1) return c / 2 * (t*t*((s + 1)*t - s)) + b;
				const float postFix = t -= 2;
				return c / 2 * ((postFix)*t*((s + 1)*t + s) + 2) + b;
			};

			const EaseFunction BOUNCE_OUT = [](float t, const float b, const float c, const float d) -> float {
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
			};

			//BOUNCE_IN is defined after BOUNCE_OUT just because it requires BOUNCE_OUT to be eclared
			const EaseFunction BOUNCE_IN = [](float t, const float b, const float c, const float d) -> float {
				return c - EaseFunctions::BOUNCE_OUT(d - t, 0, c, d) + b;
			};

			const EaseFunction BOUNCE_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t < d / 2) return EaseFunctions::BOUNCE_IN(t * 2, 0, c, d) * .5f + b;
				else return EaseFunctions::BOUNCE_OUT(t * 2 - d, 0, c, d) * .5f + c*.5f + b;
			};

			const EaseFunction CIRCLE_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return -c * (std::sqrt(1 - t*t) - 1) + b;
			};

			const EaseFunction CIRCLE_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c * std::sqrt(1 - (t - 1)*t) + b;
			};

			const EaseFunction CIRCLE_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) {
					return -c / 2 * (std::sqrt(1 - t*t) - 1) + b;
				}

				t -= 2;
				return c / 2 * (std::sqrt(1 - t*t) + 1) + b;
			};

			const EaseFunction CUBIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*t*t*t + b;
			};

			const EaseFunction CUBIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*((t - 1)*t*t + 1) + b;
			};

			const EaseFunction CUBIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) {
					return c / 2 * t*t*t + b;
				}

				t -= 2;
				return c / 2 * (t*t*t + 2) + b;
			};

			const EaseFunction ELASTIC_IN = [](float t, const float b, const float c, const float d) -> float {
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
			};

			const EaseFunction ELASTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) {
					return b;
				} else if ((t /= d) == 1) {
					return b + c;
				}

				const float p = d*.3f;
				const float s = p / 4;
				return static_cast<float>(c*std::pow(2, -10 * t) * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p) + c + b);
			};

			const EaseFunction ELASTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
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
			};

			const EaseFunction EXPONENTIAL_IN = [](float t, const float b, const float c, const float d) -> float {
				return (t == 0) ? b : c * static_cast<float>(std::pow(2, 10 * (t / d - 1))) + b;
			};

			const EaseFunction EXPONENTIAL_OUT = [](float t, const float b, const float c, const float d) -> float {
				return (t == d) ? b + c : c * static_cast<float>(-std::pow(2, -10 * t / d) + 1) + b;
			};

			const EaseFunction EXPONENTIAL_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) return b;
				if (t == d) return b + c;
				if ((t /= d / 2) < 1) return c / 2 * static_cast<float>(std::pow(2, 10 * (t - 1))) + b;
				return c / 2 * static_cast<float>(-std::pow(2, -10 * --t) + 2) + b;
			};

			const EaseFunction QUADRATIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*t*t + b;
			};

			const EaseFunction QUADRATIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return -c *t*(t - 2) + b;
			};

			const EaseFunction QUADRATIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return ((c / 2)*(t*t)) + b;
				--t;
				return -c / 2 * (((t - 1)*(t)) - 1) + b;
			};

			const EaseFunction QUARTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*(t)*t*t*t + b;
			};

			const EaseFunction QUARTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return -c * ((t - 1)*t*t*t - 1) + b;
			};

			const EaseFunction QUARTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				if ((t / 2) < 1) {
					return c / 2 * t*t*t*t + b;
				}
				return -c / 2 * (t*t*t*t - 2) + b;
			};

			const EaseFunction QUINTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*t*t*t*t*t + b;
			};

			const EaseFunction QUINTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*((t - 1)*t*t*t*t + 1) + b;
			};

			const EaseFunction QUINTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				if ((t / 2) < 1) {
					return c / 2 * t*t*t*t*t + b;
				}
				t -= 2;
				return c / 2 * (t*t*t*t*t + 2) + b;
			};

			const EaseFunction SINUSODIAL_IN = [](float t, const float b, const float c, const float d) -> float {
				return -c * std::cos(t / d * (static_cast<float>(math::pi()) / 2)) + c + b;
			};

			const EaseFunction SINUSODIAL_OUT = [](float t, const float b, const float c, const float d) -> float {
				return c * std::sin(t / d * (static_cast<float>(math::pi()) / 2)) + b;
			};

			const EaseFunction SINUSODIAL_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				return -c / 2 * (std::cos(static_cast<float>(math::pi())*t / d) - 1) + b;
			};
		}

		AlignmentComponent::AlignmentComponent(const VerticalAlign vert, const HorizontalAlign horz)
			: vertAlign(vert), horzAlign(horz) {}

		void AlignmentComponent::setVerticalAlign(const VerticalAlign align) {
			if (vertAlign != align) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				vertAlign = align;
			}
		}

		const VerticalAlign AlignmentComponent::getVerticalAlign() const {
			return vertAlign;
		}

		void AlignmentComponent::setHorizontalAlign(HorizontalAlign align) {
			if (horzAlign != align) {
				if (getParent() != nullptr) {
					getParent()->makeDirty();
				}

				horzAlign = align;
			}
		}

		const HorizontalAlign AlignmentComponent::getHorizontalAlign() const {
			return horzAlign;
		}

		bool AlignmentComponent::operator==(const AlignmentComponent & other) const {
			return horzAlign == other.horzAlign && vertAlign == other.vertAlign && Component::operator==(other);
		}

		bool AlignmentComponent::operator!=(const AlignmentComponent & other) const {
			return !operator==(other);
		}

		void AlignmentComponent::clean() {
			Entity::Metrics m = parent->getMetrics();

			float width = m.scale[0], height = m.scale[1];

			switch (horzAlign) {
				default:
				case HorizontalAlign::CENTER:
					parent->setX(0.0f);
					//e.setX((-width / 2) + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
				case HorizontalAlign::RIGHT:
					parent->setX(1.0f - (width / 2.0f));
					break;
				case HorizontalAlign::LEFT:
					parent->setX((width / 2.0f) - 1.0f);
					break;
			}

			switch (vertAlign) {
				default:
				case VerticalAlign::CENTER:
					parent->setX(0.0f);
					break;
				case VerticalAlign::BOTTOM:
					parent->setY((height / 2.0f) - 1.0f);
					break;
				case VerticalAlign::TOP:
					parent->setY(1.0f - (height / 2.0f));
					break;
			}
		}

		EaseComponent::EaseComponent(const float duration, const float startingProgress, const float destination, const EaseUpdateCallback callback, const EaseFunction easeFunction, const EaseDoneCallback doneCallback)
			: Component(), t(0), b(startingProgress), c(destination), d(duration), updateCallback(callback), ease(easeFunction), done(doneCallback) {}

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

		PointerComponent::PointerComponent(Component * com) : ptr(com) {}

		Component * PointerComponent::get() {
			return ptr;
		}

		const Component * PointerComponent::get() const {
			return ptr;
		}

		Component * PointerComponent::operator*() {
			return ptr;
		}

		const Component * PointerComponent::operator*() const {
			return ptr;
		}

		Component * PointerComponent::operator->() {
			return ptr;
		}

		const Component * PointerComponent::operator->() const {
			return ptr;
		}

		void PointerComponent::init() {
			if (ptr == nullptr) {
				throw NullPointerError("PointerComponent: pointer was nullptr in init()");
			}

			ptr->parent = this->parent;

			ptr->init();
		}

		bool PointerComponent::update() {
			if (ptr == nullptr) {
				throw NullPointerError("PointerComponent: pointer was nullptr in update()");
			}

			return ptr->update();
		}

		void PointerComponent::render() {
			if (ptr == nullptr) {
				throw NullPointerError("PointerComponent: pointer was nullptr in render()");
			}

			ptr->render();
		}

		void PointerComponent::hover() {
			if (ptr == nullptr) {
				throw NullPointerError("PointerComponent: pointer was nullptr in hover()");
			}

			ptr->hover();
		}

		void PointerComponent::clean() {
			if (ptr == nullptr) {
				throw NullPointerError("PointerComponent: pointer was nullptr in clean()");
			}

			ptr->clean();
		}

		void PointerComponent::destroy() {
			if (ptr == nullptr) {
				throw NullPointerError("PointerComponent: pointer was nullptr in destroy()");
			}

			ptr->destroy();

			delete ptr;

			delete this;
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
			return destroyCallback == other.destroyCallback && renderCallback == other.renderCallback
				&& initCallback == other.initCallback && hoverCallback == other.hoverCallback
				&& cleanCallback == other.cleanCallback && updateCallback == other.updateCallback
				&& Component::operator==(other);
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
		Index FPSComponent::getUpdatesPerSecond() const {
			return updatesPerSecond;
		}
		Index FPSComponent::getFramesPerSecond() const {
			return framesPerSecond;
		}
		Index FPSComponent::getCleansPerSecond() const {
			return cleansPerSecond;
		}
		Index FPSComponent::getHoversPerSecond() const {
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
			lastTime = std::time(0);
		}

		bool FPSComponent::update() {
			++nbUpdates;
			if (std::time(0) - lastTime >= 1.0) {
				updatesPerSecond = nbUpdates;
				framesPerSecond = nbFrames;
				cleansPerSecond = nbCleans;
				hoversPerSecond = nbHovers;

				nbFrames = 0;
				nbUpdates = 0;
				nbCleans = 0;
				nbHovers = 0;

				lastTime += 1;

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
	}//gfx
}//mc
