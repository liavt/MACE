/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Core/Constants.h>
#include <MACE/Core/Error.h>
#include <MACE/Utility/Transform.h>
#include <string>

namespace mc {
	namespace gfx {
		void Component::init() {}

		bool Component::update() {
			return true;
		}

		void Component::destroy() {}

		void Component::render() {}

		void Component::clean() {}

		void Component::hover() {}

		Entity * Component::getParent() {
			return parent;
		}

		bool Component::operator==(const Component & other) const {
			return parent == other.parent;
		}

		bool Component::operator!=(const Component & other) const {
			return !operator==(other);
		}

		bool Entity::hasChild(Entity & e) const {
			for (Size i = 0; i < children.size(); ++i) {
				if (children[i].get() == &e) {
					return true;
				}
			}
			return false;
		}

		void Entity::clearChildren() {
			makeDirty();

			while (!children.empty()) {
				children.back()->setParent(nullptr);
				children.pop_back();
			}
		}

		void Entity::makeChildrenDirty() {
			setProperty(Entity::DIRTY, true);

			for (std::shared_ptr<Entity> e : children) {
				e->makeChildrenDirty();
			}
		}

		const std::vector<std::shared_ptr<Entity>>& Entity::getChildren() const {
			return this->children;
		}


		void Entity::removeChild(const Entity& e) {
			removeChild(&e);
		}

		void Entity::removeChild(const Entity * e) {
#ifdef MACE_DEBUG_CHECK_ARGS
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Argument to removeChild is nullptr!");
			} else if (children.empty()) {
				MACE__THROW(AssertionFailed, "Can\'t remove child from an empty entity (empty() is true)");
			}
#endif

			for (Index i = 0; i < children.size(); ++i) {
				if (e == children[i].get()) {
					makeDirty();

					removeChild(i);
					return;
				}
			}

			MACE__THROW(ObjectNotFound, "Specified argument to removeChild is not a valid object in the array!");
		}

		void Entity::removeChild(std::shared_ptr<Entity> ent) {
			removeChild(ent.get());
		}

		void Entity::removeChild(Index index) {
			makeDirty();

			if (index >= children.size()) {
				MACE__THROW(OutOfBounds, std::to_string(index) + " is larger than the amount of children!");
			} else if (children.size() == 1) {
				children.clear();
			} else {
				removeChild(children.begin() + index);
			}
		}

		void Entity::removeChild(const std::vector<std::shared_ptr<Entity>>::iterator& iter){
			if (children.empty()) {
				MACE__THROW(OutOfBounds, "Can\'t remove a child from an empty entity!");
			}
			
			children.erase(iter);
		}

		void Entity::render() {
			if (!getProperty(Entity::INIT)) {
				init();
			}

			//check if we can render
			if (!getProperty(Entity::DISABLED)) {
				//we want to do the actual cleaning in render() because clean() does some graphical work,
				//like updating buffers on the gpu side, so it needs a graphics context
				if (getProperty(Entity::DIRTY)) {
					clean();
				}

				onRender();

				for (Index i = 0; i < children.size(); ++i) {
					if (children[i] != nullptr) {
						children[i]->render();
					}
				}

				for (Index i = 0; i < components.size(); ++i) {
					components[i]->render();
				}
			}

		}

		void Entity::onUpdate() {}

		void Entity::onInit() {}

		void Entity::onDestroy() {}

		void Entity::hover() {
			onHover();
			for (Index i = 0; i < components.size(); ++i) {
				components[i]->hover();
			}
		}

		void Entity::clean() {
			if (getProperty(Entity::DIRTY)) {
				onClean();

				for (Size i = 0; i < children.size(); ++i) {
					if (children[i] == nullptr) {
						removeChild(i--);//to account for the entity beng removed
					} else if (children[i]->getProperty(Entity::INIT)) {
						children[i]->setProperty(Entity::DIRTY, true);
						children[i]->clean();
					}
				}

				for (Index i = 0; i < components.size(); ++i) {
					if (components[i].get() == nullptr) {
						MACE__THROW(NullPointer, "One of the components in an entity was nullptr");
					}

					components[i]->clean();
				}

				setProperty(Entity::DIRTY, false);
			} else {
				for (Size i = 0; i < children.size(); ++i) {
					if (children[i]->getProperty(Entity::INIT)) {
						if (children[i] != nullptr) {
							children[i]->clean();
						}
					}
				}
			}
		}

		Entity * Entity::getRoot() {
			Entity* par = this;

			//get the highest level element
			while (par->parent != nullptr) {
				par = par->parent;
			}

			return par;
		}

		const Entity * Entity::getRoot() const {
			const Entity* par = this;

			//get the highest level element
			while (par->hasParent()) {
				par = par->getParent();
			}

			return par;
		}

		Entity::Metrics Entity::getMetrics() const {
			Entity::Metrics m;
			m.translation = transformation.translation;
			m.rotation = transformation.rotation;
			m.scale = transformation.scaler;

			if (hasParent()) {

				const Entity* par = getParent();

				const Entity::Metrics parentMetrics = par->getMetrics();

				m.inheritedTranslation += parentMetrics.translation + parentMetrics.inheritedTranslation;
				m.inheritedScale *= parentMetrics.scale;
				m.inheritedRotation += parentMetrics.rotation;
			}
			/*
			m.translation *= m.inheritedScale;

			m.scale *= m.inheritedScale;*/

			m.rotation += m.inheritedRotation;

			const Vector<float, 2> windowRatios = gfx::getCurrentWindow()->getContext()->getRenderer()->getWindowRatios();

			if (getProperty(Entity::MAINTAIN_X)) {
				m.translation[0] *= windowRatios[0];
				m.inheritedTranslation[0] *= windowRatios[0];
			}
			if (getProperty(Entity::MAINTAIN_Y)) {
				m.translation[1] *= windowRatios[1];
				m.inheritedTranslation[1] *= windowRatios[1];
			}
			if (getProperty(Entity::MAINTAIN_WIDTH)) {
				m.scale[0] *= windowRatios[0];
				m.inheritedScale[0] *= windowRatios[0];
			}
			if (getProperty(Entity::MAINTAIN_HEIGHT)) {
				m.scale[1] *= windowRatios[1];
				m.inheritedScale[1] *= windowRatios[1];
			}

			return m;
		}

		void Entity::reset() {
			clearChildren();
			properties = 0;
			transformation.reset();

			for (Index i = 0; i < components.size(); ++i) {
				if (components[i].get() != nullptr) {
					components[i]->destroy();
				}
			}
			components.clear();
		}

		void Entity::makeDirty() {
			//checking for the parent can be slow. only want to do the pointer stuff if its not already dirty
			if (!getProperty(Entity::DIRTY)) {
				setProperty(Entity::DIRTY, true);

				getRoot()->setProperty(Entity::DIRTY, true);
			}
		}

		void Entity::onRender() {}

		void Entity::onClean() {}

		void Entity::onHover() {}

		void Entity::setParent(Entity * par) {
			makeDirty();

			this->parent = par;
		}

		Entity* const Entity::getParent() {
			return parent;
		}

		const Entity* const Entity::getParent() const {
			return parent;
		}

		bool Entity::hasParent() const {
			return parent != nullptr;
		}

		Entity& Entity::operator[](Index i) {
			return *children[i].get();
		}

		const Entity& Entity::operator[](Index i) const {
			return *children[i].get();
		}

		Entity& Entity::getChild(Index i) {
			return *children.at(i).get();
		}

		const Entity& Entity::getChild(Index i) const {
			return *children.at(i).get();
		}

		int Entity::indexOf(const Entity & e) const {
			for (Index i = 0; i < children.size(); ++i) {
				if (children[i].get() == &e) {
					return i;
				}
			}
			return -1;
		}

		bool Entity::isEmpty() const {
			return size() == 0;
		}

		std::vector<std::shared_ptr<Entity>>::iterator Entity::begin() {
			return children.begin();
		}

		std::vector<std::shared_ptr<Entity>>::iterator Entity::end() {
			return children.end();
		}

		Size Entity::size() const {
			//size() returns size_t which could be larger than unsigned int on some systems, meaning static_cast is necessary
			return static_cast<Size>(children.size());
		}

		void Entity::kill() {
			destroy();
		}

		void Entity::addChild(std::shared_ptr<Entity> e) {
			if (e == nullptr) {
				MACE__THROW(NullPointer, "Inputted Entity to addChild() was nullptr");
			}

			children.push_back(e);

			e->setParent(this);

			if (getProperty(Entity::INIT) && !e->getProperty(Entity::INIT)) {
				e->init();
			}

			makeDirty();


		}

		void Entity::addChild(Entity* e) {
			addChild(std::shared_ptr<Entity>(e, [](Entity*) {}));
		}

		void Entity::addChild(Entity & e) {
			addChild(&e);
		}

		void Entity::addComponent(Component & com) {
			addComponent(&com);
		}

		void Entity::addComponent(Component * com) {
			addComponent(std::shared_ptr<Component>(com, [](Component*) {}));
		}

		void Entity::addComponent(std::shared_ptr<Component> com) {
			components.push_back(com);
			components.back()->parent = this;
			components.back()->init();

			makeDirty();
		}

		std::vector<std::shared_ptr<Component>> Entity::getComponents() {
			return components;
		}

		void Entity::update() {
			//check if we can update
			if (!getProperty(Entity::DISABLED)) {

				//update the components of this entity
				for (Index i = 0; i < components.size(); ++i) {
					std::shared_ptr<Component> a = components.at(i);
					if (a.get() == nullptr) {
						MACE__THROW(NullPointer, "A component located at index " + std::to_string(i) + " was nullptr");
					}
					if (a->update()) {
						a->destroy();
						components.erase(components.begin() + i--);//update the index after a removal, so we dont get an exception for accessing deleted memory
					}
				}

				onUpdate();

				//call update() on children
				for (Index i = 0; i < children.size(); ++i) {
					if (children[i] == nullptr || children[i]->getProperty(Entity::DEAD)) {
						if (children[i] != nullptr) {
							children[i]->kill();
						}
						removeChild(i--);//update the index after the removal of an element, dont want an error
						continue;
					}
					children[i]->update();
				}
			}
		}

		void Entity::init() {
			if (getProperty(Entity::INIT)) {
				MACE__THROW(InitializationFailed, "Entity can not have init() called twice.");
			}

			makeDirty();
			for (Index i = 0; i < children.size(); ++i) {
				if (children[i] == nullptr) {
					removeChild(i--);//update the index after the removal of an element
					continue;
				}
				children[i]->init();
			}
			onInit();
			setProperty(Entity::INIT, true);

		}

		void Entity::destroy() {
			if (!getProperty(Entity::INIT)) {
				MACE__THROW(InitializationFailed, "Entity can not have destroy() called when it has not been initialized");
			}
			makeDirty();
			setProperty(Entity::DEAD, true);
			for (Index i = 0; i < children.size(); ++i) {
				if (children[i] != nullptr) {
					children[i]->destroy();
				}
			}
			onDestroy();
			reset();
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

		void Entity::setProperties(EntityProperties& b) {
			if (b != properties) {
				makeDirty();
				properties = b;
			}
		}

		bool Entity::getProperty(const Byte flag) const {
			return properties & flag;
		}

		void Entity::setProperty(const Byte position, const bool value) {
			if (((properties & position) != 0) != value) {
				if (position != Entity::DIRTY) {
					properties |= Entity::DIRTY;
				}

				if (value) {
					properties |= position;
				} else {
					properties &= ~position;
				}
			}
		}

		TransformMatrix & Entity::getTransformation() {
			makeDirty();

			return transformation;
		}

		const TransformMatrix & Entity::getTransformation() const {
			return transformation;
		}

		//we are trans-supportive here!
		void Entity::setTransformation(TransformMatrix & trans) {
			if (transformation != trans) {
				makeDirty();

				transformation = trans;
			}
		}

		Entity & Entity::translate(float x, float y, float z) {
			makeDirty();

			transformation.translate(x, y, z);

			return *this;
		}

		Entity & Entity::rotate(float x, float y, float z) {
			makeDirty();

			transformation.rotate(x, y, z);

			return *this;
		}

		Entity & Entity::scale(float x, float y, float z) {
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

		bool Entity::operator!=(const Entity & other) const noexcept {
			return !operator==(other);
		}

		void Group::onInit() {}

		void Group::onUpdate() {}

		void Group::onRender() {}

		void Group::onDestroy() {}

		float & Entity::getWidth() {
			makeDirty();

			return transformation.scaler[0];
		}

		const float & Entity::getWidth() const {
			return transformation.scaler[0];
		}

		void Entity::setWidth(const float & s) {
			if (transformation.scaler[0] != s) {
				makeDirty();

				transformation.scaler[0] = s;
			}
		}

		float & Entity::getHeight() {
			makeDirty();

			return transformation.scaler[1];
		}

		const float & Entity::getHeight() const {
			return transformation.scaler[1];
		}

		void Entity::setHeight(const float & s) {
			if (transformation.scaler[1] != s) {
				makeDirty();

				transformation.scaler[1] = s;
			}
		}

		float & Entity::getDepth() {
			makeDirty();

			return transformation.scaler[2];
		}

		const float & Entity::getDepth() const {
			return transformation.scaler[2];
		}

		void Entity::setDepth(const float & s) {
			if (transformation.scaler[2] != s) {
				makeDirty();

				transformation.scaler[2] = s;
			}
		}

		float & Entity::getX() {
			makeDirty();

			return transformation.translation[0];
		}
		const float & Entity::getX() const {
			return transformation.translation[0];
		}
		void Entity::setX(const float & newX) {
			if (transformation.translation[0] != newX) {
				makeDirty();

				transformation.translation[0] = newX;
			}
		}

		float & Entity::getY() {
			makeDirty();

			return transformation.translation[1];
		}

		const float & Entity::getY() const {
			return transformation.translation[1];
		}

		void Entity::setY(const float & newY) {
			if (transformation.translation[1] != newY) {
				makeDirty();

				transformation.translation[1] = newY;
			}
		}

		float & Entity::getZ() {
			makeDirty();

			return transformation.translation[2];
		}

		const float & Entity::getZ() const {
			return transformation.translation[2];
		}

		void Entity::setZ(const float & newZ) {
			if (transformation.translation[2] != newZ) {
				makeDirty();

				transformation.translation[2] = newZ;
			}
		}

		bool Entity::Metrics::operator==(const Metrics & other) const {
			return translation == other.translation&&scale == other.scale&&rotation == other.rotation
				&&inheritedTranslation == other.inheritedTranslation&&inheritedScale == other.inheritedScale
				&&inheritedRotation == other.inheritedRotation;
		}

		bool Entity::Metrics::operator!=(const Metrics & other) const {
			return !operator==(other);
		}

	}//gfx
}//mc
