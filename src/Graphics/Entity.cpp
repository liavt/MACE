/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity.h>
#include <MACE/Core/Constants.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/BitField.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Graphics/Renderer.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace mc {
	namespace gfx {
		namespace {
			//when you request for a solid color ColorAttachment, it will use the same texture to save memory
			ogl::Texture solidColor = ogl::Texture();
		};

		ColorAttachment::ColorAttachment() : Texture(), paint(0.0f, 0.0f, 0.0f, 0.0f) {}

		ColorAttachment::ColorAttachment(const ogl::Texture& tex, const Color& col) : Texture(tex), paint(col) {}

		ColorAttachment::ColorAttachment(const char * file) : ColorAttachment() {
			init();
			setDataFromFile(file);
		}

		ColorAttachment::ColorAttachment(const std::string & file) : ColorAttachment(file.c_str()) {}

		ColorAttachment::ColorAttachment(const Color& col) : ColorAttachment(solidColor, col) {
			if( !solidColor.isCreated() ) {
				solidColor.init();

				const float data[] = { 1,1,1,1 };
				solidColor.setData(data, 1, 1, GL_FLOAT, GL_RGBA);

				solidColor.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				solidColor.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
		}

		void ColorAttachment::setDataFromFile(const char * file) {
			int width, height, componentSize;

			Byte* image = stbi_load(file, &width, &height, &componentSize, STBI_rgb_alpha);

			setData(image, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);

			stbi_image_free(image);

			setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		void ColorAttachment::setDataFromFile(const std::string & file) {
			setDataFromFile(file.c_str());
		}

		Color& ColorAttachment::getPaint() {
			return paint;
		}

		const Color& ColorAttachment::getPaint() const {
			return paint;
		}

		void ColorAttachment::setPaint(const Color& col) {
			paint = col;
		}

		bool ColorAttachment::operator==(const ColorAttachment& other) const {
			return paint == other.paint && Texture::operator==(other);
		}

		bool ColorAttachment::operator!=(const ColorAttachment& other) const {
			return !operator==(other);
		}

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

		const std::vector<Entity*>& Entity::getChildren() const {
			return this->children;
		}


		void Entity::removeChild(const Entity& e) {
			makeDirty();

			for( Index i = 0; i < children.size(); i++ ) {
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

				for( Index i = 0; i < components.size(); i++ ) {
					components[i]->render(this);
				}
			}

		}

		void Entity::hover() {
			onHover();
			for( Index i = 0; i < components.size(); i++ ) {
				components[i]->hover(this);
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

				for( Index i = 0; i < components.size(); i++ ) {
					components[i]->clean(this);
				}

				setProperty(Entity::DIRTY, false);
			} else {
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

			for( Index i = 0; i < components.size(); i++ ) {
				components[i]->destroy(this);
			}
			components.clear();
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

		void Entity::onHover() {}

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
			//size() returns size_t which could be larger than unsigned int on some systems, meaning static_cast is necessary
			return static_cast<Size>(children.size());
		}

		void Entity::kill() {
			destroy();
		}

		void Entity::addChild(Entity * e) {
			makeDirty();

			children.push_back(e);
			e->setParent(this);

			if( getProperty(Entity::INIT) && !e->getProperty(Entity::INIT) ) {
				e->init();
			}
		}

		void Entity::addChild(Entity & e) {
			addChild(&e);
		}

		void Entity::addComponent(Component * component) {
			components.push_back(component);
			component->init(this);
		}

		void Entity::addComponent(Component & component) {
			addComponent(&component);
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
				if( position != Entity::DIRTY ) {
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
			if( transformation.scaler[0] != s ) {
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

		GraphicsEntity::GraphicsEntity() noexcept : Entity() {}

		GraphicsEntity::~GraphicsEntity() noexcept {}

		void GraphicsEntity::init() {
			if( !sslBuffer.isCreated() ) {
				sslBuffer.init();
			}
			ssl::bindBuffer(sslBuffer);
			Entity::init();
		}

		void GraphicsEntity::destroy() {
			Entity::destroy();
			sslBuffer.destroy();
		}

		bool GraphicsEntity::operator==(const GraphicsEntity & other) const noexcept {
			return sslBuffer == other.sslBuffer&&opacity == other.opacity&&Entity::operator==(other);
		}

		bool GraphicsEntity::operator!=(const GraphicsEntity & other) const noexcept {
			return !(operator==(other));
		}

		void GraphicsEntity::clean() {
			if( getProperty(Entity::DIRTY) ) {
				ssl::fillBuffer(this);
			}
			Entity::clean();
		}

		float GraphicsEntity::getOpacity() {
			makeDirty();

			return opacity;
		}

		const float GraphicsEntity::getOpacity() const {
			return opacity;
		}

		void GraphicsEntity::setOpacity(const float f) {
			if( opacity != f ) {
				makeDirty();

				opacity = f;
			}
		}

		void Component::clean(Entity *) {}

		void Component::hover(Entity *) {}

	}//gfx
}//mc
