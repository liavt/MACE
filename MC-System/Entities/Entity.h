#pragma once
#include <vector>
#include <MC-System/Constants.h>
#include <MC-System/System.h>


namespace mc {
	class Entity;
	class EntityModule;

	class Container {
		void updateChildren();
		void initChildren();
		void destroyChildren();//think of the children!

		friend class EntityModule;
		friend class Entity;
		
		Container();

	public:
		~Container();

		virtual void update();
		virtual void init();
		virtual void destroy();

		const std::vector<Entity*>& getChildren() const;

		void addChild(Entity& e);
		void removeChild(const Entity& e);
		void removeChild(unsigned int index);

		bool hasChild(Entity& e);

		Entity& operator[](unsigned int i);//get children via [i]
		const Entity& operator[](unsigned int i) const;//get children via [i]

		Entity& getChild(unsigned int i);
		const Entity& getChild(unsigned int i) const;

		unsigned int indexOf(Entity& e);

		std::vector<Entity*>::iterator begin();
		std::vector<Entity*>::iterator end();
		Size size() const;

		bool operator==(Container& other);
		bool operator!=(Container& other);
	protected:
		std::vector<Entity*> children = std::vector<Entity*>();
	};

	class Entity : public Container{
		friend class Container;
		void update();
		void init();
		void destroy();

		Container* parent;

		void setParent(Container* parent);
	public:
		static void cleanEntity(Entity* e);

		Entity();
		Entity(const Entity &obj);

		~Entity();

		Byte getProperties();
		void setProperties(Byte b);

		bool getProperty(unsigned int position);
		void setProperty(unsigned int position, bool value);

		Container& getParent();

		bool operator==(Entity& other);
		bool operator!=(Entity& other);

		/**
		Automatically called when {@code ENTITY_PROPERTY_DEAD} is true. Removes this entity from it's parent, and calls it's {@code destroy()} method.
		*/
		void kill();

	protected:
		virtual void customUpdate() = 0;
		virtual void customInit() = 0;
		virtual void customDestroy() = 0;

		Byte properties = ENTITY_DEFAULT_PROPERTIES;

	};

	class EntityModule : public Module,public Container {
	public:
		EntityModule();


		void init();
		void update();
		void destroy();
		std::string getName() const;
	};
	
}