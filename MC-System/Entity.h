#pragma once
#include <vector>
#include <MC-System/Definitions.h>
#include <MC-System/System.h>


namespace mc {
	class Entity;
	class EntityModule;

	class EntityContainer {
		void updateChildren();
		void initChildren();
		void destroyChildren();//think of the children!

		friend class EntityModule;
		friend class Entity;
		
		EntityContainer();

	public:
		~EntityContainer();

		virtual void update();
		virtual void init();
		virtual void destroy();

		std::vector<Entity*>& getChildren();
		const std::vector<Entity*>& getChildren() const;

		void addChild(Entity& e);
		void removeChild(const Entity& e);

		Entity& operator[](int i);//get children via [i]
		Entity& getChild(int i);

		std::vector<Entity*>::iterator begin();
		std::vector<Entity*>::iterator end();
		std::size_t size() const;
	protected:
		std::vector<Entity*> children = std::vector<Entity*>();
	};

	using Container = EntityContainer;

	class Entity : public Container{
		friend class EntityContainer;
		void update();
		void init();
		void destroy();

	public:
		Entity();
		Entity(const Entity &obj);

		Byte getProperties();
		void setProperties(Byte b);

		bool getProperty(unsigned int position);
		void setProperty(unsigned int position, bool value);

	protected:
		virtual void customUpdate() = 0;
		virtual void customInit() = 0;
		virtual void customDestroy() = 0;

		Byte properties = ENTITY_DEFAULT_PROPERTIES;

	};

	class EntityModule : public Module,public EntityContainer {
	public:
		EntityModule();


		void init();
		void update();
		void destroy();
		std::string getName() const;
	};
	
}