#include <MC-Testing/Catch.h>
#include <MC-System/Entity.h>
#include <iostream>

class DummyEntity : public mc::Entity {
public:
	DummyEntity(): Entity(){};


	bool isUpdated = false,isInit=false,isDestroyed=false;
protected:

	void customUpdate() {
		isUpdated = true;
	}

	void customInit() {
		isInit = true;
	}

	void customDestroy() {
		isDestroyed = true;
	}
};

namespace mc{

TEST_CASE("Testing entity properties","[entity][system]") {
	DummyEntity e = DummyEntity();

	REQUIRE_FALSE(e.getProperty(ENTITY_PROPERTY_DEAD));
	e.setProperty(ENTITY_PROPERTY_DEAD, true);

	REQUIRE(e.getProperty(ENTITY_PROPERTY_DEAD));

	REQUIRE(e.getProperty(ENTITY_PROPERTY_ENABLED));
	e.setProperty(ENTITY_PROPERTY_ENABLED, false);
	REQUIRE_FALSE(e.getProperty(ENTITY_PROPERTY_ENABLED));
	REQUIRE(e.getProperty(ENTITY_PROPERTY_DEAD));
}

TEST_CASE("Testing entity module and updating", "[entity][system]") {

	EntityModule* c =new EntityModule();
	
	System::addModule(*c);

	DummyEntity *e = new DummyEntity();

	REQUIRE(c->size() == 0);
	REQUIRE(e->isUpdated == false);

	c->addChild(*e);


	REQUIRE(c->size() == 1);

	c->init();

	REQUIRE(e->isInit);

	c->update();

	REQUIRE(e->isUpdated);

	c->destroy();

	REQUIRE(e->isDestroyed);

	DummyEntity child = DummyEntity();
	e->addChild(child);

	c->init();

	REQUIRE(child.isInit);

	c->update();

	REQUIRE(child.isUpdated);

	c->destroy();

	REQUIRE(child.isDestroyed);

	System::removeModule(*c);
}
}