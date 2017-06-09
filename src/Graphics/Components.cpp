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
