#include "Entity.h"

namespace mc {

	Container::Container()
	{
		children=  std::vector<std::unique_ptr<Entity>>(_MACE_DEFAULT_CONTAINER_SIZE);
	}

	Container::Container(int size)
	{
		children= std::vector<std::unique_ptr<Entity>>(size);
	}

	void Container::updateChildren()
	{
		for (std::size_t i = 0; i < children.size();i++) {
			children.at(i).get()->update();
		}
	}
	
	
	std::vector<std::unique_ptr<Entity>>& Container::getChildren()
	{
		return this->children;
	}

	const std::vector<std::unique_ptr<Entity>>& Container::getChildren() const
	{
		return this->children;
	}


	Entity & Container::operator[](int i)
	{
		return getChild(i);
	}

	Entity & Container::getChild(int i)
	{
		return *children.at(i).get();
	}

}