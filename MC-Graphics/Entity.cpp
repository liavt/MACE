/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MC-Graphics/Entity.h>
#include <MC-System/Exceptions.h>
#include <MC-System/Constants.h>

namespace mc {
	namespace gfx {
		Container::Container()
		{
		}


		Container::~Container()
		{
			children.clear();
			destroy();
		}

		void Container::updateChildren()
		{
			for (Index i = 0; i < children.size(); i++) {
				if (children[i]->getProperty(ENTITY_PROPERTY_DEAD)) {
					children[i]->kill();
					removeChild(i);
					i--;//update the index after the removal of an element
					return;
				}
				children[i]->update();
			}
		}

		void Container::initChildren()
		{
			for (Index i = 0; i < children.size(); i++) {
				if (!children[i]->getProperty(ENTITY_PROPERTY_INIT))children[i]->init();

			}
		}

		bool Container::hasChild(Entity & e) const
		{
			for (Size i = 0; i < children.size(); i++) {
				if (children[i] == &e) {
					return true;
				}
			}
			return false;
		}

		void Container::clearChildren()
		{
			children.clear();
		}

		void Container::destroyChildren()
		{
			for (Index i = 0; i < children.size(); i++) {
				children[i]->destroy();
			}
		}

		void Container::renderChildren()
		{
			for (Index i = 0; i < children.size(); i++) {
				children[i]->render();
			}
		}

		const std::vector<Entity*>& Container::getChildren() const
		{
			return this->children;
		}


		void Container::addChild(Entity& e)
		{
			children.push_back(&e);
			e.setParent(this);
		}

		void Container::removeChild(const Entity& e)
		{
			for (Size i = 0; i < children.size(); i++) {
				if (&e == children.at(i)) {
					removeChild(i);
					return;
				}
			}
			throw mc::ObjectNotFoundInArray("Specified argument is not a valid object in the array!");
		}

		void Container::removeChild(Index index) {
			if (index > children.size()) {
				throw mc::IndexOutOfBounds(index + " is larger than the amount of children!");
			}
			else if (children.size() == 1) {
				children.clear();
			}
			else {
				children.erase(children.begin() + index);
			}
		}

		void Entity::render()
		{
			customRender();
			Container::render();
		}

		void Entity::setParent(Container * parent)
		{
			this->parent = parent;
			setProperty(ENTITY_PROPERTY_INIT, false);
		}

		Container& Entity::getParent() {
			return *parent;
		}

		const Container& Entity::getParent() const {
			return *parent;
		}

		Entity& Container::operator[](Index i) {
			return *children[i];
		}

		const Entity& Container::operator[](Index i) const
		{
			return *children[i];
		}

		Entity& Container::getChild(Index i) {
			return *children.at(i);
		}

		const Entity& Container::getChild(Index i) const
		{
			return *children.at(i);
		}

		Index Container::indexOf(const Entity & e) const
		{
			for (Index i = 0; i < children.size(); i++) {
				if (children[i] == &e) {
					return i;
				}
			}
			return -1;
		}

		std::vector<Entity*>::iterator Container::begin()
		{
			return children.begin();
		}

		std::vector<Entity*>::iterator Container::end()
		{
			return children.end();
		}

		Size Container::size() const
		{
			return children.size();
		}

		void Container::update() {
			updateChildren();
		}

		void Container::init() {
			initChildren();
		}

		void Container::destroy() {
			destroyChildren();
		}

		void Container::render()
		{
			renderChildren();
		}

		void Entity::kill() {
			destroy();
		}

		void Entity::update() {
			if (getProperty(ENTITY_PROPERTY_UPDATE_ENABLED)) {
				if (getProperty(ENTITY_PROPERTY_DIRTY)) {
					clean();
				}
				customUpdate();
				Container::update();
			}
		}

		void Entity::init()
		{
			customInit();
			Container::init();
			setProperty(ENTITY_PROPERTY_INIT, true);
		}

		void Entity::destroy()
		{
			customDestroy();
			Container::destroy();
		}

		Entity::Entity() :Container()
		{

		}
		Entity::Entity(const Entity & obj) : Container(obj)
		{
			properties = obj.properties;
		}

		Entity::~Entity()
		{
		}

		void Entity::clean()
		{
			setProperty(ENTITY_PROPERTY_DIRTY, false);
		}

		ByteField& Entity::getProperties() {
			return properties;
		}

		const ByteField& Entity::getProperties() const
		{
			return properties;
		}

		void Entity::setProperties(ByteField& b)
		{
			properties = b;
		}
		bool Entity::getProperty(Index position) const
		{
			if (position > properties.size())throw IndexOutOfBounds("Input position is greater than 8");
			else if (position < 0)throw IndexOutOfBounds("Input position is less than 0!");
			return properties.getBit(position);
		}
		void Entity::setProperty(Index position, bool value)
		{
			if (position > properties.size())throw IndexOutOfBounds("Input position is greater than 8");
			else if (position < 0)throw IndexOutOfBounds("Input position is less than 0!");
			properties.setBit(position, value);
		}

		EntityModule::EntityModule() :Container()
		{
		}

		std::string EntityModule::getName() const {
			return "MC-Entity";
		}

		void EntityModule::init()
		{
			Container::init();
		}

		void EntityModule::update() {
			Container::update();
		}

		void EntityModule::destroy()
		{
			Container::destroy();
		}

		void EntityModule::render()
		{
			Container::render();
		}

		bool Entity::operator==(Entity& other) const {
			if (other.getProperties() != getProperties()) {
				return false;
			}
			if (&other.getParent() != &getParent()) {
				return false;
			}
			return Container::operator==(other);
		}

		bool Entity::operator!=(Entity & other) const
		{
			return !(this == &other);
		}

		bool Container::operator==(Container & other) const
		{
			return getChildren() == other.getChildren();
		}

		bool Container::operator!=(Container & other) const
		{
			return !(this == &other);
		}
	}
}