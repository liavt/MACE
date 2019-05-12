/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_ENTITY_H
#define MACE__GRAPHICS_ENTITY_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>
#include <MACE/Utility/Transform.h>
#include <vector>
#include <memory>

namespace mc {
	namespace gfx {
		using EntityProperties = Byte;

		//forward-defining dependencies
		class Entity;
		class Texture;
		class Painter;
		class ComponentQueue;

		struct Metrics {
			TransformMatrix transform{};
			TransformMatrix inherited{};

			bool operator==(const Metrics& other) const;
			bool operator!=(const Metrics& other) const;
		};//Metrics

		/**
		Can be plugged into an `Entity` to allow for additional functionality by listening to events. Instead of extending an existing
		`Entity` subclass, you should prefer using a `Component` to not interfere with custom Entity::onRender() and similar functions.
		<p>
		Similarly to all objects in MACE, Component::init(Entity*) is always called and Component::destroy(Entity*) is also always called.
		<p>
		Contains virtual functions for every event in an `Entity` which each have 1 parameter - the
		`Entity` being acted on.
		<p>
		You are required to implement Component::init(Entity*), Component::update(Entity*), and Component::destroy(Entity*).
		There are additional functions which are optional to implement.
		<p>
		There is no function to remove a `Component` from an `Entity`. Instead, the `Component` decides when to be removed
		from Component::update(Entity*). This makes sure that the `Component` completes whatever task it was trying to do.
		@see Entity::addComponent(Component&)
		@todo unit testing for clean() render() and hover()
		*/
		class MACE_NOVTABLE Component: public Initializable {
			friend class Entity;
			friend class ComponentQueue;
		public:
			virtual ~Component() = default;

			Entity* getParent();

			bool operator==(const Component& other) const;
			bool operator!=(const Component& other) const;
		protected:
			Entity* parent = nullptr;

			/**
			Called when this `Component` is added to the `Entity` via Entity::addComponent(Component&).
			Required function.
			@note This is not called at Entity::init(), instead it is called when the component is added to the `Entity`. Keep that in mind.
			@opengl
			*/
			virtual void init() override;
			/**
			Called when Entity::update() is called. Required function.
			<p>
			There is no function to remove a `Component` so this is the only way for a `Component` to be removed from an `Entity`
			<p>
			Component::destroy(Entity*) will be called afterwards.
			@return Whether this `Component` should be deleted or not.
			@opengl
			*/
			virtual bool update();
			/**
			Called when Entity::destroy() is called or the `Component` is removed via Component::update(Entity*),
			whichever comes first. Once Component::destroy(Entity*) is called, it is immediately removed from
			the `Entity`. Required function.
			@opengl
			*/
			virtual void destroy() override;

			virtual void render();
			/**
			Called when Entity::clean() is called and it was dirty. This is not required for inheritance.
			@opengl
			*/
			virtual void clean(Metrics& metrics);

			/**
			@opengl
			*/
			virtual void hover();
		};//Component

		/**
		Abstract superclass for all graphical objects. Contains basic information like position, and provides a standard interface for communicating with graphical objects.
		<p>
		Prefer using `Component` instead of extending this class for one-time functionality.
		@see GraphicsEntity
		@see Entity2D
		@see Component
		*/
		class MACE_NOVTABLE Entity: public Initializable {
		public:
			//values defining which bit in a byte every propety is, or how much to bit shift it
			enum EntityProperty: Byte {
				/**
				Bit location representing whether an `Entity` is dead.
				<p>
				If `true,` any {@link Entity} holding it will remove it and call `kill()`
				@see Entity::getProperty(unsigned int)
				*/
				DEAD,
				/**
				Property defining if an `Entity` can be updated and rendered. If this is `true`, `Entity::update()` and `Entity::render()` will not be called by it's parent.
				@see Entity::getProperty(unsigned int)
				*/
				DISABLED,

				/**
				Flag representing whether an Entity's init() function has been called.
				<p>
				If destroy() or update() is called and this is `false`, an `InitializationError` is thrown.
				<p>
				If init() is called and this is `true`, an `InitializationError` is thrown.
				@see Entity::getProperty(unsigned int)
				*/
				INIT,

				/**
				Flag representing whether this `Entity` is dirty and it's positions needs to be recalculated.
				<p>
				This will become true under the following conditions:
				- The `Entity` has been changed. Assume that any non-const function other than render() and update() will trigger this condition.
				- The window is resized, moved, or created
				<p>
				Other classes that inherit `Entity` can also set this to true via Entity#setProperty(Byte, bool)
				<p>
				When an `Entity` becomes dirty, it will propogate up the tree. It's parent will become dirty, it's parent will become dirty, etc. This will continue until it reaches the highest level `Entity`, which is usually the `GraphicsContext`. From there, it will decide what to do based on it's `Entity::DIRTY` flag.
				<p>
				Certain `GraphicsContexts` may only render when something is dirty, heavily increasing performance in applications with little moving objects.
				<p>
				Additionally, an `Entity` that is considered dirty will have it's buffer data updated on the GPU side.
				*/
				DIRTY,

				DEFAULT_PROPERTIES = 0x00
			};//EntityProperty

			/**
			Default constructor. Constructs properties based on `Entity::DEFAULT_PROPERTIES`
			*/
			Entity() noexcept;
			/**
			Destructor. Made `virtual` for inheritance.
			@see ~Entity()
			*/
			virtual ~Entity() noexcept;

			/**
			Gets all of this `Entity's` children.
			@return an `std::vector` with all children of this `Entity`
			*/
			const std::vector<std::shared_ptr<Entity>>& getChildren() const;
			/**
			Removes a child.
			<p>
			This function calls no member functions of the argument, meaning that
			@throws ObjectNotFoundInArray if this `Entity` does not contain the argument returns `false`
			@param e an `Entity` to remove
			@see Entity::removeChild(Index)
			@dirty
			*/
			void removeChild(const Entity& e);

			/**
			@copydoc Entity::removeChild(const Entity&)
			@throws NullPointerError if the argument is `nullptr`
			*/
			void removeChild(const Entity* e);

			/**
			@copydoc Entity::removeChild(const Entity*)
			*/
			void removeChild(std::shared_ptr<Entity> ent);

			/**
			Removes a child via location.
			@throws IndexOutOfBounds if the index is less than 0 or greater than {@link #size()}
			@param index Index of the `Entity` to be removed
			@see Entity::indexOf(const Entity&) const
			@see Entity::removeChild(const Entity&)
			@dirty
			*/
			void removeChild(Index index);

			/**
			@copydoc removeChild(Index)
			*/
			void removeChild(const std::vector<std::shared_ptr<Entity>>::iterator& iter);

			/**
			Checks to see if this `Entity` contains an `Entity`
			@param e Reference to an `Entity`
			@return `false` if this `Entity` doesn't contain the referenced `Entity`, `true` otherwise
			@see Entity::indexOf(const Entity& ) const
			*/
			bool hasChild(Entity& e) const;

			/**
			Removes EVERY `Entity` from this `Entity`
			@dirty
			@see Entity::size()
			@see Entity::removeChild(Index)
			@see Entity::removeChild(const Entity&)
			*/
			void clearChildren();

			/**
			@dirty
			*/
			void makeChildrenDirty();

			/**
			Access an `Entity`.
			<p>
			This is different than `getChild()` because `operator[]` doesn't do bounds checking. Accessing an invalid location will result in a memory error.
			@param i Location of an `Entity`
			@return Reference to the `Entity` located at `i`
			@see Entity::getChild(Index)
			@see Entity::indexOf(const Entity&) const
			*/
			Entity& operator[](Index i);//get children via [i]

			/**
			`const` version of {@link #operator[](Index i)}
			@param i Location of an `Entity`
			@return Reference to the `Entity` located at `i`
			@see Entity::getChild(Index) const
			@see Entity::indexOf(const Entity&) const
			*/
			const Entity& operator[](Index i) const;//get children via [i]

			/**

			Retrieves a child at a certain index.
			@param i Index of the `Entity`
			@return Reference to the `Entity` located at `i`
			@throws IndexOutOfBounds if `i` is less than `0` or greater than {@link #size()}
			@see Entity::operator[]
			@see Entity::indexOf(const Entity&) const
			*/
			Entity& getChild(Index i);
			/**
			`const` version of {@link #getChild(Index)}

			@param i `Index` of the `Entity`
			@return Reference to the `Entity` located at `i`
			@throws IndexOutOfBounds if `i` is less than `0` or greater than {@link #size()}
			@see Entity::operator[]
			@see Entity::indexOf(const Entity&) const
			*/
			const Entity& getChild(Index i) const;

			/**
			Finds the location of an `Entity` in this `Entity`
			@param e Reference to an `Entity`
			@return Location of `e,` or -1 if `e` is not a child of this `Entity`
			@see Entity::operator[]
			@see Entity::getChild(Index)
			*/
			Index indexOf(const Entity& e) const;

			/**
			Checks whether this `Entity` has any children
			@return If Entity::size() is 0
			*/
			bool isEmpty() const;

			/**
			Retrieves the beginning of the children of this `Entity`
			@return Pointer to the first `Entity`
			@see Entity::end()
			@see Entity::size()
			*/
			std::vector<std::shared_ptr<Entity>>::iterator begin();
			/**
			Retrieves the end of the children of this `Entity`
			@return End of the last `Entity`
			@see Entity::begin()
			@see Entity::size()
			*/
			std::vector<std::shared_ptr<Entity>>::iterator end();

			/**
			Calculates the amount of children this `Entity` has.
			@return Size of this `Entity`
			@see Entity::isEmpty()
			*/
			Size size() const;

			/**
			Retrieves the `Entity's` properties as a `ByteField`
			@return The current properties belonging to this `Entity`
			@see Entity::getProperties() const
			@see Entity::setProperties(ByteField&)
			@see Entity::getProperty(Index) const
			@see Entity::setProperty(Index, bool)
			@dirty
			*/
			EntityProperties& getProperties();
			/**
			`const` version of `getProperties()`
			@return The current properties belonging to this `Entity`
			@see Entity::setProperties(ByteField&)
			@see Entity::getProperty(Index) const
			@see Entity::setProperty(Index, bool)
			*/
			const EntityProperties& getProperties() const;
			/**
			Set the properties for this `Entity`
			@param b New `Entity` properties
			@see Entity::getProperties()
			@see Entity::getProperty(Index) const
			@see Entity::setProperty(Index, bool)
			@dirty
			*/
			void setProperties(EntityProperties& b);

			/**
			Retrieve the value of a property.
			<p>
			By default, they are all false.
			@param position Location of the property based on a constant
			@return `true` or `false` based on the postition
			@see Entity::setProperty(Index, bool)
			@see Entity::getProperties()
			@see Entity::setProperties(ByteField&)
			*/
			bool getProperty(const Byte position) const;
			/**
			Set a property to be `true` or `false`.
			<p>
			By default, they are all false.
			@note Do not use `setProperty(Entity::DIRTY, true)`. Use Entity::makeDity() instead.
			@param position Location of the property based on a constant
			@param value Whether it is `true` or `false`
			@see Entity::getProperty(Index) const
			@see Entity::getProperties()
			@see Entity::setProperties(ByteField&)
			@dirty
			*/
			void setProperty(const Byte position, const bool value);

			/**
			@dirty
			*/
			TransformMatrix& getTransformation();
			const TransformMatrix& getTransformation() const;
			/**
			@dirty
			*/
			void setTransformation(TransformMatrix& trans);

			/**
			@dirty
			*/
			Entity& translate(float x, float y, float z = 0.0f);
			/**
			@dirty
			*/
			Entity& rotate(float x, float y, float z = 0.0f);
			/**
			@dirty
			*/
			Entity& scale(float x, float y, float z = 1.0f);

			/**
			Retrieve this `Entitys` parent `Entity.`
			@return A `Entity` which contains `this`
			@see Entity#hasChild(const Entity&) const;
			*/
			Entity* const getParent();
			/**
			`const` version of `getParent()`
			@return A `Entity` which contains `this`
			@see Entity#hasChild(const Entity&) const;
			*/
			const Entity* const getParent() const;

			bool hasParent() const;

			bool needsRemoval() const;

			/**
			@dirty
			*/
			void addChild(Entity& e);
			/**
			@copydoc Entity::addChild(Entity&)
			*/
			void addChild(Entity* e);
			/**
			@copydoc Entity::addChild(Entity&)
			*/
			void addChild(std::shared_ptr<Entity> ent);

			void addComponent(Component& com);
			void addComponent(Component* com);
			/**
			@param com The SmartPointer of an `Entity`. Ownership of the pointer will change meaning this parameter cannot be marked `const`
			*/
			void addComponent(std::shared_ptr<Component> com);
			std::vector<std::shared_ptr<Component>> getComponents();

			const float& getWidth() const;
			/**
			@dirty
			@copydoc Entity::getWidth() const
			*/
			float& getWidth();
			/**
			@dirty
			*/
			void setWidth(const float& s);

			const float& getHeight() const;
			/**
			@copydoc Entity::getHeight() const
			@dirty
			*/
			float& getHeight();
			/**
			@dirty
			*/
			void setHeight(const float& s);

			const float& getDepth() const;
			/**
			@dirty
			@copydoc Entity::getHeight() const
			*/
			float& getDepth();
			/**
			@dirty
			*/
			void setDepth(const float& s);

			const float& getX() const;
			/**
			@dirty
			@copydoc Entity::getX() const
			*/
			float& getX();
			/**
			@dirty
			*/
			void setX(const float& newX);

			const float& getY() const;
			/**
			@dirty
			@copydoc Entity::getY() const
			*/
			float& getY();
			/**
			@dirty
			*/
			void setY(const float& newY);

			const float& getZ() const;
			/**
			@dirty
			@copydoc Entity::getZ() const
			*/
			float& getZ();
			/**
			@dirty
			*/
			void setZ(const float& newY);

			/**
			Compares if 2 `Entities` have the same children, parent, and properties.
			@param other An `Entity` compare this one to
			@return `true` if they are equal
			@see getProperties() const
			@see getParent() const
			@see getChildren() const
			@see operator!=
			*/
			bool operator==(const Entity& other) const noexcept;
			/**
			Compares if 2 `Entities` don't have the same children, parent, and properties.
			@param other An `Entity` compare this one to
			@return `false` if they are equal
			@see getProperties() const
			@see getParent() const
			@see getChildren() const
			@see operator==
			*/
			bool operator!=(const Entity& other) const noexcept;

			/**
			Retrieves the top most parent (known as the "root.")
			<p>
			The root does not have any parent.
			<p>
			If this `Entity` does not have any parent, returns `this`
			@return The root `Entity` of which this `Entity` belongs to.
			*/
			const Entity* getRoot() const;
			/**
			@copydoc Entity::getRoot() const
			*/
			Entity* getRoot();

			/**
			@opengl
			*/
			const Metrics& getMetrics() const;

			/**
			@internal
			@opengl
			*/
			virtual void clean();


			/**
			@internal
			@opengl
			*/
			virtual void hover();

			/**
			@dirty
			*/
			void reset();

			/**
			Makes this `Entity` dirty and root dirty.
			<p>
			Should be used over `setProperty(Entity::DIRTY,true)` as it updates the root parent.
			@see Entity::getRoot()
			@dirty
			*/
			void makeDirty();
		protected:
			/**
			`std::vector` of this `Entity\'s` children. Use of this variable directly is unrecommended. Use `addChild()` or `removeChild()` instead.
			@internal
			*/
			std::vector<std::shared_ptr<Entity>> children = std::vector<std::shared_ptr<Entity>>();

			/**
			Should be called a by `Entity` when `MACE.update()` is called. Calls `onUpdate()`.
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onUpdate()`
			@throws InitializationError If the property `Entity::INIT` is false, meaning `init()` was not called.
			*/
			virtual void update();
			/**
			Should be called a by `Entity` when `MACE.init()` is called. Calls `onInit()`
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onInit()`
			@dirty
			@opengl
			@throws InitializationError If the property `Entity::INIT` is true, meaning `init()` has already been called.
			*/
			virtual void init() override;
			/**
			Should be called a by `Entity` when `MACE.destroy()` is called. Calls `onDestroy()`. Sets `Entity::INIT` to be false
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onDestroy()`
			@dirty
			@opengl
			@throws InitializationError If the property `Entity::INIT` is false, meaning `init()` was not called.
			*/
			virtual void destroy() override;

			/**
			Should be called by a `Entity` when the graphical `Window` clears the frame.
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onRender()`
			@opengl
			@see Entity#update()
			*/
			virtual void render();

			/**
			When `Entity.update()` is called, `onUpdate()` is called on all of it's children.
			@see MACE#update()
			@internal
			*/
			virtual void onUpdate();
			/**
			When `Entity.init()` is called, `onInit()` is called on all of it's children.
			@see MACE#init()
			@internal
			@opengl
			*/
			virtual void onInit();
			/**
			When `Entity.destroy()` is called, `onDestroy()` is called on all of it's children.
			@see MACE#destroy()
			@internal
			@opengl
			*/
			virtual void onDestroy();

			/**
			When `Entity.render()` is called, `onRender()` is called on all of it's children.
			@internal
			@opengl
			*/
			virtual void onRender();

			/**
			@internal
			@opengl
			*/
			virtual void onClean();

			/**
			@internal
			@opengl
			*/
			virtual void onHover();
		private:
			std::vector<std::shared_ptr<Component>> components = std::vector<std::shared_ptr<Component>>();

			EntityProperties properties = Entity::DEFAULT_PROPERTIES;

			Entity* parent = nullptr;

			/**
			@internal
			*/
			TransformMatrix transformation;

			/**
			@internal
			*/
			Metrics metrics;

			/**
			Automatically called when `Entity::PROPERTY_DEAD` is true. Removes this entity from it's parent, and calls it's `destroy()` method.
			@dirty
			@see getParent()
			*/
			void kill();

			void setParent(Entity* parent);
		};//Entity

		class Group: public Entity {
		protected:
			void onInit() override;
			void onUpdate() override;
			void onRender() override;
			void onDestroy() override;

		};//Group
	}//gfx
}//mc

#endif
