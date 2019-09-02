/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <catch2/catch.hpp>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Entity2D.h>

class DummyEntity: public mc::gfx::Entity {
public:
	DummyEntity() : Entity() {};

	//bring protected members in public for testing purposes
	using mc::gfx::Entity::init;
	using mc::gfx::Entity::update;
	using mc::gfx::Entity::render;
	using mc::gfx::Entity::clean;
	using mc::gfx::Entity::setProperty;
	using mc::gfx::Entity::getProperty;

	bool isUpdated = false, isInit = false, isRendered = false, isCleaned = false;
protected:

	virtual void onUpdate() override {
		isUpdated = true;
	}

	virtual void onInit() override {
		isInit = true;
	}

	virtual void onRender() override {
		isRendered = true;
	}

	virtual void onClean() override {
		isCleaned = true;
	}
};

class DummyGroup: public mc::gfx::Group {
public:
	using Entity::init;
	using Entity::update;
	using Entity::render;
};

class DummyComponent: public mc::gfx::Component {
public:
	int updates;
	bool destroySelf = false;
	bool isInit = false;
	bool destroyed = false;

	void init() override {
		isInit = true;
	}

	void update() override {
		updates++;
	}

	bool isDone() {
		return destroySelf;
	}

	void render() override {

	}
};

template<typename T>
void requireEntityNotInit(mc::gfx::EntityPtr<T> en) {
	requireEntityNotInit(en.get());
}

void requireEntityNotInit(mc::gfx::Entity* en) {
	REQUIRE(en->getID() == 0);
	REQUIRE_FALSE(en->isInit());
	REQUIRE(en->getRoot() == nullptr);
}

template<typename T>
void requireEntityInit(mc::gfx::EntityPtr<T> en) {
	requireEntityInit(en.get());
}

void requireEntityInit(mc::gfx::Entity* en) {
	REQUIRE(en->getID() != 0);
	REQUIRE(en->isInit());
}


TEST_CASE("Testing basic entity operations", "[entity]") {
	DummyGroup en{};

	requireEntityNotInit(&en);
	REQUIRE(en.getChildren().empty());
	REQUIRE(en.getParent() == nullptr);

	en.init();

	requireEntityInit(&en);
	//since it is the only entity in the tree, it is the root
	REQUIRE(en.getRoot() == &en);
	REQUIRE(en.getChildren().empty());
}

TEST_CASE("Testing entity with children", "[entity]") {
	mc::gfx::EntityPtr<DummyEntity> root = mc::gfx::EntityPtr<DummyEntity>(new DummyEntity());

	requireEntityNotInit(root);

	REQUIRE(root->getChildren().empty());
	REQUIRE(root->getParent() == nullptr);

	mc::gfx::EntityPtr<DummyEntity> child = mc::gfx::EntityPtr<DummyEntity>(new DummyEntity());

	requireEntityNotInit(child);

	REQUIRE(child->getParent() == nullptr);

	root->addChild(child);

	requireEntityNotInit(child);

	REQUIRE(child->getParent() == root.get());

	root->init();

	requireEntityInit(root);
	requireEntityInit(child);

	REQUIRE(root->getID() != child->getID());
	REQUIRE(child->getRoot() == root.get());

}