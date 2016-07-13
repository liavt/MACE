#include "Entity.h"
#include <MC-System/Exceptions.h>
#include <MC-System/Constants.h>

namespace mc {
	namespace gfx{
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
			for (Size i = 0; i < children.size(); i++) {
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
			for (Size i = 0; i < children.size(); i++) {
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
			for (Size i = 0; i < children.size(); i++) {
				children[i]->destroy();
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

		void Container::removeChild(unsigned int index) {
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

		Entity& Container::operator[](unsigned int i) {
			return *children[i];
		}

		const Entity& Container::operator[](unsigned int i) const
		{
			return *children[i];
		}

		Entity& Container::getChild(unsigned int i) {
			return *children.at(i);
		}

		const Entity& Container::getChild(unsigned int i) const
		{
			return *children.at(i);
		}

		unsigned int Container::indexOf(Entity & e) const
		{
			for (unsigned int i = 0; i < children.size(); i++) {
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

		void Entity::kill() {
			destroy();
		}

		void Entity::update() {
			if (getProperty(ENTITY_PROPERTY_UPDATE_ENABLED)) {
				customUpdate();
				Container::update();
			}
			if (getProperty(ENTITY_PROPERTY_DIRTY)) {
				cleanEntity(this);
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

		void Entity::cleanEntity(Entity * e)
		{
			e->setProperty(ENTITY_PROPERTY_DIRTY, false);
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
		bool Entity::getProperty(unsigned int position) const
		{
			return properties.getBit(position);
		}
		void Entity::setProperty(unsigned int position, bool value)
		{
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