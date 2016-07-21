#pragma once
#include <vector>
#include <MC-System/Constants.h>
#include <MC-System/System.h>
#include <MC-System/Utility/BitField.h>
#include <MC-Graphics/GraphicsConstants.h>


namespace mc {
	namespace gfx {
		//forward-defining classes
		class Entity;
		class EntityModule;

		/**
		A class which holds an internal buffer of {@link Entity entities,} known as "children."
		<p>
		Calling {@link #update()} will call `update()` on all of it's children.
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

								   /**
								   `EntityModule` needs to be able to see privates in order to properly interact with `Entities`
								   */
			friend class EntityModule;
			/**
			`Entity` needs to do some additional work with the private members
			*/
			friend class Entity;

			/**
			Default constructor. Creates an empty list of children.
			*/
			Container();

		public:

			/**
			Destructor. Clears all of the children.
			@see #clearChildren
			*/
			virtual ~Container();

			/**
			Should be called every time {@link System#update()} is called. Calls `update()` on all of this `Container\'s` children.
			*/
			virtual void update();
			/**
			Should be called every time {@link System#init()} is called. Calls `init()` on all of this `Container\'s` children.
			*/
			virtual void init();
			/**
			Should be called every time {@link System#terminate()} is called. Calls `destroy()` on all of this `Container\'s` children.
			*/
			virtual void destroy();

			/**
			Gets all of this `Container's` children.
			@return an `std::vector` with all children of this `Container`
			*/
			const std::vector<Entity*>& getChildren() const;

			/**
			Add an {@link Entity} to this {@link Container}
			@param e Reference to an `Entity` to become a child
			@see #update()
			@see #getChildren()
			*/
			void addChild(Entity& e);
			/**
			Removes a child by reference.
			@throws ObjectNotFoundInArray if {@link #hasChild(Entity&) const} returns `false`
			@param e Reference to a child
			@see #removeChild(Index)
			*/
			void removeChild(const Entity& e);

			/**
			Removes a child via location.
			@throws IndexOutOfBounds if the index is less than 0 or greater than {@link #size()}
			@param index Index of the `Entity` to be removed
			@see #indexOf(const Entity&) const
			@see #removeChild(const Entity&)
			*/
			void removeChild(Index index);

			/**
			Checks to see if this `Container` contains an `Entity`
			@param e Reference to an `Entity`
			@return `false` if this `Container` doesn't contain the referenced `Entity`, `true` otherwise
			@see #indexOf(const Entity& ) const
			*/
			bool hasChild(Entity& e) const;

			/**
			Removes EVERY `Entity` from this `Container`
			@see #size()
			@see #removeChild(Index)
			@see #removeChild(const Entity&)
			*/
			void clearChildren();

			/**
			Access an `Entity`.
			<p>
			This is different than `getChild()` because `operator[]` doesn't do bounds checking. Accessing an invalid location will result in a memory error.
			@param i Location of an `Entity`
			@return Reference to the `Entity` located at `i`
			@see #getChild(Index)
			@see #indexOf(const Entity&) const
			*/
			Entity& operator[](Index i);//get children via [i]

			/**
			`const` version of {@link #operator[](Index i)}
			@param i Location of an `Entity`
			@return Reference to the `Entity` located at `i`
			@see #getChild(Index) const
			@see #indexOf(const Entity&) const
			*/
			const Entity& operator[](Index i) const;//get children via [i]

			/**
			Retrieves a child at a certain index.
			@param i Index of the `Entity`
			@return Reference to the `Entity` located at `i`
			@throws IndexOutOfBounds if `i` is less than `0` or greater than {@link #size()}
			@see #operator[]
			@see #indexOf(const Entity&) const
			*/
			Entity& getChild(Index i);
			/**
			`const` version of {@link #getChild(Index)}

			@param i `Index` of the `Entity`
			@return Reference to the `Entity` located at `i`
			@throws IndexOutOfBounds if `i` is less than `0` or greater than {@link #size()}
			@see #operator[]
			@see #indexOf(const Entity&) const
			*/
			const Entity& getChild(Index i) const;

			/**
			Finds the location of an `Entity` in this `Container`
			@param e Reference to an `Entity`
			@return Location of `e,` or -1 if `e` is not a child of this `Container`
			@see #operator[]
			@see #getChild(Index)
			*/
			Index indexOf(const Entity& e) const;

			/**
			Gets an iterator over this `Container` for enchanced for loops.
			@return Iterator of the first `Entity`
			@see #end()
			@see #size()
			*/
			std::vector<Entity*>::iterator begin();
			/**
			Gets an iterator over this `Container` for enchanced for loops.
			@return Iterator of the last `Entity`
			@see #begin()
			@see #size()
			*/
			std::vector<Entity*>::iterator end();

			/**
			Calculates the amount of children this `Container` has.
			@return Size of this `Container`
			*/
			Size size() const;

			/**
			Compares if 2 `Containers` are equal.
			@param other Another `Container` to compare
			@return `true` if both `Containers` have the same children and `size()`
			@see #operator!=
			*/
			bool operator==(Container& other) const;
			/**
			Compares if 2 `Containers` are unequal.
			@param other Another `Container` to compare
			@return `false` if both `Containers` have the same children and `size()`, `true` otherwise
			@see #operator==
			*/
			bool operator!=(Container& other) const;
		private:
			/**
			`std::vector` of this `Container\'s` children. Use of this variable directly is unrecommended. Use `addChild()` or `removeChild()` instead.
			*/
			std::vector<Entity*> children = std::vector<Entity*>();
		};

		/**
		Abstract superclass for all graphical objects. Contains basic information like position, and provides a standard interface for communicating with graphical objects.
		*/
		class Entity : public Container {
			/**
			A `Container` needs to access `update()`, `init()`,  and `destroy()`
			*/
			friend class Container;
			/**
			Should be called a by `Container` when `System.update()` is called. Calls `customUpdate()`
			*/
			void update();
			/**
			Should be called a by `Container` when `System.init()` is called. Calls `customInit()`
			*/
			void init();
			/**
			Should be called a by `Container` when `System.terminate()` is called. Calls `customDestroy()`
			*/
			void destroy();

			Container* parent;

			void setParent(Container* parent);

			ByteField properties = mc::gfx::ENTITY_DEFAULT_PROPERTIES;
		public:

			/**
			Default constructor. Constructs properties based on `ENTITY_DEFAULT_PROPERTIES`
			*/
			Entity();
			/**
			Cloning constructor. Copies another's `Entity's` properties and children.
			*/
			Entity(const Entity &obj);
			/**
			Destructor. Made `virtual` for inheritance.
			@see ~Container()
			*/
			virtual ~Entity();

			/**
			Calculates position and various properties. Automatically called when `ENTITY_PROPERTY_DIRTY` is true.
			*/
			virtual void clean();

			/**
			Retrieves the `Entity's` properties as a `ByteField`
			@return The current properties belonging to this `Entity`
			@see getProperties() const
			@see setProperties(ByteField&)
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			ByteField& getProperties();
			/**
			`const` version of `getProperties()`
			@return The current properties belonging to this `Entity`
			@see setProperties(ByteField&)
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			const ByteField& getProperties() const;
			/**
			Set the properties for this `Entity`
			@param b New `Entity` properties
			@throw IndexOutOfBounds if `position<0`
			@throw IndexOutOfBounds if `position>properties.size()`
			@see getProperties()
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			void setProperties(ByteField& b);

			/**
			Retrieve the value of a property. Property consants start with `ENTITY_PROPERTY_`
			@param position Location of the property based on a constant
			@return `true` or `false` based on the postition
			@throw IndexOutOfBounds if `position<0`
			@throw IndexOutOfBounds if `position>properties.size()`
			@see setProperty(Index, bool)
			@see getProperties()
			@see setProperties(ByteField&)
			*/
			bool getProperty(Index position) const;
			/**
			Set a property to be `true` or `false`.Property consants start with `ENTITY_PROPERTY_`
			@param position Location of the property based on a constant
			@param value Whether it is `true` or `false`
			@see getProperty(Index) const
			@see getProperties()
			@see setProperties(ByteField&)
			*/
			void setProperty(Index position, bool value);

			/**
			Retrieve this `Entitys` parent `Container.`
			@return A `Container` which contains `this`
			@see Container#hasChild(const Entity&) const;
			*/
			Container& getParent();
			/**
			`const` version of `getParent()`
			@return A `Container` which contains `this`
			@see Container#hasChild(const Entity&) const;
			*/
			const Container& getParent() const;

			/**
			Compares if 2 `Entities` have the same children, parent, and properties.
			@param other An `Entity` compare this one to
			@return `true` if they are equal
			@see getProperties() const
			@see getParent() const
			@see getChildren() const
			@see operator!=
			*/
			bool operator==(Entity& other) const;
			/**
			Compares if 2 `Entities` don't have the same children, parent, and properties.
			@param other An `Entity` compare this one to
			@return `false` if they are equal
			@see getProperties() const
			@see getParent() const
			@see getChildren() const
			@see operator==
			*/
			bool operator!=(Entity& other) const;

			/**
			Automatically called when `ENTITY_PROPERTY_DEAD` is true. Removes this entity from it's parent, and calls it's `destroy()` method.
			@see getParent()
			*/
			void kill();

		protected:
			/**
			When `Container.update()` is called, `customUpdate()` is called on all of it's children.
			@see System#update()
			*/
			virtual void customUpdate() = 0;
			/**
			When `Container.init()` is called, `customInit()` is called on all of it's children.
			@see System#init()
			*/
			virtual void customInit() = 0;
			/**
			When `Container.destroy()` is called, `customDestroy()` is called on all of it's children.
			@see System#terminate()
			*/
			virtual void customDestroy() = 0;
		};

		/**
		`Module` that handles `Entities`. Required by `GraphicsModule`
		*/
		class EntityModule : public Module, public Container {
		public:
			/**
			Default constructor.
			*/
			EntityModule();
			/**
			Calls `init()` on all of this `EntityModules` children.
			*/
			virtual void init();
			/**
			Calls `update()` on all of this `EntityModules` children.
			*/
			virtual void update();
			/**
			Calls `destroy()` on all of this `EntityModules` children.
			*/
			virtual void destroy();
			/**
			Get this `EntityModules` unique name.
			@return `MC-Entity`
			*/
			virtual std::string getName() const;
		};
	}
}