#include <MC-Testing/Catch.h>
#include <MC-System/Entity.h>
#include <iostream>
#include <bitset>

class DummyEntity : public mc::Entity {
protected:
	void customUpdate() {
	}
};

TEST_CASE("Testing entity properties","[entity][system]") {
	DummyEntity e = DummyEntity();

	std::cout << std::bitset<8>(e.getProperties())<<std::endl;

	//REQUIRE(!e.getProperty(_MACE_ENTITY_DEAD));
	REQUIRE(!e.getProperty(_MACE_ENTITY_DEAD));
	//REQUIRE(e.getProperty(_MACE_ENTITY_DEAD)==false);
	e.setProperty(_MACE_ENTITY_DEAD, true);
	std::cout << std::bitset<8>(e.getProperties()) << std::endl;

	REQUIRE(e.getProperty(_MACE_ENTITY_DEAD));

	REQUIRE(e.getProperty(_MACE_ENTITY_ENABLED));
	e.setProperty(_MACE_ENTITY_ENABLED, false);
	REQUIRE_FALSE(e.getProperty(_MACE_ENTITY_ENABLED));
	REQUIRE(e.getProperty(_MACE_ENTITY_DEAD));
}

/*
TEST_CASE("Testing containers", "[entity][system]") {
	mc::Container c = mc::Container();
	DummyEntity e = DummyEntity();

	c.addChild(&e);

}
*/
