#pragma once
#include <vector>
#include <memory>
#include <MC-System/Definitions.h>


namespace mc {
	class Entity;

	class Container {
	public:
		Container();
		Container(int i);
		~Container();
		Container(const Container& other);

		void updateChildren();

		std::vector<Entity*>& getChildren();
		const std::vector<Entity*>& getChildren() const;

		Container operator=(const Container& other);

		void addChild(Entity* e);
		void removeChild(const Entity* e);

		Entity& operator[](int i);//get children via [i]
		Entity& getChild(int i);

		std::vector<Entity*>::iterator begin();
		std::vector<Entity*>::iterator end();
		std::size_t size() const;
	protected:
		std::vector<Entity*> children;
	};


	class Entity : public Container{
		friend class Container;
		void update();

	public:
		Entity();
		Entity(const Entity &obj);

		Byte getProperties();
		void setProperties(Byte b);

		bool getProperty(unsigned int position);
		void setProperty(unsigned int position, bool value);

	protected:
		virtual void customUpdate() = 0;

		Byte properties = _MACE_ENTITY_DEFAULT_PROPERTIES;

	private:
	};

	using EntityPtr = std::unique_ptr<Entity>;
	
}