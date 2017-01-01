/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_ENTITY_H
#define MACE_GRAPHICS_ENTITY_H

#include <MACE/Utility/BitField.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Graphics/OGL.h>
#include <vector>

namespace mc {
	namespace gfx {
		using EntityProperties = BitField;

		//forward-defining dependencies
		class Entity;
		class GraphicsEntity;

		namespace ssl {
			void fillBuffer(GraphicsEntity*);
			void bindBuffer(ogl::UniformBuffer&);
			void bindEntity(const GraphicsEntity*, ogl::ShaderProgram&);
			void checkInput();
		};

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
		*/
		class Component {
		public:
			/**
			Called when this `Component` is added to the `Entity` via Entity::addComponent(Component&).
			Required function.
			@param e The parent `Entity`
			@note This is not called at Entity::init(), instead it is called when the component is added to the `Entity`. Keep that in mind.
			@opengl
			*/
			virtual void init(Entity* e) = 0;
			/**
			Called when Entity::update() is called. Required function.
			<p>
			There is no function to remove a `Component` so this is the only way for a `Component` to be removed from an `Entity`
			<p>
			Component::destroy(Entity*) will be called afterwards.
			@param e The parent `Entity`
			@return Whether this `Component` should be deleted or not.
			@opengl
			*/
			virtual bool update(Entity* e) = 0;
			/**
			Called when Entity::destroy() is called or the `Component` is removed via Component::update(Entity*),
			whichever comes first. Once Component::destroy(Entity*) is called, it is immediately removed from
			the `Entity`. Required function.
			@param e The parent `Entity`
			@opengl
			*/
			virtual void destroy(Entity* e) = 0;
			/**
			Called when Entity::clean() is called and it was dirty. This is not required for inheritance.
			@param e The parent `Entity`
			@opengl
			*/
			virtual void clean(Entity* e);

			/**
			@opengl
			*/
			virtual void hover(Entity* e);
		};//Component

		/**
		Abstract superclass for all graphical objects. Contains basic information like position, and provides a standard interface for communicating with graphical objects.
		<p>
		Prefer using `Component` instead of extending this class for one-time functionality.
		@see GraphicsEntity
		@see Entity2D
		@see Component
		@todo Events for hover for both Entity and Component
		*/
		class Entity {
		public:
			//values defining which bit in a byte every propety is, or how much to bit shift it
			enum EntityProperty: Byte {
				/**
				Bit location representing whether an `Entity` is dead.
				<p>
				If `true,` any {@link Entity} holding it will remove it and call `kill()`
				@see Entity#getProperty(unsigned int)
				*/
				DEAD = 0,
				/**
				Property defining if an `Entity` can be updated. If this is `FALSE`, `update()` will be called by it's parent.
				@see Entity#getProperty(unsigned int)
				*/
				UPDATE_DISABLED = 1,
				/**
				Property defining if an `Entity` can be rendered. If this is `false`, `render()` will be called by it's parent.
				@see Entity#getProperty(unsigned int)
				*/
				RENDER_DISABLED = 2,

				/**
				Flag representing whether an Entity's init() function has been called.
				<p>
				If destroy() or update() is called and this is `false`, an `InitializationError` is thrown.
				<p>
				If init() is called and this is `true`, an `InitializationError` is thrown.
				@see Entity#getProperty(unsigned int)
				*/
				INIT = 3,

				/**
				Flag representing whether an Entity's X position should move when it's parent is resized.
				@see Entity::STRETCH_Y
				*/
				STRETCH_X = 4,

				/**
				Flag representing whether an Entity's Y position should move when it's parent is resized.
				@see Entity::STRETCH_X
				*/
				STRETCH_Y = 5,
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
				Additionally, an `Entity` that is considered dirty will have it's buffer updated on the GPU side.
				*/
				DIRTY = 6
			};

			/**
			Default constructor. Constructs properties based on `Entity::DEFAULT_PROPERTIES`
			*/
			Entity() noexcept;
			/**
			Cloning constructor. Copies another's `Entity's` properties and children.
			*/
			Entity(const Entity &obj) noexcept;
			/**
			Destructor. Made `virtual` for inheritance.
			@see ~Entity()
			*/
			virtual ~Entity() noexcept;
			
			/**
			Gets all of this `Entity's` children.
			@return an `std::vector` with all children of this `Entity`
			*/
			const std::vector<Entity*>& getChildren() const;
			/**
			Removes a child by reference.
			@dirty
			@throws ObjectNotFoundInArray if {@link #hasChild(Entity&) const} returns `false`
			@param e Reference to a child
			@see #removeChild(Index)
			*/
			void removeChild(const Entity& e);

			/**
			Removes a child via location.
			@dirty
			@throws IndexOutOfBounds if the index is less than 0 or greater than {@link #size()}
			@param index Index of the `Entity` to be removed
			@see #indexOf(const Entity&) const
			@see #removeChild(const Entity&)
			*/
			void removeChild(Index index);

			/**
			Checks to see if this `Entity` contains an `Entity`
			@param e Reference to an `Entity`
			@return `false` if this `Entity` doesn't contain the referenced `Entity`, `true` otherwise
			@see #indexOf(const Entity& ) const
			*/
			bool hasChild(Entity& e) const;

			/**
			Removes EVERY `Entity` from this `Entity`
			@dirty
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
			Finds the location of an `Entity` in this `Entity`
			@param e Reference to an `Entity`
			@return Location of `e,` or -1 if `e` is not a child of this `Entity`
			@see #operator[]
			@see #getChild(Index)
			*/
			int indexOf(const Entity& e) const;

			/**
			Gets an iterator over this `Entity` for enchanced for loops.
			@return Iterator of the first `Entity`
			@see #end()
			@see #size()
			*/
			std::vector<Entity*>::iterator begin();
			/**
			Gets an iterator over this `Entity` for enchanced for loops.
			@return Iterator of the last `Entity`
			@see #begin()
			@see #size()
			*/
			std::vector<Entity*>::iterator end();

			/**
			Calculates the amount of children this `Entity` has.
			@return Size of this `Entity`
			*/
			Size size() const;

			/**
			Retrieves the `Entity's` properties as a `ByteField`
			@dirty
			@return The current properties belonging to this `Entity`
			@see getProperties() const
			@see setProperties(ByteField&)
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			EntityProperties& getProperties();
			/**
			`const` version of `getProperties()`
			@return The current properties belonging to this `Entity`
			@see setProperties(ByteField&)
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			const EntityProperties& getProperties() const;
			/**
			Set the properties for this `Entity`
			@dirty
			@param b New `Entity` properties
			@see getProperties()
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			void setProperties(EntityProperties& b);

			/**
			Retrieve the value of a property. Property consants start with `Entity::`
			@param position Location of the property based on a constant
			@return `true` or `false` based on the postition
			@see setProperty(Index, bool)
			@see getProperties()
			@see setProperties(ByteField&)
			*/
			bool getProperty(const Byte position) const;
			/**
			Set a property to be `true` or `false`.Property consants start with `Entity::`
			@dirty
			@param position Location of the property based on a constant
			@param value Whether it is `true` or `false`
			@see getProperty(Index) const
			@see getProperties()
			@see setProperties(ByteField&)
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
			Entity& translate(float x, float y, float z);
			/**
			@dirty
			*/
			Entity& rotate(float x, float y, float z);
			/**
			@dirty
			*/
			Entity& scale(float x, float y, float z);


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

			/**
			@dirty
			*/
			void addChild(Entity& e);

			void addComponent(Component& action);
			std::vector<Component*> getComponents();

			/**
			@dirty
			*/
			float& getWidth();
			const float& getWidth() const;
			/**
			@dirty
			*/
			void setWidth(const float& s);

			/**
			@dirty
			*/
			float& getHeight();
			const float& getHeight() const;
			/**
			@dirty
			*/
			void setHeight(const float& s);

			/**
			@dirty
			*/
			float& getX();
			const float& getX() const;
			/**
			@dirty
			*/
			void setX(const float& newX);

			/**
			@dirty
			*/
			float& getY();
			const float& getY() const;
			/**
			@dirty
			*/
			void setY(const float& newY);

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
			@dirty
			*/
			Entity* getRootParent();
			const Entity* getRootParent() const;

			/**
			@internal
			@opengl
			*/
			virtual void clean();

			/**
			@dirty
			*/
			void reset();

			/**
			Makes this `Entity` dirty and the root level parent dirty. Should be used over `setProperty(Entity::DIRTY,true)` as it updaets the root parent.
			@dirty
			*/
			void makeDirty();
		protected:
			/**
			Should be called a by `Entity` when `System.update()` is called. Calls `onUpdate()`.
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onUpdate()`
			@throws InitializationError If the property `Entity::INIT` is false, meaning `init()` was not called.
			*/
			virtual void update();
			/**
			Should be called a by `Entity` when `System.init()` is called. Calls `onInit()`
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onInit()`
			@dirty
			@opengl
			@throws InitializationError If the property `Entity::INIT` is true, meaning `init()` has already been called.
			*/
			virtual void init();
			/**
			Should be called a by `Entity` when `System.destroy()` is called. Calls `onDestroy()`. Sets `Entity::INIT` to be false
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onDestroy()`
			@dirty
			@opengl
			@throws InitializationError If the property `Entity::INIT` is false, meaning `init()` was not called.
			*/
			virtual void destroy();

			/**
			Should be called by a `Entity` when the graphical `Window` clears the frame.
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onRender()`
			@opengl
			@see Entity#update()
			*/
			virtual void render();

			/**
			@internal
			@opengl
			*/
			virtual void hover();

			/**
			When `Entity.update()` is called, `onUpdate()` is called on all of it's children.
			@see System#update()
			@internal
			*/
			virtual void onUpdate() = 0;
			/**
			When `Entity.init()` is called, `onInit()` is called on all of it's children.
			@see System#init()
			@internal
			@opengl
			*/
			virtual void onInit() = 0;
			/**
			When `Entity.destroy()` is called, `onDestroy()` is called on all of it's children.
			@see System#destroy()
			@internal
			@opengl
			*/
			virtual void onDestroy() = 0;

			/**
			When `Entity.render()` is called, `onRender()` is called on all of it's children.
			@internal
			@opengl
			*/
			virtual void onRender() = 0;

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

			/**
			`std::vector` of this `Entity\'s` children. Use of this variable directly is unrecommended. Use `addChild()` or `removeChild()` instead.
			@internal
			*/
			std::vector<Entity*> children = std::vector<Entity*>();

			/**
			@internal
			*/
			TransformMatrix transformation;

		private:
			std::vector<Component*> components = std::vector<Component*>();

			EntityProperties properties = 0;


			Entity* parent = nullptr;

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

		class CallbackEntity: public Entity {
		public:
			void setInitCallback(const VoidFunctionPtr func);
			VoidFunctionPtr getInitCallback();
			const VoidFunctionPtr getInitCallback() const;

			void setUpdateCallback(const VoidFunctionPtr func);
			VoidFunctionPtr getUpdateCallback();
			const VoidFunctionPtr getUpdateCallback() const;

			void setRenderCallback(const VoidFunctionPtr func);
			VoidFunctionPtr getRenderCallback();
			const VoidFunctionPtr getRenderCallback() const;

			void setDestroyCallback(const VoidFunctionPtr func);
			VoidFunctionPtr getDestroyCallback();
			const VoidFunctionPtr getDestroyCallback() const;
		protected:
			void onInit() final;
			void onUpdate() final;
			void onRender() final;
			void onDestroy() final;
		private:
			VoidFunctionPtr destroyCallback = [] {}, updateCallback = [] {}, renderCallback = [] {}, initCallback = [] {};
		};//CallbackEntity

		class GraphicsEntity: public Entity {
			friend void ssl::bindBuffer(ogl::UniformBuffer&);
			friend void ssl::bindEntity(const GraphicsEntity*, ogl::ShaderProgram&);
			friend void ssl::fillBuffer(GraphicsEntity*);
			friend void ssl::checkInput();
		public:
			GraphicsEntity() noexcept;

			virtual ~GraphicsEntity() noexcept;

			/**
			@dirty
			*/
			ogl::UniformBuffer& getBuffer();
			const ogl::UniformBuffer& getBuffer() const;
			/**
			@dirty
			*/
			void setBuffer(const ogl::UniformBuffer& newBuffer);

			void clean() final;

			void init() final;

			void destroy() final;

			/**
			@dirty
			*/
			Color& getPaint();
			const Color& getPaint() const;
			/**
			@dirty
			*/
			void setPaint(const Color& c);

			/**
			@dirty
			*/
			float getOpacity();
			const float getOpacity() const;
			/**
			@dirty
			*/
			void setOpacity(const float f);

			bool operator==(const GraphicsEntity& other) const noexcept;
			bool operator!=(const GraphicsEntity& other) const noexcept;
		private:
			ogl::UniformBuffer buffer = ogl::UniformBuffer();

			Color paint = Color(1.0f, 1.0f, 1.0f, 1.0f);

			float opacity = 1.0f;
		};//GraphicsEntity

	}//gfx
}//mc

#endif