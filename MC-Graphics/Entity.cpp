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
}

void Container::updateChildren()
{
	for (Index i = 0; i < children.size(); i++) {
		if (children[i]->getProperty(ENTITY_DEAD)) {
			children[i]->kill();
			removeChild(i);
			i--;//update the index after the removal of an element, dont want an error
			return;
		}
		children[i]->update();
	}
}

void Container::initChildren()
{
	for (Index i = 0; i < children.size(); ++i) {
		if (!children[i]->getProperty(ENTITY_INIT))children[i]->init();

	}
}

bool Container::hasChild(Entity & e) const
{
	for (Size i = 0; i < children.size(); ++i) {
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
	for (Index i = 0; i < children.size(); ++i) {
		children[i]->destroy();
	}
}

void Container::renderChildren()
{
	for (Index i = 0; i < children.size();++i) {
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
	//check if we can render
	if(getProperty(ENTITY_RENDER_ENABLED)){
		//we want to render as fast as possible, so we avoid doing anything but rendering here. actions and inheritence is done during update()
		customRender();
		renderChildren();
	}
}

void Entity::setParent(Entity * parent)
{
	this->parent = parent;
	setProperty(ENTITY_INIT, false);
}

Entity& Entity::getParent() {
	return *parent;
}

const Entity& Entity::getParent() const {
	return *parent;
}

bool Entity::hasParent() const
{
	return parent != 0||parent!=nullptr;
}

void Entity::addChild(Entity & e)
{
	Container::addChild(e);
	e.setParent(this);
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

int Container::indexOf(const Entity & e) const
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

void Entity::kill() {
	destroy();
	//need to verify if getParent().removeEntity(this) needs to be called here
}

void Entity::addAction(Action & action)
{
	actions.push_back(&action);
	action.init(*this);
}

std::vector<Action*> Entity::getActions()
{
	return actions;
}

void Entity::update() {
	//check if we can update
	if (getProperty(ENTITY_UPDATE_ENABLED)) {

		//inherit properties from the parent
		//first,check if we can inherit, and then check if we have a parent()
		if (!getProperty(ENTITY_IGNORE_PARENT)&&hasParent()) {
			const Entity& parent = getParent();//we will be doing multiple calls on the parent, so we assign the refernce to a variable. this is faster than calling getParent() multiple times.
			//check if the parent can pass it's properties down from the parent
			if (parent.getProperty(ENTITY_PASS_DOWN)) {//check if the parent is allowed to pass it's properties down
				inherit(parent);//actually inherit properties
			}
		}

		//update the actions of this entity
		for (Index i = 0; i < actions.size();i++) {
			Action* a = actions.at(i);
			if (a->update(*this)) {
				a->destroy(*this);
				actions.erase(actions.begin() + i);
				i--;//update the index after a removal, so we dont get an exception for accessing deleted memory
			}
		}

		customUpdate();
		//call update() on children
		updateChildren();
	}
}

void Entity::init()
{
	customInit();
	initChildren();
	setProperty(ENTITY_INIT, true);
}

void Entity::destroy()
{
	//destroy each action
	for (Index i = 0; i < actions.size();i++) {
		actions[i]->destroy(*this);
	}
	actions.clear();
	destroyChildren();
	customDestroy();
}

void Entity::inherit(const Entity & e)
{
	//check stuff
	if (getProperty(ENTITY_INHERIT_STRETCH_X)) {
		setProperty(ENTITY_STRETCH_X, e.getProperty(ENTITY_STRETCH_X));
	}

	if (getProperty(ENTITY_INHERIT_STRETCH_Y)) {
		setProperty(ENTITY_STRETCH_Y, e.getProperty(ENTITY_STRETCH_Y));
	}
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


BitField<uint16_t>& Entity::getProperties() {
	return properties;
}

const BitField<uint16_t>& Entity::getProperties() const
{
	return properties;
}

void Entity::setProperties(BitField<uint16_t>& b)
{
	properties = b;
}
TransformMatrix & Entity::getBaseTransformation()
{
	return baseTransformation;
}

const TransformMatrix & Entity::getBaseTransformation() const
{
	return baseTransformation;
}

void Entity::setBaseTransformation(TransformMatrix & trans)
{
	baseTransformation = trans;
}

Entity & Entity::translate(float x, float y, float z)
{
	baseTransformation.translate(x,y,z);
	return *this;
}

Entity & Entity::rotate(float x, float y, float z)
{
	baseTransformation.rotate(x, y, z);
	return *this;
}

Entity & Entity::scale(float x, float y, float z)
{
	baseTransformation.scale(x, y, z);
	return *this;
}

bool Entity::operator==(Entity& other) const {
	if (other.getProperties() != getProperties()) {
		return false;
	}
	if (&other.getParent() != &getParent()) {
		return false;
	}
	if (other.getBaseTransformation() != getBaseTransformation()) {
		return false;
	}
	if (other.actions!=actions) {
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
GraphicsEntity::GraphicsEntity() : Entity()
{
}
GraphicsEntity::GraphicsEntity(Texture & t) : Entity()
{
	texture = t;
}
GraphicsEntity::~GraphicsEntity()
{
}
void GraphicsEntity::setTexture(Texture & tex)
{
	texture = tex;
}
Texture & GraphicsEntity::getTexture()
{
	return texture;
}
const mc::gfx::Texture & GraphicsEntity::getTexture() const
{
	return texture;
}

void Group::customInit()
{
}

void Group::customUpdate()
{
}

void Group::customRender()
{
}

void Group::customDestroy()
{
}

float & Entity::getWidth()
{
	return baseTransformation.scaler[0];
}
const float & Entity::getWidth() const
{
	return baseTransformation.scaler[0];
}
void Entity::setWidth(const float & s)
{
	baseTransformation.scaler[0] = s;
}
float & Entity::getHeight()
{
	return baseTransformation.scaler[1];
}
const float & Entity::getHeight() const
{
	return baseTransformation.scaler[1];
}
void Entity::setHeight(const float & s)
{
	baseTransformation.scaler[1] = s;
}
float & Entity::getX()
{
	return baseTransformation.translation[0];
}
const float & Entity::getX() const
{
	return baseTransformation.translation[0];
}
void Entity::setX(const float & newX)
{
	baseTransformation.translation[0] = newX;
}
float & Entity::getY()
{
	return baseTransformation.translation[1];
}
const float & Entity::getY() const
{
	return baseTransformation.translation[1];
}
void Entity::setY(const float & newY)
{
	baseTransformation.translation[1] = newY;
}

}//gfx
}//mc