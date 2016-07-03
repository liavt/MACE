#pragma once
#include <vector>
#include <memory>
#ifndef _MACE_DEFAULT_CONTAINER_SIZE
	#define _MACE_DEFAULT_CONTAINER_SIZE 2
#endif

namespace mc {
	class Entity;

	class Container {
	public:
		Container();
		Container(int i);

		void updateChildren();

		std::vector<std::unique_ptr<Entity>>& getChildren();
		const std::vector<std::unique_ptr<Entity>>& getChildren() const;

		void addChild(std::unique_ptr<Entity>& e);
		void removeChild(const std::unique_ptr<Entity>& e);

		Entity& operator[](int i);//get children via [i]
		Entity& getChild(int i);

		std::unique_ptr<Entity> * begin();
		std::unique_ptr<Entity> * end();
		std::size_t size() const;
	protected:
		std::vector<std::unique_ptr<Entity>> children;
	};


	class Entity : public Container{
		friend class Container;
		void update();

	public:
		virtual void customUpdate() = 0;

		Container& getParent() const;
		void setParent(Entity * e);
		bool hasParent() const;

		bool isDead() const;
		void setDeath(bool b);




	protected:
		bool dead = false;

		Container& parent;
	};

	
}