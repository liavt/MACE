#pragma once
#include <vector>
#include <MC-System/Constants.h>
#include <MC-System/System.h>
#include <MC-System/Utility/BitField.h>


namespace mc {
	class Entity;
	class EntityModule;

	/**
	A class which holds an internal buffer of {@link Entity entities,} known as "children."
	<p>
	Calling {@link #update()} will call {@link Entity#update()} on all of it's children.
	*/
	class Container {
		/**
		Calls {@link #update()} on all of it's children.
		*/
		void updateChildren();
		/**
		Calls {@link #init()} on all of it's children.
		*/
		void initChildren();
		/**
		Calls {@link #destroy()} on all of it's children.
		*/
		void destroyChildren();//think of the children!

		friend class EntityModule;
		friend class Entity;
		
		/**
		Default constructor. Has no children
		*/
		Container();

	public:
		
		/**
		Destructor. Clears all of the children.
		@see #clearChildren
		*/
		virtual ~Container();

		/**
		Should be called every time {@link System#update()} is called. Calls {@code update()} on all of this {@code Container's} children.
		*/
		virtual void update();
		/**
		Should be called every time {@link System#init()} is called. Calls {@code init()} on all of this {@code Container's} children.
		*/
		virtual void init();
		/**
		Should be called every time {@link System#terminate()} is called. Calls {@code destroy()} on all of this {@code Container's} children.
		*/
		virtual void destroy();

		/**
		Gets all of this {@code Container's} children.
		@returns an {@code std::vector} with all children of this {@code Container}
		*/
		const std::vector<Entity*>& getChildren() const;

		/**
		Add an {@link Entity} to this {@link Container}
		@param e Reference to an {@code Entity} to become a child
		@see #update()
		@see #getChildren()
		*/
		void addChild(Entity& e);
		/**
		Removes a child by reference.

		@throws {@code ObjectNotFoundInArray} if {@link #hasChild(Entity&)} returns {@code false}
 		@param e Reference to a child
		@see #removeChild(unsigned int)
		*/
		void removeChild(const Entity& e);

		/**
		Removes a child via location.

		@throws {@code IndexOutOfBounds} if the index is less than 0 or greater than {@link #size()}
		@param Index of the {@code Entity} to be removed
		@see #indexOf(Entity&)
		@see #removeChild(const Entity&)
		*/
		void removeChild(unsigned int index);

		/**
		Checks to see if this {@code Container} contains an {@code Entity}
		@param e Reference to an {@code Entity}
		@return {@code false} if this {@code Container} doesn't contain the referenced {@code Entity, true} 
		@see #indexOf(Entity& )
		*/
		bool hasChild(Entity& e) const;

		/**
		Removes EVERY {code Entity} from this {@code Container.}
		@see #size()
		@see #removeChild(unsigned int)
		@see #removeChild(const Entity&)
		*/
		void clearChildren();

		/**
		Access an {@code Entity}.
		@see #getChild(unsigned int)
		*/
		Entity& operator[](unsigned int i);//get children via [i]
		const Entity& operator[](unsigned int i) const;//get children via [i]

		Entity& getChild(unsigned int i);
		const Entity& getChild(unsigned int i) const;

		unsigned int indexOf(Entity& e) const;

		std::vector<Entity*>::iterator begin();
		std::vector<Entity*>::iterator end();
		Size size() const;

		bool operator==(Container& other) const;
		bool operator!=(Container& other) const;
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

		virtual ~Entity();

		ByteField& getProperties();
		const ByteField& getProperties() const;
		void setProperties(ByteField& b);

		bool getProperty(unsigned int position) const;
		void setProperty(unsigned int position, bool value);

		const Container& getParent() const;
		Container& getParent();

		bool operator==(Entity& other) const;
		bool operator!=(Entity& other) const;

		/**
		Automatically called when {@code ENTITY_PROPERTY_DEAD} is true. Removes this entity from it's parent, and calls it's {@code destroy()} method.
		*/
		void kill();

	protected:
		virtual void customUpdate() = 0;
		virtual void customInit() = 0;
		virtual void customDestroy() = 0;

		ByteField properties = ENTITY_DEFAULT_PROPERTIES;

	};

	class EntityModule : public Module, public Container {
	public:
		EntityModule();

		virtual void init();
		virtual void update();
		virtual void destroy();
		virtual std::string getName() const;
	};
}