#include <MACE/Graphics/Components.h>
#include <ctime>

namespace mc {
	namespace gfx {
		void CallbackComponent::init(Entity * e) {
			initCallback(e);
		}

		bool CallbackComponent::update(Entity * e) {
			return updateCallback(e);
		}

		void CallbackComponent::render(Entity * e) {
			renderCallback(e);
		}

		void CallbackComponent::destroy(Entity * e) {
			destroyCallback(e);
		}

		void CallbackComponent::hover(Entity * e) {
			hoverCallback(e);
		}

		void CallbackComponent::clean(Entity * e) {
			cleanCallback(e);
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

		void FPSComponent::init(Entity *) {
			lastTime = time(0);
		}

		bool FPSComponent::update(Entity * e) {
			++nbUpdates;
			if( time(0) - lastTime >= 1.0 ) {
				updatesPerSecond = nbUpdates;
				framesPerSecond = nbFrames;
				cleansPerSecond = nbCleans;
				hoversPerSecond = nbHovers;

				nbFrames = 0;
				nbUpdates = 0;
				nbCleans = 0;
				nbHovers = 0;

				lastTime += 1;

				tickCallback(this, e);
			}
			return false;
		}

		void FPSComponent::render(Entity *) {
			++nbFrames;
		}

		void FPSComponent::clean(Entity *) {
			++nbCleans;
		}

		void FPSComponent::hover(Entity *) {
			++nbHovers;
		}
		void FPSComponent::destroy(Entity *) {}
	}
}//mc
