#include "Entity.h"
#include <MC-System/Exceptions.h>
#include <MC-System/Definitions.h>

namespace mc {

	EntityContainer::EntityContainer()
	{
		children=  std::vector<Entity*>();
	}


	EntityContainer::~EntityContainer()
	{
		children.clear();
	}

	void EntityContainer::updateChildren()
	{
		for (std::size_t i = 0; i < children.size();i++) {
			children[i]->update();
		}
	}

	void EntityContainer::initChildren()
	{
		for (std::size_t i = 0; i < children.size(); i++) {
			children[i]->init();
		}
	}
	
	void EntityContainer::destroyChildren()
	{
		for (std::size_t i = 0; i < children.size(); i++) {
			children[i]->destroy();
		}
	}
	
	std::vector<Entity*>& EntityContainer::getChildren()
	{
		return this->children;
	}

	const std::vector<Entity*>& EntityContainer::getChildren() const
	{
		return this->children;
	}


	void EntityContainer::addChild(Entity* e)
	{
		children.push_back(e);
	}

	void EntityContainer::removeChild(const Entity* e)
	{
		for (unsigned int i = 0; i < children.size();i++) {
			if (e == children[i]) {
				children.erase(children.begin()+i);
			}
		}
		throw mc::ObjectNotFoundInArray("Specified argument is not a valid object in the array!");
	}


	Entity & EntityContainer::operator[](int i)
	{
		return getChild(i);
	}

	Entity &EntityContainer::getChild(int i)
	{
		return *children.at(i);
	}

	std::vector<Entity*>::iterator EntityContainer::begin()
	{
		return children.begin();
	}

	std::vector<Entity*>::iterator EntityContainer::end()
	{
		return children.end();
	}

	std::size_t EntityContainer::size() const
	{
		return children.size();
	}

	void EntityContainer::update()
	{
		updateChildren();
	}

	void EntityContainer::init()
	{
		initChildren();
	}

	void EntityContainer::destroy() {
		destroyChildren();
	}

	void Entity::update() {
		customUpdate();
		EntityContainer::update();
	}

	void Entity::init()
	{
		customInit();
		EntityContainer::init();
	}

	void Entity::destroy()
	{
		customDestroy();
		EntityContainer::destroy();
	}

	Entity::Entity() :EntityContainer()
	{

	}
	Entity::Entity(const Entity & obj) : EntityContainer(obj)
	{
		properties = obj.properties;
	}
	Byte Entity::getProperties()
	{
		return properties;
	}
	void Entity::setProperties(Byte b)
	{
		properties = b;
	}
	bool Entity::getProperty(unsigned int position)
	{
		return (((properties >> position) & 1)==1);
	}
	void Entity::setProperty(unsigned int position, bool value)
	{
		if (value) {
			properties |= (1 << position);
		}
		else {
			properties &= ~(1 << position);
		}

	}

	EntityModule::EntityModule()
	{
	}

	std::string EntityModule::getName() const {
		return "MC-Entity";
	}

	void EntityModule::init()
	{
		EntityContainer::init();
	}

	void EntityModule::update()
	{
		EntityContainer::update();
	}

	void EntityModule::destroy()
	{
		EntityContainer::destroy();
	}
}