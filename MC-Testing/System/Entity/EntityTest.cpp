#include <MC-Testing/Catch.h>
#include <MC-System/Entities/Entity.h>
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

TEST_CASE("Testing the getParent() function","[entity][system]") {
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();

	System::addModule(c);

	c.addChild(*e);

	REQUIRE(e->getParent()==c);

	c.removeChild(*e);

	System::removeModule(c);
}
	
TEST_CASE("Testing the ENABLE property of an entity","[entity][system]") {
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();

	System::addModule(c);

	c.addChild(*e);

	REQUIRE(!e->isUpdated);

	c.update();

	REQUIRE(e->isUpdated);
	e->isUpdated = false;
	e->setProperty(ENTITY_PROPERTY_ENABLED,false);

	c.update();

	REQUIRE(!e->isUpdated);

	c.removeChild(*e);

	System::removeModule(c);
}

TEST_CASE("Testing death","[entity][system]") {//the sweet embrace of death
	
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();

	System::addModule(c);//c++ or c? that is the question.
	
	c.addChild(*e);

	REQUIRE(c.hasChild(*e));
	REQUIRE(!e->getProperty(ENTITY_PROPERTY_DEAD));

	c.update();

	REQUIRE(!e->getProperty(ENTITY_PROPERTY_DEAD));
	REQUIRE(e->isUpdated);

	
	e->isUpdated = false;
	e->setProperty(ENTITY_PROPERTY_DEAD,true);

	c.update();

	REQUIRE(!e->isUpdated);
	REQUIRE(e->isDestroyed);
	REQUIRE(!c.hasChild(*e));	
	
	System::removeModule(c);
}

TEST_CASE("Testing init()","[entity][system") {
	EntityModule c = EntityModule();

	DummyEntity* e = new DummyEntity();

	System::addModule(c);

	REQUIRE(!e->getProperty(ENTITY_PROPERTY_INIT));

	c.addChild(*e);

	c.init();

	REQUIRE(e->isInit);
	REQUIRE(e->getProperty(ENTITY_PROPERTY_INIT));

	c.removeChild(*e);

	System::removeModule(c);
}

TEST_CASE("Testing entity dirtiness","[entity][system]") {
	EntityModule c = EntityModule();
	
	DummyEntity* e = new DummyEntity();

	System::addModule(c);

	c.addChild(*e);

	REQUIRE(e->getProperty(ENTITY_PROPERTY_DIRTY));

	c.update();

	REQUIRE(!e->getProperty(ENTITY_PROPERTY_DIRTY));

	System::removeModule(c);
}

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

	EntityModule *c = new EntityModule();

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

	c->removeChild(*e);

	System::removeModule(*c);

	delete c;
}
}