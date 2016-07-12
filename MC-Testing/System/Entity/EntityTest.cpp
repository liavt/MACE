#include <MC-Testing/Catch.h>
#include <MC-System/Entities/Entity.h>
#include <iostream>

class DummyEntity : public mc::Entity {
public:
	DummyEntity(): Entity(){};


	bool isUpdated = false, isTicked = false, isInit = false, isDestroyed = false;
protected:

	void customUpdate() {
		std::cout << "updated"<<std::endl;
		isUpdated = false;
	}

	void customTick() {
		isTicked = true;
	}

	void customInit() {
		isInit = true;
	}

	void customDestroy() {
		isDestroyed = true;
	}
};

namespace mc{

TEST_CASE("Testing the getParent() function","[entity][system]") {
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();


	c.addChild(*e);

	REQUIRE(e->getParent()==c);

	c.removeChild(*e);

}
	
TEST_CASE("Testing the ENABLE property of an entity","[entity][system]") {
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();


	c.addChild(*e);

	REQUIRE(!e->isTicked);

	c.tick();

	REQUIRE(e->isTicked);

	c.update();

	REQUIRE(e->isUpdated);

	e->isTicked = false;
	e->setProperty(ENTITY_PROPERTY_TICK_ENABLED,false);

	c.tick();

	REQUIRE(!e->isTicked);

	e->isUpdated = false;
	e->setProperty(ENTITY_PROPERTY_UPDATE_ENABLED,false);

	c.update();

	REQUIRE(!e->isUpdated);

	c.removeChild(*e);

}

TEST_CASE("Testing death","[entity][system]") {//the sweet embrace of death
	
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();
	
	c.addChild(*e);

	REQUIRE(c.hasChild(*e));
	REQUIRE(!e->getProperty(ENTITY_PROPERTY_DEAD));

	c.tick();

	REQUIRE(!e->getProperty(ENTITY_PROPERTY_DEAD));
	REQUIRE(e->isTicked);

	
	e->isTicked = false;
	e->setProperty(ENTITY_PROPERTY_DEAD,true);

	c.tick();

	REQUIRE(!e->isTicked);
	REQUIRE(e->isDestroyed);
	REQUIRE(!c.hasChild(*e));	
	
}

TEST_CASE("Testing init()","[entity][system") {
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();


	REQUIRE(!e->getProperty(ENTITY_PROPERTY_INIT));

	c.addChild(*e);

	c.init();

	REQUIRE(e->isInit);
	REQUIRE(e->getProperty(ENTITY_PROPERTY_INIT));

	c.removeChild(*e);

}

TEST_CASE("Testing entity dirtiness","[entity][system]") {
	EntityModule c = EntityModule();
	
	DummyEntity* e = new DummyEntity();


	c.addChild(*e);

	REQUIRE(e->getProperty(ENTITY_PROPERTY_DIRTY));

	c.tick();

	REQUIRE(!e->getProperty(ENTITY_PROPERTY_DIRTY));

}

TEST_CASE("Testing entity properties","[entity][system]") {
	DummyEntity e = DummyEntity();

	REQUIRE_FALSE(e.getProperty(ENTITY_PROPERTY_DEAD));
	e.setProperty(ENTITY_PROPERTY_DEAD, true);

	REQUIRE(e.getProperty(ENTITY_PROPERTY_DEAD));

	REQUIRE(e.getProperty(ENTITY_PROPERTY_UPDATE_ENABLED));
	e.setProperty(ENTITY_PROPERTY_UPDATE_ENABLED, false);
	REQUIRE_FALSE(e.getProperty(ENTITY_PROPERTY_UPDATE_ENABLED));
	REQUIRE(e.getProperty(ENTITY_PROPERTY_DEAD));
}


TEST_CASE("Testing entity module and updating", "[entity][system]") {

	EntityModule *c = new EntityModule();

	DummyEntity *e = new DummyEntity();

	REQUIRE(c->size() == 0);
	REQUIRE(e->isTicked == false);

	c->addChild(*e);


	REQUIRE(c->size() == 1);

	c->init();

	REQUIRE(e->isInit);

	c->tick();

	REQUIRE(e->isTicked);

	c->update();

	REQUIRE(e->isUpdated);

	c->destroy();

	REQUIRE(e->isDestroyed);

	DummyEntity child = DummyEntity();
	e->addChild(child);

	c->init();

	REQUIRE(child.isInit);

	c->tick();

	REQUIRE(child.isTicked);

	c->update();

	REQUIRE(child.isUpdated);

	c->destroy();

	REQUIRE(child.isDestroyed);

	c->removeChild(*e);

	delete c;
}
}