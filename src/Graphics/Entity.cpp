/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity.h>
#include <MACE/System/Constants.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/BitField.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Graphics/Renderer.h>
#include <string>
#include <iostream>

namespace mc {
	namespace gfx {

		bool Entity::hasChild(Entity & e) const {
			for( Size i = 0; i < children.size(); ++i ) {
				if( children[i] == &e ) {
					return true;
				}
			}
			return false;
		}

		void Entity::clearChildren() {
			makeDirty();

			while( !children.empty() ) {
				children.back()->setParent(nullptr);
				children.pop_back();
			}
		}

		void Entity::clearComponents() {
			for( Index i = 0; i < components.size(); i++ ) {
				components[i]->destroy(this);
			}
			components.clear();
		}

		const std::vector<Entity*>& Entity::getChildren() const {
			return this->children;
		}


		void Entity::removeChild(const Entity& e) {
			makeDirty();

			for( Size i = 0; i < children.size(); i++ ) {
				if( &e == children[i] ) {
					removeChild(i);
					return;
				}
			}
			throw ObjectNotFoundInArrayException("Specified argument is not a valid object in the array!");
		}

		void Entity::removeChild(Index index) {
			makeDirty();

			if( children.empty() ) {
				throw IndexOutOfBoundsException("Can\'t remove a child from an empty entity!");
			} else if( index >= children.size() ) {
				throw IndexOutOfBoundsException(std::to_string(index) + " is larger than the amount of children!");
			} else if( children.size() == 1 ) {
				children.clear();
			} else {
				children.erase(children.begin() + index);
			}
		}

		void Entity::render() {
			if( !getProperty(Entity::INIT) ) {
				init();
			}

			//we want to do the actual cleaning in render() because clean() does some graphical work
			if( getProperty(Entity::DIRTY) ) {
				clean();
			}

			//check if we can render
			if( !getProperty(Entity::RENDER_DISABLED) ) {
				//we want to render as fast as possible, so we avoid doing anything but rendering here. components and inheritence is done during update()
				onRender();

				for( Index i = 0; i < children.size(); ++i ) {
					children[i]->render();
				}
			}

		}

		void Entity::clean() {
			if( getProperty(Entity::DIRTY) ) {
				onClean();

				for( Size i = 0; i < children.size(); i++ ) {
					if( children[i]->getProperty(Entity::INIT) ) {
						children[i]->setProperty(Entity::DIRTY, true);
						children[i]->clean();
					}
				}

				setProperty(Entity::DIRTY, false);
			}else{
				for( Size i = 0; i < children.size(); i++ ) {
					if( children[i]->getProperty(Entity::INIT) ) {
						children[i]->clean();
					}
				}
			}
		}

		Entity * Entity::getRootParent() {
			makeDirty();

			if( !hasParent() ) return nullptr;

			Entity* par = getParent();

			//get the highest level element
			while( par->hasParent() ) {
				par = par->getParent();
			}

			return par;
		}

		const Entity * Entity::getRootParent() const {
			const Entity* par = getParent();

			if( !hasParent() ) return nullptr;

			//get the highest level element
			while( par->hasParent() ) {
				par = par->getParent();
			}

			return par;
		}

		void Entity::reset() {

			clearChildren();
			properties = 0;
			transformation.reset();
			clearComponents();
		}

		void Entity::makeDirty() {
			//checking for the parent can be slow. only want to do the pointer stuff if its not already dirty
			if( !getProperty(Entity::DIRTY) ) {
				setProperty(Entity::DIRTY, true);
				if( hasParent() ) {
					getRootParent()->setProperty(Entity::DIRTY, true);
				}
			}
		}

		void Entity::onClean() {}

		void Entity::setParent(Entity * par) {
			makeDirty();

			this->parent = par;
		}

		Entity* const Entity::getParent() {
			return parent;
		}

		const Entity* const Entity::getParent() const {
			return parent;
		}

		bool Entity::hasParent() const {
			return parent != 0 || parent != nullptr;
		}

		void Entity::addChild(Entity & e) {
			makeDirty();

			children.push_back(&e);
			e.setParent(this);
		}

		Entity& Entity::operator[](Index i) {
			return *children[i];
		}

		const Entity& Entity::operator[](Index i) const {
			return *children[i];
		}

		Entity& Entity::getChild(Index i) {
			return *children.at(i);
		}

		const Entity& Entity::getChild(Index i) const {
			return *children.at(i);
		}

		int Entity::indexOf(const Entity & e) const {
			for( Index i = 0; i < children.size(); i++ ) {
				if( children[i] == &e ) {
					return i;
				}
			}
			return -1;
		}

		std::vector<Entity*>::iterator Entity::begin() {
			return children.begin();
		}

		std::vector<Entity*>::iterator Entity::end() {
			return children.end();
		}

		Size Entity::size() const {
			return children.size();
		}

		void Entity::kill() {
			destroy();
		}

		void Entity::addComponent(Component & action) {
			components.push_back(&action);
			action.init(this);
		}

		std::vector<Component*> Entity::getComponents() {
			return components;
		}

		void Entity::update() {
			//check if we can update
			if( !getProperty(Entity::UPDATE_DISABLED) ) {

				//update the components of this entity
				for( Index i = 0; i < components.size(); i++ ) {
					Component* a = components.at(i);
					if( a->update(this) ) {
						a->destroy(this);
						components.erase(components.begin() + i);
						i--;//update the index after a removal, so we dont get an exception for accessing deleted memory
					}
				}



				onUpdate();

				//call update() on children
				for( Index i = 0; i < children.size(); i++ ) {
					if( children[i]->getProperty(Entity::DEAD) ) {
						children[i]->kill();
						removeChild(i);
						i--;//update the index after the removal of an element, dont want an error
						return;
					}
					children[i]->update();
				}
			}
		}

		void Entity::init() {
			if( getProperty(Entity::INIT) ) {
				throw InitializationError("Entity can not have init() called twice.");
			}
			makeDirty();
			for( Index i = 0; i < children.size(); ++i ) {
				children[i]->init();
			}
			onInit();
			setProperty(Entity::INIT, true);
		}

		void Entity::destroy() {
			if( !getProperty(Entity::INIT) ) {
				throw InitializationError("Entity can not have destroy() called when it has not been initialized");
			}
			makeDirty();
			for( Index i = 0; i < children.size(); ++i ) {
				children[i]->destroy();
			}
			onDestroy();
			reset();
		}


		Entity::Entity() noexcept {}

		Entity::Entity(const Entity & obj) noexcept {
			children = obj.children;
			properties = obj.properties;
		}

		Entity::~Entity() noexcept {
			children.clear();
		}


		EntityProperties& Entity::getProperties() {
			makeDirty();

			return properties;
		}

		const EntityProperties& Entity::getProperties() const {
			return properties;
		}

		void Entity::setProperties(EntityProperties& b) {
			if( b != properties ) {
				makeDirty();
				properties = b;
			}
		}

		bool Entity::getProperty(const Byte position) const {
#ifdef MACE_ERROR_CHECK
			if( position > 8 )throw IndexOutOfBoundsException("Input position is greater than 8");
			else if( position < 0 )throw IndexOutOfBoundsException("Input position is less than 0!");
#endif
			return properties.getBit(position);
		}

		void Entity::setProperty(const Byte position, const bool value) {
#ifdef MACE_ERROR_CHECK
			if( position > 8 )throw IndexOutOfBoundsException("Input position is greater than 8");
			else if( position < 0 )throw IndexOutOfBoundsException("Input position is less than 0!");
#endif
			if( properties.getBit(position) != value ) {
				if( position != Entity::DIRTY && position != Entity::HOVERED ) {
					properties.setBit(Entity::DIRTY, true);
				}
				properties.setBit(position, value);
			}
		}

		TransformMatrix & Entity::getTransformation() {
			makeDirty();

			return transformation;
		}

		const TransformMatrix & Entity::getTransformation() const {
			return transformation;
		}

		//we are trans-supportive here!
		void Entity::setTransformation(TransformMatrix & trans) {
			if( transformation != trans ) {
				makeDirty();

				transformation = trans;
			}
		}

		Entity & Entity::translate(float x, float y, float z) {
			makeDirty();

			transformation.translate(x, y, z);

			return *this;
		}

		Entity & Entity::rotate(float x, float y, float z) {
			makeDirty();

			transformation.rotate(x, y, z);

			return *this;
		}

		Entity & Entity::scale(float x, float y, float z) {
			makeDirty();

			transformation.scale(x, y, z);

			return *this;
		}

		bool Entity::operator==(const Entity& other) const noexcept {
			if( other.properties != properties ) {
				return false;
			}
			if( other.parent != parent ) {
				return false;
			}
			if( other.transformation != transformation ) {
				return false;
			}
			if( other.components != components ) {
				return false;
			}
			return children == other.children;
		}

		bool Entity::operator!=(const Entity & other) const noexcept {
			return !(this == &other);
		}

		void Group::onInit() {}

		void Group::onUpdate() {}

		void Group::onRender() {}

		void Group::onDestroy() {}

		float & Entity::getWidth() {
			makeDirty();

			return transformation.scaler[0];
		}
		const float & Entity::getWidth() const {
			return transformation.scaler[0];
		}
		void Entity::setWidth(const float & s) {
			if( transformation.scaler[1] != s ) {
				makeDirty();

				transformation.scaler[0] = s;
			}
		}
		float & Entity::getHeight() {
			makeDirty();

			return transformation.scaler[1];
		}
		const float & Entity::getHeight() const {
			return transformation.scaler[1];
		}
		void Entity::setHeight(const float & s) {
			if( transformation.scaler[1] != s ) {
				makeDirty();

				transformation.scaler[1] = s;
			}
		}
		float & Entity::getX() {
			makeDirty();

			return transformation.translation[0];
		}
		const float & Entity::getX() const {
			return transformation.translation[0];
		}
		void Entity::setX(const float & newX) {
			if( transformation.translation[0] != newX ) {
				makeDirty();

				transformation.translation[0] = newX;
			}
		}
		float & Entity::getY() {
			makeDirty();

			return transformation.translation[1];
		}
		const float & Entity::getY() const {
			return transformation.translation[1];
		}
		void Entity::setY(const float & newY) {
			if( transformation.translation[1] != newY ) {
				makeDirty();

				transformation.translation[1] = newY;
			}
		}
		ogl::UniformBuffer & GraphicsEntity::getBuffer() {
			makeDirty();
			return buffer;
		}
		const ogl::UniformBuffer & GraphicsEntity::getBuffer() const {
			return buffer;
		}
		void GraphicsEntity::setBuffer(const ogl::UniformBuffer & newBuffer) {
			if( newBuffer != buffer ) {
				makeDirty();
				buffer = newBuffer;
			}
		}

		void CallbackEntity::onInit() {
			initCallback();
		}

		void CallbackEntity::onUpdate() {
			updateCallback();
		}

		void CallbackEntity::onRender() {
			renderCallback();
		}

		void CallbackEntity::onDestroy() {
			destroyCallback();
		}

		void CallbackEntity::setInitCallback(const VoidFunctionPtr func) {
			if( func == 0 || func == nullptr )throw NullPointerException("Init callback must be a valid pointer, and not null!");
			initCallback = func;
		}

		VoidFunctionPtr CallbackEntity::getInitCallback() {
			return initCallback;
		}

		const VoidFunctionPtr CallbackEntity::getInitCallback() const {
			return initCallback;
		}

		void CallbackEntity::setUpdateCallback(const VoidFunctionPtr func) {
			if( func == 0 || func == nullptr )throw NullPointerException("Update callback must be a valid pointer, and not null!");
			updateCallback = func;
		}

		VoidFunctionPtr CallbackEntity::getUpdateCallback() {
			return updateCallback;
		}

		const VoidFunctionPtr CallbackEntity::getUpdateCallback() const {
			return updateCallback;
		}

		void CallbackEntity::setRenderCallback(const VoidFunctionPtr func) {
			if( func == 0 || func == nullptr )throw NullPointerException("Render callback must be a valid pointer, and not null!");
			renderCallback = func;
		}

		VoidFunctionPtr CallbackEntity::getRenderCallback() {
			return renderCallback;
		}

		const VoidFunctionPtr CallbackEntity::getRenderCallback() const {
			return renderCallback;
		}

		void CallbackEntity::setDestroyCallback(const VoidFunctionPtr func) {
			if( func == 0 || func == nullptr )throw NullPointerException("Destroy callback must be a valid pointer, and not null!");
			destroyCallback = func;
		}

		VoidFunctionPtr CallbackEntity::getDestroyCallback() {
			return destroyCallback;
		}

		const VoidFunctionPtr CallbackEntity::getDestroyCallback() const {
			return destroyCallback;
		}

		GraphicsEntity::GraphicsEntity() noexcept : Entity() {}

		GraphicsEntity::GraphicsEntity(ogl::Texture & t) noexcept : Entity() {
			texture = t;
		}

		GraphicsEntity::~GraphicsEntity() noexcept {}

		void GraphicsEntity::setTexture(ogl::Texture & tex) {
			if( tex != texture ) {
				makeDirty();

				texture = tex;
			}
		}

		ogl::Texture & GraphicsEntity::getTexture() {
			makeDirty();

			return texture;
		}

		const ogl::Texture & GraphicsEntity::getTexture() const {
			return texture;
		}

		void GraphicsEntity::init() {
			if( !buffer.isCreated() ) {
				buffer.init();
			}
			ssl::bindBuffer(buffer);
			Entity::init();
		}

		void GraphicsEntity::destroy() {
			Entity::destroy();
			buffer.destroy();
		}

		bool GraphicsEntity::operator==(const GraphicsEntity & other) const noexcept {
			return texture == other.texture&&buffer == other.buffer&&Entity::operator==(other);
		}

		bool GraphicsEntity::operator!=(const GraphicsEntity & other) const noexcept {
			return !(operator==(other));
		}

		void GraphicsEntity::onClean() {
			ssl::fillBuffer(this);
		}

		Color & GraphicsEntity::getPaint() {
			makeDirty();

			return texture.getPaint();
		}

		const Color & GraphicsEntity::getPaint() const {
			return texture.getPaint();
		}

		void GraphicsEntity::setPaint(const Color & c) {
			if( texture.getPaint() != c ) {
				makeDirty();

				texture.setPaint(c);
			}
		}

		float GraphicsEntity::getOpacity() {
			makeDirty();

			return texture.getOpacity();
		}

		const float GraphicsEntity::getOpacity() const {
			return texture.getOpacity();
		}

		void GraphicsEntity::setOpacity(const float f) {
			if( texture.getOpacity() != f ) {
				makeDirty();

				texture.setOpacity(f);
			}
		}

	}//gfx
}//mc