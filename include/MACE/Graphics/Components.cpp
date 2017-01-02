#include <MACE/Graphics/Components.h>

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

		void CallbackComponent::setInitCallback(const CallbackPtr func) {
			initCallback = func;
		}
		CallbackComponent::CallbackPtr CallbackComponent::getInitCallback() {
			return initCallback;
		}
		const CallbackComponent::CallbackPtr CallbackComponent::getInitCallback() const {
			return CallbackPtr();
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
	}
}//mc
