/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/RenderTarget.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Components.h>
#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>
#include <MACE/Core/Error.h>
#include <MACE/Utility/Transform.h>
#include <string>
#include <algorithm>
#include <execution>

#define MACE__ASSERT_CHILD_NOT_NULL(child) MACE_ASSERT(child != nullptr, "Internal Error: Child was nullptr during iteration")

namespace mc {
	namespace gfx {
		void Component::init() {}

		void Component::update() {}

		void Component::render() {}

		void Component::clean(Metrics&) {}

		bool Component::isDone() const {
			return false;
		}

		EventListener::~EventListener() noexcept {
			disconnect();
		}

		void EventListener::disconnect() noexcept {
			if (auto lockedNode = node.lock()) {
				lockedNode->connected = false;
			}
		}

		bool EventListener::isInit() const noexcept {
			std::shared_ptr< MACE__INTERNAL_NS::EventListenerNodeBase> lockedNode = node.lock();
			return lockedNode && lockedNode->connected;
		}

		void EventListener::init() {
			//do nothing as initialization is done in constructor
		}

		EventListener::EventListener(std::weak_ptr<MACE__INTERNAL_NS::EventListenerNodeBase> n) : node(n) {}

		void EventListenerManager::manage(EventListener&& listener) {
			eventListeners.push_front(listener);
		}

		EventListenerManager::~EventListenerManager(){
			disconnect();
		}

		void EventListenerManager::disconnect() noexcept{
			while (!eventListeners.empty()) {
				eventListeners.front().disconnect();
				eventListeners.pop_front();
			}
		}

		Entity* Component::getParent() {
			return parent;
		}

		bool Component::isInit() const noexcept {
			return parent != nullptr;
		}

		bool Component::operator==(const Component& other) const {
			return parent == other.parent;
		}

		bool Component::operator!=(const Component& other) const {
			return !operator==(other);
		}

		void Entity::clearChildren() {
			makeDirty();

			children.clear();
		}

		void Entity::removeFromParent() {
			const EntityID myID = getID();
			children.erase(std::remove_if(std::execution::par_unseq, parent->children.begin(), parent->children.end(), [myID](EntityPtr<Entity> en) {
				MACE__ASSERT_CHILD_NOT_NULL(en);
				return en->getID() == myID;
			}));
		}

		void Entity::makeChildrenDirty() {
			setProperty(Entity::DIRTY, true);

			for (auto e : children) {
				MACE__ASSERT_CHILD_NOT_NULL(e);
				e->makeChildrenDirty();
			}
		}

		const std::vector<EntityPtr<Entity>>& Entity::getChildren() const {
			return this->children;
		}

		void Entity::onUpdate() {}

		void Entity::onInit() {}

		Entity* Entity::getRoot() {
			return root;
		}

		const Entity* Entity::getRoot() const {
			return root;
		}

		const Metrics& Entity::getMetrics() const {
			return metrics;
		}

		void Entity::makeDirty() noexcept {
			if (!getProperty(Entity::DIRTY)) {
				setProperty(Entity::DIRTY, true);

				if (root != nullptr) {
					root->setProperty(Entity::DIRTY, true);
				}
			}
		}

		void Entity::onRender() {}

		void Entity::onClean() {}

		Entity* const Entity::getParent() {
			return parent;
		}

		const Entity* const Entity::getParent() const {
			return parent;
		}

		bool Entity::hasParent() const {
			return parent != nullptr;
		}

		bool Entity::isInit() const noexcept {
			return getProperty(EntityProperty::INIT);
		}

		bool Entity::isEmpty() const {
			return size() == 0;
		}

		std::vector<EntityPtr<Entity>>::iterator Entity::begin() {
			return children.begin();
		}

		std::vector<EntityPtr<Entity>>::iterator Entity::end() {
			return children.end();
		}

		Size Entity::size() const {
			//size() returns size_t which could be larger than unsigned int on some systems, meaning static_cast is necessary
			return static_cast<Size>(children.size());
		}

		EntityID Entity::getID() const {
			return id;
		}

		void Entity::addChild(EntityPtr<Entity> e) {
			MACE_ASSERT(e != nullptr, "Added Entity was nullptr");

			e->parent = this;
			if (isInit()) {
				e->init();
			}
			children.push_back(e);

			makeDirty();
		}

		void Entity::addChild(Entity* e) {
			addChild(EntityPtr<Entity>(e, [](Entity*) {}));
		}

		void Entity::addChild(Entity& e) {
			addChild(&e);
		}

		void Entity::render() {
			//check if we can render
			if (!getProperty(Entity::DISABLED)) {
				for (auto com : components) {
					com.second->render();
				}

				onRender();

				for (auto child : children) {
					MACE__ASSERT_CHILD_NOT_NULL(child);
					child->render();
				}
			}

		}

		void Entity::clean() {
			if (getProperty(Entity::DIRTY)) {
				onClean();

				metrics.transform = transformation;

				metrics.inherited = Transformation();
				//root doesn't have a parent
				if (hasParent()) MACE_LIKELY{
					const Entity * par = getParent();

					const Metrics& parentMetrics = par->getMetrics();

					metrics.inherited.translation += parentMetrics.transform.translation + parentMetrics.inherited.translation;
					metrics.inherited.scaler *= parentMetrics.transform.scaler;
					metrics.inherited.rotation += parentMetrics.transform.rotation;
				}

				for (auto com = components.begin(); com != components.end();) {
					com->second->clean(metrics);
					if (com->second->isDone()) MACE_UNLIKELY{
						com->second->parent = nullptr;
						components.erase(com++);
					} else {
						++com;
					}
				}

				for (auto child : children) {
					MACE__ASSERT_CHILD_NOT_NULL(child);
					child->setProperty(Entity::DIRTY, true);
					child->clean();
				}

				setProperty(Entity::DIRTY, false);
			} else {
				for (auto child : children) {
					MACE__ASSERT_CHILD_NOT_NULL(child);
					child->clean();
				}
			}
		}

		void Entity::update() {
			//check if we can update
			if (!getProperty(Entity::DISABLED)) {
				//update the components of this entity
				for (auto com : components) {
					com.second->update();
					if (com.second->isDone()) MACE_UNLIKELY{
						//it only gets deleted in clean() so we need to notify the render thread to check
						makeDirty();
					}
				}

				onUpdate();

				//call update() on children
				for (auto child : children) {
					MACE__ASSERT_CHILD_NOT_NULL(child);
					child->update();
				}
			}
		}

		void Entity::init() {
			if (getProperty(Entity::INIT)) {
				MACE__THROW(InitializationFailed, "Entity can not have init() called twice.");
			}

			setProperty(Entity::INIT, true);

			makeDirty();

			if (root == nullptr) {
				if (parent != nullptr) {
					MACE_ASSERT(parent->root != nullptr, "Parent root was nullptr");

					root = parent->root;
				} else {
					root = this;
				}
			}

			id = root->getOrCreateComponent<MACE__INTERNAL_NS::RootComponent>()->generateID(this);

			for (auto child : children) {
				MACE__ASSERT_CHILD_NOT_NULL(child);
				child->init();
			}

			for (auto com : components) {
				com.second->init();
			}

			onInit();
		}

		Entity::~Entity() {
			if (isInit() && root->isInit()) {
				root->setProperty(Entity::DIRTY, true);

				// if this is root, the RootComponent would have already been destroyed already.
				if (root != this) {
					root->getComponent<MACE__INTERNAL_NS::RootComponent>()->deleteID(id);
				}
			}
		}

		Entity::Entity() noexcept {}

		EntityProperties& Entity::getProperties() {
			makeDirty();

			return properties;
		}

		const EntityProperties& Entity::getProperties() const {
			return properties;
		}

		void Entity::setProperties(const EntityProperties& b) {
			if (b != properties) {
				makeDirty();
				properties = b;
			}
		}

		bool Entity::getProperty(const Byte flag) const noexcept {
			return os::bittest(properties, flag);
		}

		void Entity::setProperty(const Byte position, const bool value) noexcept {
			if (getProperty(position) != value) {
				if (position != Entity::DIRTY) {
					properties |= (1 << Entity::DIRTY);
				}

				if (value) {
					properties |= (1 << position);
				} else {
					properties &= ~(1 << position);
				}
			}
		}

		Transformation& Entity::getTransformation() {
			makeDirty();

			return transformation;
		}

		const Transformation& Entity::getTransformation() const {
			return transformation;
		}

		//we are trans-supportive here!
		void Entity::setTransformation(const Transformation& trans) {
			if (transformation != trans) {
				makeDirty();

				transformation = trans;
			}
		}

		Entity& Entity::translate(const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z) {
			makeDirty();

			transformation.translate(x, y, z);

			return *this;
		}

		Entity& Entity::rotate(const RelativeRadian x, const RelativeRadian y, const RelativeRadian z) {
			makeDirty();

			transformation.rotate(x, y, z);

			return *this;
		}

		Entity& Entity::scale(const RelativeScale x, const RelativeScale y, const RelativeScale z) {
			makeDirty();

			transformation.scale(x, y, z);

			return *this;
		}

		bool Entity::operator==(const Entity& other) const noexcept {
			if (other.properties != properties) {
				return false;
			} else if (other.parent != parent) {
				return false;
			} else if (other.transformation != transformation) {
				return false;
			} else if (other.components != components) {
				return false;
			}

			return children == other.children;
		}

		bool Entity::operator!=(const Entity& other) const noexcept {
			return !operator==(other);
		}

		RelativeScale& Entity::getWidth() {
			makeDirty();

			return transformation.scaler[0];
		}

		const RelativeScale& Entity::getWidth() const {
			return transformation.scaler[0];
		}

		void Entity::setWidth(const RelativeScale s) {
			if (transformation.scaler[0] != s) {
				makeDirty();

				transformation.scaler[0] = s;
			}
		}

		RelativeScale& Entity::getHeight() {
			makeDirty();

			return transformation.scaler[1];
		}

		const RelativeScale& Entity::getHeight() const {
			return transformation.scaler[1];
		}

		void Entity::setHeight(const RelativeScale s) {
			if (transformation.scaler[1] != s) {
				makeDirty();

				transformation.scaler[1] = s;
			}
		}

		RelativeScale& Entity::getDepth() {
			makeDirty();

			return transformation.scaler[2];
		}

		const RelativeScale& Entity::getDepth() const {
			return transformation.scaler[2];
		}

		void Entity::setDepth(const RelativeScale s) {
			if (transformation.scaler[2] != s) {
				makeDirty();

				transformation.scaler[2] = s;
			}
		}

		RelativeTranslation& Entity::getX() {
			makeDirty();

			return transformation.translation[0];
		}
		const RelativeTranslation& Entity::getX() const {
			return transformation.translation[0];
		}
		void Entity::setX(const RelativeTranslation newX) {
			if (transformation.translation[0] != newX) {
				makeDirty();

				transformation.translation[0] = newX;
			}
		}

		RelativeTranslation& Entity::getY() {
			makeDirty();

			return transformation.translation[1];
		}

		const RelativeTranslation& Entity::getY() const {
			return transformation.translation[1];
		}

		void Entity::setY(const RelativeTranslation newY) {
			if (transformation.translation[1] != newY) {
				makeDirty();

				transformation.translation[1] = newY;
			}
		}

		RelativeTranslation& Entity::getZ() {
			makeDirty();

			return transformation.translation[2];
		}

		const RelativeTranslation& Entity::getZ() const {
			return transformation.translation[2];
		}

		void Entity::setZ(const RelativeTranslation newZ) {
			if (transformation.translation[2] != newZ) {
				makeDirty();

				transformation.translation[2] = newZ;
			}
		}

		void Entity::tween(const Transformation start, const Transformation dest, const EaseSettings settings) {
			addComponent(ComponentPtr<TweenComponent>(new TweenComponent(this, start, dest, settings)));
		}

		void Entity::tween(const Transformation start, const Transformation dest) {
			tween(start, dest, EaseSettings());
		}

		void Entity::tween(const Transformation dest, const EaseSettings settings) {
			tween(getTransformation(), dest, settings);
		}

		void Entity::tween(const Transformation dest) {
			tween(dest, EaseSettings());
		}

		bool Metrics::operator==(const Metrics& other) const {
			return transform == other.transform && inherited == other.inherited;
		}

		bool Metrics::operator!=(const Metrics& other) const {
			return !operator==(other);
		}
	}//gfx

	namespace internal {
		gfx::Entity* RootComponent::getEntityByID(const gfx::EntityID id) const {
			if (id == 0) {
				return nullptr;
			} else if (id > ids.size()) {
				return nullptr;
			}

			return ids[id - 1];
		}

		gfx::EntityID RootComponent::generateID(gfx::Entity* e) {
			for (gfx::EntityID i = 0; i < static_cast<gfx::EntityID>(ids.size()); ++i) {
				if (ids[i] == nullptr) {
					ids[i] = e;
					//0 is reserved for an unitialized Entity, so add 1 to the interator
					return i + 1;
				}
			}

			ids.push_back(e);

			return static_cast<gfx::EntityID>(ids.size());
		}

		void RootComponent::deleteID(const gfx::EntityID id) {
			if (id > 0 && id <= ids.size()) {
				ids[id - 1] = nullptr;
			}
		}

		void RootComponent::init() {
			ids.clear();
		}

		void RootComponent::destroy() {
			ids.clear();
		}
	}//internal
}//mc

#undef MACE__ASSERT_CHILD_NOT_NULL