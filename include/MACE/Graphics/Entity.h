/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_ENTITY_H
#define MACE__GRAPHICS_ENTITY_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>
#include <MACE/Utility/Transform.h>
#include <vector>
#include <unordered_map>
#include <memory>
//TODO find a way to not import type_traits as it's really heavy. a custom SFINAE would be nice
#include <type_traits>
#include <queue>

namespace mc {
	namespace gfx {
		class Component;
		class Entity;
	}

	namespace internal {
		/*
		In order to allow for fast Component lookup in Entity's with minimal runtime overhead
		and easy semantics, we used some clever template tricks to generate a unique "Type ID"
		for each Component. Whenever a class that needs to use the internal Entity Component Map
		it calls the getComponentTypeID() which returns a unique pointer address for each type T.

		These pointers are guarenteed to never return the same pointer for different types in a
		single execution of a program, but NOT throughout multiple executions.

		ComponentTypeID is used as the key in the unordered_map in Entity
		*/

		template<typename T>
		struct ComponentTypeIDPtr {
			static const T* const ID;
		};

		template<typename T>
		const T* const ComponentTypeIDPtr<T>::ID = nullptr;

		using ComponentTypeID = const void*;

		/**
		To ensure type safety, ExtendsComponent allows for SFINAE in classes that require Component
		*/
		template<typename T>
		using ExtendsComponent = std::enable_if_t < std::is_base_of_v<gfx::Component, T> >;

		template<typename T>
		using ExtendsEntity = std::enable_if_t < std::is_base_of_v<gfx::Entity, T> >;

		template<typename T>
		using DefaultConstrucible = std::enable_if_t < std::is_default_constructible_v<T>>;

		template<typename T, typename = ExtendsComponent<T>>
		MACE_NODISCARD MACE_CONSTEXPR inline ComponentTypeID getComponentTypeID() noexcept {
			return &ComponentTypeIDPtr<T>::ID;
		}
	}//internal

	namespace gfx {
		using EntityProperties = Byte;
		/**
		A 32-Bit integer representing an identifier for an `Entity`.
		<p>
		Each `Entity` is assigned a unique `EntityID` from 1 to `MAX_INT - 1` by it's `RootEntity`.
		No two `Entity`'s will share the same `EntityID` at the same time.
		<p>
		`0` is reserved as an identifier for an uninitialized `Entity` and will
		never be used as a valid `EntityID`.
		<p>
		`EntityID`'s may be reused throughout the lifetime of the program
		if `Entity`'s are being destroyed. However, no two `Entity`'s will
		use the same `EntityID` at the same time.

		@note An `EntityID` uses a 32-Bit integer in order to allow a `Renderer` to store an `EntityID` Z-buffer
		*/
		using EntityID = unsigned int;

		//forward-defining dependencies
		class Entity;
		class Texture;
		class Painter;
		class ComponentQueue;
		struct EaseSettings;

		struct Metrics {
			Transformation transform{};
			Transformation inherited{};

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
		There is no function to remove a `Component` from an `Entity`. Instead, the `Component` decides when to be removed
		from Component::update(Entity*). This makes sure that the `Component` completes whatever task it was trying to do.
		@see Entity::addComponent(Component&)
		@todo unit testing for clean() render() and hover()
		*/
		class MACE_NOVTABLE Component: public Initializable {
			friend class Entity;
			friend class ComponentQueue;
		public:
			Component() = default;
			virtual MACE__DEFAULT_OPERATORS(Component);

			Entity* getParent();

			bool isInit() const noexcept override;

			bool operator==(const Component& other) const;
			bool operator!=(const Component& other) const;
		protected:
			Entity* parent = nullptr;

			/**
			Called when this `Component` is added to the `Entity` via Entity::addComponent().
			@rendercontext
			*/
			virtual void init() override;
			/**
			Called when Entity::update() is called.
			@rendercontext
			*/
			virtual void update();
			/**
			Called when Entity::destroy() is called or the `Component` is removed via Component::update(Entity*),
			whichever comes first. Once Component::destroy(Entity*) is called, it is immediately removed from
			the `Entity`.
			@rendercontext
			*/
			virtual void destroy() override;

			virtual void render();
			/**
			Called when Entity::clean() is called and it was dirty.
			@rendercontext
			@return whether the `Component` needs to be removed from the `Entity`. Use this for tweens or timed events.
			*/
			virtual void clean(Metrics& metrics);

			/**
			@rendercontext
			*/
			virtual void hover();

			/**
			Calculate whether this `Component` has finished performing whatever functionality it needed.
			<br>
			This is used to implement timed events like tweening or easing.
			<br>
			This is the only method of which you can remove a `Compoment` from an `Entity`
			@return Whether this `Component` should be removed
			@note This function must be able to work WITHOUT a render context, as it can be called from any thread.
			*/
			MACE_NODISCARD virtual bool isDone() const;
		};//Component

		/**
		A pointer to a `Component` that automatically manages pointer lifecycle
		@note There is no default type as you are expected to provide the specific `Component` type as the template parameter. It uses the template typename to drive internal calculations
		*/
		template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
		using ComponentPtr = std::shared_ptr<T>;
		/**
		A pointer to a `Entity` that automatically manages pointer lifecycle
		*/
		template<typename T = Entity, typename = MACE__INTERNAL_NS::ExtendsEntity<T>>
		using EntityPtr = std::shared_ptr<T>;

		/**
		Abstract superclass for all graphical objects. Contains basic information like position, and provides a standard interface for communicating with graphical objects.
		<p>
		Prefer using `Component` instead of extending this class for one-time functionality.
		@see GraphicsEntity
		@see Entity2D
		@see Component
		*/
		class MACE_NOVTABLE Entity: public Initializable {
			friend class Component;
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
				When an `Entity` becomes dirty, it will propogate up the tree. It's parent will become dirty, it's parent will become dirty, etc. This will continue until it reaches the highest level `Entity`, which is usually the `GraphicsContextComponent`. From there, it will decide what to do based on it's `Entity::DIRTY` flag.
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
			<p>
			All subclasses of `Entity` should have a `noexcept` default constructor. Exceptions
			should instead be thrown from `init()`.
			*/
			Entity() noexcept;
			/**
			Destructor. Made `virtual` for inheritance.
			@see ~Entity()
			*/
			virtual MACE__DEFAULT_OPERATORS(Entity);

			/**
			Gets all of this `Entity's` children.
			@return an `std::vector` with all children of this `Entity`
			*/
			const std::vector<EntityPtr<Entity>>& getChildren() const;
			/**
			Removes a child.
			<p>
			This function calls no member functions of the argument, meaning that
			@throws ObjectNotFoundInArray if this `Entity` does not contain the argument returns `false`
			@param e an `Entity` to remove
			@see Entity::removeChild(Index)
			@dirty
			@rendercontext
			*/
			void removeChild(const Entity& e);

			/**
			@copydoc Entity::removeChild(const Entity&)
			<p>
			The passed pointer does not have it's lifecycle managed by this `Entity`
			@throws NullPointerError if the argument is `nullptr`
			@dirty
			@rendercontext
			@todo Make this function not have to be called in a render context by moving the destroy() call to render() and set a flag
			*/
			void removeChild(const Entity* e);

			/**
			@copydoc Entity::removeChild(const Entity*)
			*/
			void removeChild(EntityPtr<Entity> ent);

			/**
			Removes a child via location.
			@throws IndexOutOfBounds if the index is less than 0 or greater than {@link #size()}
			@param index Index of the `Entity` to be removed
			@see Entity::indexOf(const Entity&) const
			@see Entity::removeChild(const Entity&)
			@todo call Entity::destroy in the render thread
			@dirty
			@rendercontext
			*/
			void removeChild(const Index index);

			/**
			@copydoc removeChild(Index)
			*/
			void removeChild(const std::vector<EntityPtr<Entity>>::iterator& iter);

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
			Entity& operator[](const Index i);//get children via [i]

			/**
			`const` version of {@link #operator[](Index i)}
			@param i Location of an `Entity`
			@return Reference to the `Entity` located at `i`
			@see Entity::getChild(Index) const
			@see Entity::indexOf(const Entity&) const
			*/
			const Entity& operator[](const Index i) const;//get children via [i]

			/**

			Retrieves a child at a certain index.
			@param i Index of the `Entity`
			@return Reference to the `Entity` located at `i`
			@throws IndexOutOfBounds if `i` is less than `0` or greater than {@link #size()}
			@see Entity::operator[]
			@see Entity::indexOf(const Entity&) const
			*/
			Entity& getChild(const Index i);
			/**
			`const` version of {@link #getChild(Index)}

			@param i `Index` of the `Entity`
			@return Reference to the `Entity` located at `i`
			@throws IndexOutOfBounds if `i` is less than `0` or greater than {@link #size()}
			@see Entity::operator[]
			@see Entity::indexOf(const Entity&) const
			*/
			const Entity& getChild(const Index i) const;

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
			MACE_NODISCARD bool isEmpty() const;

			/**
			Retrieves the beginning of the children of this `Entity`
			@return Pointer to the first `Entity`
			@see Entity::end()
			@see Entity::size()
			*/
			std::vector<EntityPtr<Entity>>::iterator begin();
			/**
			Retrieves the end of the children of this `Entity`
			@return End of the last `Entity`
			@see Entity::begin()
			@see Entity::size()
			*/
			std::vector<EntityPtr<Entity>>::iterator end();

			/**
			Calculates the amount of children this `Entity` has.
			@return Size of this `Entity`
			@see Entity::isEmpty()
			*/
			MACE_NODISCARD Size size() const;

			MACE_NODISCARD EntityID getID() const;

			/**
			@dirty
			*/
			Transformation& getTransformation();
			const Transformation& getTransformation() const;
			/**
			@dirty
			*/
			void setTransformation(const Transformation& trans);

			/**
			@dirty
			*/
			Entity& translate(const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z = 0.0f);
			/**
			@dirty
			*/
			Entity& rotate(const RelativeRadian x, const RelativeRadian y, const RelativeRadian z = 0.0f);
			/**
			@dirty
			*/
			Entity& scale(const RelativeScale x, const RelativeScale y, const RelativeScale z = 1.0f);

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

			MACE_NODISCARD bool isInit() const noexcept override;

			/**
			@dirty
			*/
			void addChild(Entity& e);
			/**
			@copydoc Entity::addChild(Entity&)
			*/
			void addChild(Entity* e)MACE_EXPECTS(ent != nullptr);
			/**
			@copydoc Entity::addChild(Entity&)
			*/
			void addChild(EntityPtr<Entity> ent) MACE_EXPECTS(ent != nullptr);

			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
			inline void addComponent(T & com) {
				addComponent<T>(&com);
			}

			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
			inline void addComponent(T * com) {
				addComponent<T>(ComponentPtr<T>(com, [](T*) {}));
			}
			/**
			@param com The SmartPointer of an `Entity`. Ownership of the pointer will change meaning this parameter cannot be marked `const`
			*/
			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
			void addComponent(ComponentPtr<T> com) MACE_EXPECTS(com != nullptr) {
				MACE_ASSERT(com != nullptr, "Added Component was nullptr");
				MACE_ASSERT(!hasComponent<T>(), "Component of type already exists");

				// in case T declares init() as private, cast it to Component which is friends with Entity
				auto component = std::static_pointer_cast<Component>(com);
				component->parent = this;
				if (isInit()) {
					component->init();
				}
				components.emplace(MACE__INTERNAL_NS::getComponentTypeID<T>(), component);

				makeDirty();
			}

			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
			MACE_NODISCARD ComponentPtr<T> getComponent() {
				//TODO use the __cpp_lib_generic_unordered_hash_lookup (201902L) macro to use precomputated hashes

				auto it = components.find(MACE__INTERNAL_NS::getComponentTypeID<T>());
				if (it != components.end()) {
					return std::static_pointer_cast<T>(it->second);
				}

				return nullptr;
			}

			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
			MACE_NODISCARD const ComponentPtr<T> getComponent() const {
				auto it = components.find(MACE__INTERNAL_NS::getComponentTypeID<T>());
				if (it != components.end()) {
					return std::static_pointer_cast<T>(it->second);
				}

				return nullptr;
			}

			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>, typename = MACE__INTERNAL_NS::DefaultConstrucible<T>>
			MACE_NODISCARD ComponentPtr<T> getOrCreateComponent() {
				auto it = components.find(MACE__INTERNAL_NS::getComponentTypeID<T>());
				if (it != components.end()) {
					return std::static_pointer_cast<T>(it->second);
				}

				ComponentPtr<T> newCom = ComponentPtr<T>(new T());
				addComponent(newCom);
				return newCom;
			}

			template<typename T, typename = MACE__INTERNAL_NS::ExtendsComponent<T>>
			bool hasComponent() noexcept {
				return components.count(MACE__INTERNAL_NS::getComponentTypeID<T>()) > 0;
			}

			const RelativeScale& getWidth() const;
			/**
			@dirty
			@copydoc Entity::getWidth() const
			*/
			RelativeScale& getWidth();
			/**
			@dirty
			*/
			void setWidth(const RelativeScale s);

			const RelativeScale& getHeight() const;
			/**
			@copydoc Entity::getHeight() const
			@dirty
			*/
			RelativeScale& getHeight();
			/**
			@dirty
			*/
			void setHeight(const RelativeScale s);

			const RelativeScale& getDepth() const;
			/**
			@dirty
			@copydoc Entity::getHeight() const
			*/
			RelativeScale& getDepth();
			/**
			@dirty
			*/
			void setDepth(const RelativeScale s);

			const RelativeTranslation& getX() const;
			/**
			@dirty
			@copydoc Entity::getX() const
			*/
			RelativeTranslation& getX();
			/**
			@dirty
			*/
			void setX(const RelativeTranslation newX);

			const RelativeTranslation& getY() const;
			/**
			@dirty
			@copydoc Entity::getY() const
			*/
			RelativeTranslation& getY();
			/**
			@dirty
			*/
			void setY(const RelativeTranslation newY);

			const RelativeTranslation& getZ() const;
			/**
			@dirty
			@copydoc Entity::getZ() const
			*/
			RelativeTranslation& getZ();
			/**
			@dirty
			*/
			void setZ(const RelativeTranslation newY);

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
			If this `Entity` does not have any parent, `this` becomes
			the root of the tree
			@return The root `Entity` of which this `Entity` belongs to.
			*/
			const Entity* getRoot() const;
			/**
			@copydoc Entity::getRoot() const
			*/
			Entity* getRoot();

			/**
			@rendercontext
			*/
			const Metrics& getMetrics() const;

			/**
			Makes this `Entity` dirty and root dirty.
			<p>
			Should be used over `setProperty(Entity::DIRTY,true)` as it updates the root parent.
			@see Entity::getRoot()
			@dirty
			*/
			void makeDirty() noexcept;

			/**
			Tween this `Entity`'s transformation

			@dirty
			@param start The start of the tween
			@param dest The destination of the tween
			@param settings What easing settings to use during the tween
			@see `TweenComponent`
			*/
			//Function overloading would require the EaseSettings constructor, which means we have to include the entire header instead of forward declaring.
			void tween(const Transformation start, const Transformation dest, const EaseSettings settings);
			/**
			@copydoc #tween(const Transformation, const Transformation, const EaseSettings)
			*/
			void tween(const Transformation start, const Transformation dest);
			/**
			@copydoc #tween(const Transformation, const Transformation, const EaseSettings)
			*/
			void tween(const Transformation dest, const EaseSettings settings);
			/**
			@copydoc #tween(const Transformation, const Transformation, const EaseSettings)
			*/
			void tween(const Transformation dest);

			/**
			@internal
			@rendercontext
			*/
			virtual void hover();

		protected:
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
			@rendercontext
			@throws InitializationError If the property `Entity::INIT` is true, meaning `init()` has already been called.
			*/
			virtual void init() override;
			/**
			@internal
			@rendercontext
			*/
			virtual void clean();
			/**
			Should be called a by `Entity` when `MACE.destroy()` is called. Calls `onDestroy()`. Sets `Entity::INIT` to be false
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onDestroy()`
			@dirty
			@rendercontext
			@throws InitializationError If the property `Entity::INIT` is false, meaning `init()` was not called.
			*/
			virtual void destroy() override;

			/**
			Should be called by a `Entity` when the graphical `Window` clears the frame.
			<p>
			Overriding this function is dangerous. Only do it if you know what you are doing. Instead, override `onRender()`
			@rendercontext
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
			@rendercontext
			*/
			virtual void onInit();
			/**
			When `Entity.destroy()` is called, `onDestroy()` is called on all of it's children.
			@see MACE#destroy()
			@internal
			@rendercontext
			*/
			virtual void onDestroy();

			/**
			When `Entity.render()` is called, `onRender()` is called on all of it's children.
			@internal
			@rendercontext
			*/
			virtual void onRender();

			/**
			@internal
			@rendercontext
			*/
			virtual void onClean();

			/**
			@internal
			@rendercontext
			*/
			virtual void onHover();



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
			void setProperties(const EntityProperties& b);

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
			bool getProperty(const Byte position) const noexcept;
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
			void setProperty(const Byte position, const bool value) noexcept;
		private:
			/**
			`std::vector` of this `Entity\'s` children. Use of this variable directly is unrecommended. Use `addChild()` or `removeChild()` instead.
			@internal
			*/
			std::vector<EntityPtr<Entity>> children{};
			std::unordered_map<MACE__INTERNAL_NS::ComponentTypeID, ComponentPtr<Component>> components{};

			Entity* parent = nullptr;
			Entity* root = nullptr;
			EntityID id = 0;

			/**
			@internal
			*/
			Transformation transformation;

			/**
			@internal
			*/
			Metrics metrics;

			EntityProperties properties = Entity::DEFAULT_PROPERTIES;
		};//Entity

		class Group: public Entity {};//Group
	}//gfx

	namespace internal {
		class RootComponent: public gfx::Component {
			friend class gfx::Entity;
		public:
			MACE_NODISCARD gfx::Entity* getEntityByID(const gfx::EntityID id) const;
		private:
			std::vector<gfx::Entity*> ids{};

			MACE_NODISCARD gfx::EntityID generateID(gfx::Entity* e);

			void deleteID(const gfx::EntityID id);

			void init() override;
			void destroy() override;
		};//RootComponent
	}
}//mc

#endif
