/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity.h>
#include <MACE/System/Exceptions.h>
#include <MACE/System/Constants.h>

namespace mc {
namespace gfx {

void Entity::updateChildren()
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

void Entity::initChildren()
{
	for (Index i = 0; i < children.size(); ++i) {
		if (!children[i]->getProperty(ENTITY_INIT))children[i]->init();

	}
}

bool Entity::hasChild(Entity & e) const
{
	for (Size i = 0; i < children.size(); ++i) {
		if (children[i] == &e) {
			return true;
		}
	}
	return false;
}

void Entity::clearChildren()
{
	while (!children.empty()) {
		children.back()->setParent(nullptr);
		children.pop_back();
	}
}

void Entity::destroyChildren()
{
	for (Index i = 0; i < children.size(); ++i) {
		children[i]->destroy();
	}
}

void Entity::renderChildren()
{
	for (Index i = 0; i < children.size();++i) {
		children[i]->render();
	}
}

const std::vector<Entity*>& Entity::getChildren() const
{
	return this->children;
}


void Entity::removeChild(const Entity& e)
{
	for (Size i = 0; i < children.size(); i++) {
		if (&e == children.at(i)) {
			removeChild(i);
			return;
		}
	}
	throw mc::ObjectNotFoundInArrayException("Specified argument is not a valid object in the array!");
}

void Entity::removeChild(Index index) {
	if (index > children.size()) {
		throw mc::IndexOutOfBoundsException(index + " is larger than the amount of children!");
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
	if(!getProperty(ENTITY_RENDER_DISABLED)){
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

Entity* const Entity::getParent() {
	return parent;
}

const Entity* const Entity::getParent() const {
	return parent;
}

bool Entity::hasParent() const
{
	return parent != 0||parent!=nullptr;
}

void Entity::addChild(Entity & e)
{
	children.push_back(&e);
	e.setParent(this);
	//if the entity is getting added after init() has been called, we call init() ourselves
	if (!e.getProperty(ENTITY_INIT) && getProperty(ENTITY_INIT))e.init();
}

Entity& Entity::operator[](Index i) {
	return *children[i];
}

const Entity& Entity::operator[](Index i) const
{
	return *children[i];
}

Entity& Entity::getChild(Index i) {
	return *children.at(i);
}

const Entity& Entity::getChild(Index i) const
{
	return *children.at(i);
}

int Entity::indexOf(const Entity & e) const
{
	for (Index i = 0; i < children.size(); i++) {
		if (children[i] == &e) {
			return i;
		}
	}
	return -1;
}

std::vector<Entity*>::iterator Entity::begin()
{
	return children.begin();
}

std::vector<Entity*>::iterator Entity::end()
{
	return children.end();
}

Size Entity::size() const
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
	if (!getProperty(ENTITY_UPDATE_DISABLED)) {

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


Entity::Entity()
{

}
Entity::Entity(const Entity & obj)
{
	children = obj.children;
	properties = obj.properties;
}

Entity::~Entity()
{
	children.clear();

}


EntityProperties& Entity::getProperties() {
	return properties;
}

const EntityProperties& Entity::getProperties() const
{
	return properties;
}

void Entity::setProperties(EntityProperties& b)
{
	properties = b;
}

bool Entity::getProperty(const Index position) const {
#ifdef MACE_ERROR_CHECK
	if (position > properties.size())throw IndexOutOfBoundsException("Input position is greater than 8");
	else if (position < 0)throw IndexOutOfBoundsException("Input position is less than 0!");
#endif
	return properties.getBit(position);
}

void Entity::setProperty(const Index position, const bool value){
#ifdef MACE_ERROR_CHECK
	if (position > properties.size())throw IndexOutOfBoundsException("Input position is greater than 8");
	else if (position < 0)throw IndexOutOfBoundsException("Input position is less than 0!");
#endif
	properties.setBit(position, value);
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
	if (*other.getParent() != const_cast<Entity&>(*getParent())) {
		return false;
	}
	if (other.getBaseTransformation() != getBaseTransformation()) {
		return false;
	}
	if (other.actions!=actions) {
		return false;
	}
	return children == other.children;
}

bool Entity::operator!=(Entity & other) const
{
	return !(this == &other);
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

void CallbackEntity::customInit()
{
	initCallback();
}

void CallbackEntity::customUpdate()
{
	updateCallback();
}

void CallbackEntity::customRender()
{
	renderCallback();
}

void CallbackEntity::customDestroy()
{
	destroyCallback();
}

void CallbackEntity::setInitCallback(const VoidFunctionPtr func)
{
	if (func == 0 || func == nullptr)throw NullPointerException("Init callback must be a valid pointer, and not null!");
	initCallback = func;
}

VoidFunctionPtr CallbackEntity::getInitCallback()
{
	return initCallback;
}

const VoidFunctionPtr CallbackEntity::getInitCallback() const
{
	return initCallback;
}

void CallbackEntity::setUpdateCallback(const VoidFunctionPtr func)
{
	if (func == 0 || func == nullptr)throw NullPointerException("Update callback must be a valid pointer, and not null!");
	updateCallback = func;
}

VoidFunctionPtr CallbackEntity::getUpdateCallback()
{
	return updateCallback;
}

const VoidFunctionPtr CallbackEntity::getUpdateCallback() const
{
	return updateCallback;
}

void CallbackEntity::setRenderCallback(const VoidFunctionPtr func)
{
	if (func == 0 || func == nullptr)throw NullPointerException("Render callback must be a valid pointer, and not null!");
	renderCallback = func;
}

VoidFunctionPtr CallbackEntity::getRenderCallback()
{
	return renderCallback;
}

const VoidFunctionPtr CallbackEntity::getRenderCallback() const
{
	return renderCallback;
}

void CallbackEntity::setDestroyCallback(const VoidFunctionPtr func)
{
	if (func == 0 || func == nullptr)throw NullPointerException("Destroy callback must be a valid pointer, and not null!");
	destroyCallback = func;
}

VoidFunctionPtr CallbackEntity::getDestroyCallback()
{
	return destroyCallback;
}

const VoidFunctionPtr CallbackEntity::getDestroyCallback() const
{
	return destroyCallback;
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

}//gfx
}//mc