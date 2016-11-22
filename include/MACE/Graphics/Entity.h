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
#include <MACE/Graphics/Buffer.h>
#include <vector>

namespace mc {
	namespace gfx {

		using EntityProperties = BitField;

		//values defining which bit in a byte every propety is, or how much to bit shift it
		/**
		Bit location representing whether an `Entity` is dead.
		<p>
		If `true,` any {@link Entity} holding it will remove it and call `kill()`
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_DEAD = 0;
		/**
		Property defining if an `Entity` can be updated. If this is `true`, `update()` will be called by it's parent.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_UPDATE_DISABLED = 1;
		/**
		Property defining if an `Entity` can be rendered. If this is `true`, `render()` will be called by it's parent.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_RENDER_DISABLED = 2;

		/**
		Flag representing whether an Entity's init() function has been called.
		<p>
		If destroy() or update() is called and this is `false`, an `InitializationError` is thrown.
		<p>
		If init() is called and this is `true`, an `InitializationError` is thrown.
		@see Entity#getProperty(unsigned int)
		*/
		const Byte ENTITY_INIT = 3;

		/**
		Flag representing whether an Entity's X position should move when it's parent is resized.
		@see ENTITY_STRETCH_Y
		*/
		const Byte ENTITY_STRETCH_X = 4;

		/**
		Flag representing whether an Entity's X position should move when it's parent is resized.
		@see ENTITY_STRETCH_X
		*/
		const Byte ENTITY_STRETCH_Y = 5;

		/**
		Flag representing whether an entity has been hovered over
		@see ENTITY_CLICKED
		*/
		const Byte ENTITY_HOVERED = 6;

		/**
		Flag representing whether an entity has been clicked on.
		@see ENTITY_HOVERED
		*/
		const Byte ENTITY_CLICKED = 7;


		//forward-defining entity for component
		class Entity;

		class Component {
		public:
			virtual void init(Entity* e) = 0;
			virtual bool update(Entity* e) = 0;
			virtual void destroy(Entity* e) = 0;
		};//Component


		/**
		Abstract superclass for all graphical objects. Contains basic information like position, and provides a standard interface for communicating with graphical objects.
		*/
		class Entity {
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
			@see ~Entity()
			*/
			virtual ~Entity();

			/**
			Should be called a by `Entity` when `System.update()` is called. Calls `customUpdate()`.
			<p>
			When this function is inherited or overwritten, you must call the base class's `update()` funtion. For example:{@code
			class Derived : public Entity{
			void update(){
			Entity::update()//must call
			...
			}
			}
			}
			@throws InitializationError If the property `ENTITY_INIT` is false, meaning `init()` was not called.
			*/
			void update();
			/**
			Should be called a by `Entity` when `System.init()` is called. Calls `customInit()`
			<p>
			When this function is inherited or overwritten, you must call the base class's `init()` funtion. For example:{@code
			class Derived : public Entity{
			void init(){
			Entity::init()//must call
			...
			}
			}
			}
			@throws InitializationError If the property `ENTITY_INIT` is true, meaning `init()` has already been called.
			*/
			void init();
			/**
			Should be called a by `Entity` when `System.terminate()` is called. Calls `customDestroy()`. Sets `ENTITY_INIT` to be false
			<p>
			When this function is inherited or overwritten, you must call the base class's `destroy()` funtion. For example:{@code
			class Derived : public Entity{
			void destroy(){
			Entity::destroy()//must call
			...
			}
			}
			}
			@throws InitializationError If the property `ENTITY_INIT` is false, meaning `init()` was not called.
			*/
			void destroy();

			/**
			Should be called by a `Entity` when the graphical `Window` clears the frame.
			<p>
			When this function is inherited or overwritten, you must call the base class's `render()` funtion. For example:{@code
			class Derived : public Entity{
			void render(){
			Entity::render()//must call
			...
			}
			}
			}
			*/
			void render();


			/**
			Gets all of this `Entity's` children.
			@return an `std::vector` with all children of this `Entity`
			*/
			const std::vector<Entity*>& getChildren() const;
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
			Checks to see if this `Entity` contains an `Entity`
			@param e Reference to an `Entity`
			@return `false` if this `Entity` doesn't contain the referenced `Entity`, `true` otherwise
			@see #indexOf(const Entity& ) const
			*/
			bool hasChild(Entity& e) const;

			/**
			Removes EVERY `Entity` from this `Entity`
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
			@param b New `Entity` properties
			@see getProperties()
			@see getProperty(Index) const
			@see setProperty(Index, bool)
			*/
			void setProperties(EntityProperties& b);

			/**
			Retrieve the value of a property. Property consants start with `ENTITY_`
			@param position Location of the property based on a constant
			@return `true` or `false` based on the postition
			@see setProperty(Index, bool)
			@see getProperties()
			@see setProperties(ByteField&)
			*/
			bool getProperty(const Index position) const;
			/**
			Set a property to be `true` or `false`.Property consants start with `ENTITY_`
			@param position Location of the property based on a constant
			@param value Whether it is `true` or `false`
			@see getProperty(Index) const
			@see getProperties()
			@see setProperties(ByteField&)
			*/
			void setProperty(const Index position, const bool value);

			TransformMatrix& getBaseTransformation();
			const TransformMatrix& getBaseTransformation() const;
			void setBaseTransformation(TransformMatrix& trans);

			Entity& translate(float x, float y, float z);
			Entity& rotate(float x, float y, float z);
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

			void addChild(Entity& e);

			/**
			Automatically called when `ENTITY_PROPERTY_DEAD` is true. Removes this entity from it's parent, and calls it's `destroy()` method.
			@see getParent()
			@todo Verify if getParent().remove(this) needs to be called
			*/
			void kill();

			void addComponent(Component& action);
			std::vector<Component*> getComponents();

			float& getWidth();
			const float& getWidth() const;
			void setWidth(const float& s);

			float& getHeight();
			const float& getHeight() const;
			void setHeight(const float& s);

			float& getX();
			const float& getX() const;
			void setX(const float& newX);

			float& getY();
			const float& getY() const;
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
		protected:
			TransformMatrix baseTransformation;

			/**
			When `Entity.update()` is called, `customUpdate()` is called on all of it's children.
			@see System#update()
			*/
			virtual void customUpdate() = 0;
			/**
			When `Entity.init()` is called, `customInit()` is called on all of it's children.
			@see System#init()
			*/
			virtual void customInit() = 0;
			/**
			When `Entity.destroy()` is called, `customDestroy()` is called on all of it's children.
			@see System#terminate()
			*/
			virtual void customDestroy() = 0;

			/**
			When `Entity.render()` is called, `customRender()` is called on all of it's children.
			*/
			virtual void customRender() = 0;
		private:
			/**
			`std::vector` of this `Entity\'s` children. Use of this variable directly is unrecommended. Use `addChild()` or `removeChild()` instead.
			*/
			std::vector<Entity*> children = std::vector<Entity*>();

			Entity* parent = nullptr;

			EntityProperties properties = 0;

			std::vector<Component*> components = std::vector<Component*>();

			/**
			Calls `update()` on all of it's children and components.
			*/
			virtual void updateChildren();
			/**
			Calls `init()` on all of it's children.
			*/
			void initChildren();
			/**
			Calls `destroy()` on all of it's children and components.
			*/
			void destroyChildren();//think of the children!
			/**
			Calls `render()` on all of it's children.
			*/
			void renderChildren();

			void setParent(Entity* parent);
		};//Entity

		class Group: public Entity {
		protected:
			void customInit();
			void customUpdate();
			void customRender();
			void customDestroy();

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
			void customInit();
			void customUpdate();
			void customRender();
			void customDestroy();
		private:
			VoidFunctionPtr destroyCallback = [] {}, updateCallback = [] {}, renderCallback = [] {}, initCallback = [] {};
		};//CallbackEntity

		class GraphicsEntity: public Entity {
		public:

			GraphicsEntity();

			GraphicsEntity(Texture& t);
			virtual ~GraphicsEntity();

			void setTexture(Texture& tex);
			Texture& getTexture();
			const Texture& getTexture() const;
		protected:
			Texture texture;
		};//GraphicsEntity

	}//gfx
}//mc

#endif