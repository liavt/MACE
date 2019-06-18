/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Components.h>
#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>
#include <MACE/Core/Error.h>
#include <MACE/Utility/Transform.h>
#include <string>

namespace mc {
	namespace gfx {
		void Component::init() {}

		bool Component::update() {
			return false;
		}

		void Component::destroy() {}

		void Component::render() {}

		void Component::clean(Metrics&) {}

		void Component::hover() {}

		Entity* Component::getParent() {
			return parent;
		}

		bool Component::operator==(const Component& other) const {
			return parent == other.parent;
		}

		bool Component::operator!=(const Component& other) const {
			return !operator==(other);
		}

		bool Entity::hasChild(Entity& e) const {
			for (Size i = 0; i < children.size(); ++i) {
				if (children[i].get() == &e) {
					return true;
				}
			}
			return false;
		}

		void Entity::clearChildren() {
			makeDirty();

			for (auto iter : children) {
				removeChild(iter);
			}
		}

		void Entity::makeChildrenDirty() {
			setProperty(Entity::DIRTY, true);

			for (auto e : children) {
				e->makeChildrenDirty();
			}
		}

		const std::vector<EntityPtr>& Entity::getChildren() const {
			return this->children;
		}


		void Entity::removeChild(const Entity& e) {
			removeChild(&e);
		}

		void Entity::removeChild(const Entity* e) {
			MACE_ASSERT(e != nullptr, "Argument to removeChild is nullptr!");
			MACE_ASSERT(!children.empty(), "Can\'t remove child from an empty entity (empty() is true)");

			for (Index i = 0; i < children.size(); ++i) {
				if (e == children[i].get()) {
					makeDirty();

					removeChild(i);
					return;
				}
			}

			MACE__THROW(ObjectNotFound, "Specified argument to removeChild is not a valid object in the array!");
		}

		void Entity::removeChild(EntityPtr ent) {
			removeChild(ent.get());
		}

		void Entity::removeChild(const Index index) {
			makeDirty();

			MACE_ASSERT(index < children.size(), std::to_string(index) + " is larger than the amount of children!");

			if (children.size() == 1) {
				children.clear();
			} else {
				removeChild(children.begin() + index);
			}
		}

		void Entity::removeChild(const std::vector<EntityPtr>::iterator& iter) {
			MACE_ASSERT(!children.empty(), "Can\'t remove child from an empty entity (empty() is true)");

			//strange line, isn't it?
			//iter-> returns a std::shared_ptr, which we need to call -> of again to get the raw pointer
			iter->operator->()->destroy();

			children.erase(iter);
		}

		void Entity::render() {
			//check if we can render
			if (!getProperty(Entity::DISABLED)) {
				for (auto& com : components) {
					com.second->render();
				}

				onRender();

				for (Index i = 0; i < children.size(); ++i) {
					children[i]->render();
				}
			}

		}

		void Entity::onUpdate() {}

		void Entity::onInit() {}

		void Entity::onDestroy() {}

		void Entity::hover() {
			onHover();
			for (auto& com : components) {
				com.second->hover();
			}

			//propagate upwards
			Entity* par = getParent();
			while (par != nullptr) {
				par->hover();
				par = par->getParent();
			}
		}

		void Entity::clean() {
			if (!getProperty(Entity::INIT)) {
				init();
			}

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

					for (auto& com : components) {
						com.second->clean(metrics);
					}

				for (Size i = 0; i < children.size(); ++i) {
					std::shared_ptr<Entity> child = children[i];
					child->setProperty(Entity::DIRTY, true);
					child->clean();
				}

				setProperty(Entity::DIRTY, false);
			} else {
				for (Size i = 0; i < children.size(); ++i) {
					children[i]->clean();
				}
			}
		}

		RootEntity* Entity::getRoot() {
			return root;
		}

		const RootEntity* Entity::getRoot() const {
			return root;
		}

		const Metrics& Entity::getMetrics() const {
			return metrics;
		}

		void Entity::makeDirty() {
			if (!getProperty(Entity::DIRTY)) {
				setProperty(Entity::DIRTY, true);

				if (root != nullptr) {
					root->setProperty(Entity::DIRTY, true);
				}
			}
		}

		void Entity::onRender() {}

		void Entity::onClean() {}

		void Entity::onHover() {}

		Entity* const Entity::getParent() {
			return parent;
		}

		const Entity* const Entity::getParent() const {
			return parent;
		}

		bool Entity::hasParent() const {
			return parent != nullptr;
		}

		bool Entity::needsRemoval() const {
			return getProperty(Entity::DEAD) || !hasParent();
		}

		bool Entity::isInit() const {
			return getProperty(EntityProperty::INIT);
		}

		Entity& Entity::operator[](const Index i) {
			return *children[i].get();
		}

		const Entity& Entity::operator[](const Index i) const {
			return *children[i].get();
		}

		Entity& Entity::getChild(const Index i) {
			return *children.at(i).get();
		}

		const Entity& Entity::getChild(const Index i) const {
			return *children.at(i).get();
		}

		Index Entity::indexOf(const Entity& e) const {
			for (Index i = 0; i < children.size(); ++i) {
				if (children[i].get() == &e) {
					return i;
				}
			}
			return Index(-1);
		}

		bool Entity::isEmpty() const {
			return size() == 0;
		}

		std::vector<EntityPtr>::iterator Entity::begin() {
			return children.begin();
		}

		std::vector<EntityPtr>::iterator Entity::end() {
			return children.end();
		}

		Size Entity::size() const {
			//size() returns size_t which could be larger than unsigned int on some systems, meaning static_cast is necessary
			return static_cast<Size>(children.size());
		}

		EntityID Entity::getID() const {
			return id;
		}

		void Entity::kill() {
			destroy();
		}

		void Entity::addChild(EntityPtr e) {
			MACE_ASSERT(e != nullptr, "Inputted Entity to addChild() was nullptr");

			e->parent = this;

			if (getProperty(Entity::INIT) && !e->getProperty(Entity::INIT)) {
				e->init();
			}

			children.push_back(e);

			makeDirty();
		}

		void Entity::addChild(Entity* e) {
			addChild(EntityPtr(e, [](Entity*) {}));
		}

		void Entity::addChild(Entity& e) {
			addChild(&e);
		}

		void Entity::update() {
			//check if we can update
			if (!getProperty(Entity::DISABLED)) {

				//update the components of this entity
				for (auto& com : components) {
					if (com.second->update()) MACE_UNLIKELY{
						com.second->destroy();
						components.erase(com.first);
					}
				}

				onUpdate();

				//call update() on children
				for (Index i = 0; i < children.size(); ++i) {
					EntityPtr child = children[i];
					if (child->needsRemoval()) MACE_UNLIKELY{
						child->kill();
						removeChild(i--);//update the index after the removal of an element, dont want an error
						continue;
					}
					child->update();
				}
			}
		}

		void Entity::init() {
			if (getProperty(Entity::INIT)) {
				MACE__THROW(InitializationFailed, "Entity can not have init() called twice.");
			}

			if (root == nullptr) {
				if (parent == nullptr) {
					MACE__THROW(NullPointer, "Entity parent was nullptr");
				} else if (parent->root == nullptr) {
					MACE__THROW(NullPointer, "Entity parent's root was nullptr");
				}

				root = parent->root;
			}

			id = root->getIDManager()->generateID(this);

			makeDirty();
			for (Index i = 0; i < children.size(); ++i) {
				EntityPtr child = children[i];
				if (child->needsRemoval()) MACE_UNLIKELY{
					removeChild(i--);//update the index after the removal of an element
					continue;
				}
				child->init();
			}

			//Component::init is called on addComponent, so it doesn't need to be called here

			onInit();
			setProperty(Entity::INIT, true);
		}

		void Entity::destroy() {
			//Component::destroy is called in reset() before components.clear()

			if (getProperty(Entity::INIT)) {
				setProperty(Entity::DEAD, true);

				onDestroy();

				for (auto& component : components) {
					component.second->destroy();
				}
				components.clear();

				for (auto& child : children) {
					child->destroy();
				}
				children.clear();

				root->setProperty(Entity::DIRTY, true);

				root->getIDManager()->deleteID(id);

				transformation.reset();
				parent = nullptr;
				root = nullptr;
				id = 0;
				properties = Entity::DEFAULT_PROPERTIES;
			}
		}

		Entity::Entity() noexcept {}

		Entity::~Entity() noexcept {
			children.clear();
		}


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

		bool Entity::getProperty(const Byte flag) const {
			return os::bittest(properties, flag);
		}

		void Entity::setProperty(const Byte position, const bool value) {
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

		Entity* IDManager::getEntityByID(const EntityID id) const {
			if (id == 0) {
				return nullptr;
			} else if (id > ids.size()) {
				return nullptr;
			}

			return ids[id - 1];
		}

		EntityID IDManager::generateID(Entity* e) {
			for (EntityID i = 0; i < static_cast<EntityID>(ids.size()); ++i) {
				if (ids[i] == nullptr) {
					ids[i] = e;
					//0 is reserved for an unitialized Entity, so add 1 to the interator
					return i + 1;
				}
			}

			ids.push_back(e);

			return static_cast<EntityID>(ids.size());
		}

		void IDManager::deleteID(const EntityID id) {
			if (id > 0 && id <= ids.size()) {
				ids[id - 1] = nullptr;
			}
		}

		std::shared_ptr<IDManager> RootEntity::getIDManager() {
			return idManager;
		}

		const std::shared_ptr<IDManager> RootEntity::getIDManager() const {
			return idManager;
		}

		void RootEntity::init() {
			root = this;
			idManager = std::shared_ptr<IDManager>(new IDManager());

			Entity::init();
		}

		void RootEntity::destroy() {
			Entity::destroy();

			idManager.reset();
			root = nullptr;
		}

	}//gfx
}//mc
