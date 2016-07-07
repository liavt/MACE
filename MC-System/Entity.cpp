#include "Entity.h"
#include <MC-System/Exceptions.h>
#include <MC-System/Definitions.h>

namespace mc {

	Container::Container()
	{
		children=  std::vector<Entity*>(_MACE_ENTITY_DEFAULT_CONTAINER_SIZE);
	}

	Container::Container(int size)
	{
		children= std::vector<Entity*>(size);
	}

	Container::~Container()
	{
		for (auto &it : children) delete it; children.clear();
	}

	Container::Container(const Container & other)
	{
		children = other.children;
	}

	void Container::updateChildren()
	{
		for (std::size_t i = 0; i < children.size();i++) {
			children[i]->update();
		}
	}
	
	
	std::vector<Entity*>& Container::getChildren()
	{
		return this->children;
	}

	const std::vector<Entity*>& Container::getChildren() const
	{
		return this->children;
	}

	Container Container::operator=(const Container & other)
	{
		return Container(other);
	}

	void Container::addChild(Entity* e)
	{
		children.push_back(e);
	}

	void Container::removeChild(const Entity* e)
	{
		for (unsigned int i = 0; i < children.size();i++) {
			if (e == children[i]) {
				children.erase(children.begin()+i);
			}
		}
		throw mc::ObjectNotFoundInArray("Specified argument is not a valid object in the array!");
	}


	Entity & Container::operator[](int i)
	{
		return getChild(i);
	}

	Entity &Container::getChild(int i)
	{
		return *children.at(i);
	}

	std::vector<Entity*>::iterator Container::begin()
	{
		return children.begin();
	}

	std::vector<Entity*>::iterator Container::end()
	{
		return children.end();
	}

	std::size_t Container::size() const
	{
		return children.size();
	}

	void Entity::update()
	{
		customUpdate();
		updateChildren();
	}
	Entity::Entity() :Container()
	{

	}
	Entity::Entity(const Entity & obj) : Container(obj)
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
		//basically a ternary statement. bit shifts to a certain position, and if the first bit is 1, returns true, other false
		//the !! is to tell the comiler to explicitely return true or false. otherwise, it would return !true and true, whcih obviously is unintended.
		Byte shifted = (position >> properties) & 1;;
		return (shifted==1);
	}
	void Entity::setProperty(unsigned int position, bool value)
	{
		properties ^= ((value ? 1 :0) ^ properties) & (1 << position);
	}
}