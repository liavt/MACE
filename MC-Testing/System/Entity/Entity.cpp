#include <MC-Testing/Catch.h>
#include <MC-System/Entity.h>

class DummyEntity : public mc::Entity {
public:
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
	REQUIRE(mc::System::numberOfModules()==0);

	EntityModule* c =new EntityModule();
	
	System::addModule(*c);

	DummyEntity e = DummyEntity();

	REQUIRE(c->size() == 0);
	REQUIRE(e.isUpdated == false);

	c->addChild(&e);


	REQUIRE(c->size() == 1);

	c->update();

	REQUIRE(e.isUpdated);

	DummyEntity child = DummyEntity();
	e.addChild(&child);

	c->update();

	REQUIRE(child.isUpdated);
}
}