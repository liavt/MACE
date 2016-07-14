#include <Catch.h>
#include <iostream>
#include <MC-Graphics/Entity.h>
#include <MC-Graphics/Graphics.h>

class DummyEntity : public mc::gfx::Entity {
public:
	DummyEntity(): Entity(){};


	bool isUpdated = false,  isInit = false, isDestroyed = false;
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
	namespace gfx{

		EntityModule c = EntityModule();


		TEST_CASE("Testing the getParent() function","[entity][system]") {

			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Checking whether it recognizes the parent"){

				REQUIRE(e.getParent()==c);
			}

			c.clearChildren();

			;

		}
	
		TEST_CASE("Testing the ENABLE property of an entity", "[entity][system]") {

			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Updating an entity"){
				e.isUpdated = false;
				e.setProperty(ENTITY_PROPERTY_UPDATE_ENABLED, false);

				c.update();

				REQUIRE(!e.isUpdated);
			}

			c.clearChildren();

			;


		}

		TEST_CASE("Testing death","[entity][system]") {//the sweet embrace of death
	

			DummyEntity e = DummyEntity();
	
			c.addChild(e);

			SECTION("Killing some entities"){
				REQUIRE(c.hasChild(e));
				REQUIRE(!e.getProperty(ENTITY_PROPERTY_DEAD));

				c.update();

				REQUIRE(!e.getProperty(ENTITY_PROPERTY_DEAD));
				REQUIRE(e.isUpdated);

	
				e.isUpdated = false;
				e.setProperty(ENTITY_PROPERTY_DEAD,true);

				c.update();

				REQUIRE(!e.isUpdated);
				REQUIRE(e.isDestroyed);
				REQUIRE(!c.hasChild(e));	
			}

			c.clearChildren();

	
		}

		TEST_CASE("Testing init()","[entity][system") {

			DummyEntity e = DummyEntity();
			c.addChild(e);



			SECTION("Calling init() on entities"){


				REQUIRE(!e.getProperty(ENTITY_PROPERTY_INIT));


				c.init();

				REQUIRE(e.isInit);
				REQUIRE(e.getProperty(ENTITY_PROPERTY_INIT));


			}
			c.clearChildren();

			;

		}

		TEST_CASE("Testing entity dirtiness","[entity][system]") {
	
			DummyEntity e= DummyEntity();

			c.addChild(e);

			SECTION("Seeing if dirty gets updated"){
		
				REQUIRE(e.getProperty(ENTITY_PROPERTY_DIRTY));
		
				c.update();
		
				REQUIRE(!e.getProperty(ENTITY_PROPERTY_DIRTY));
			}
	
			c.clearChildren();

			;

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
			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Testing init(), destroy(), and destroy()") {

				REQUIRE(e.isUpdated == false);

				c.init();

				REQUIRE(e.isInit);

				c.update();

				REQUIRE(e.isUpdated);

				c.destroy();

				REQUIRE(e.isDestroyed);

			}

			DummyEntity child = DummyEntity();

			SECTION("Testing init() destroy() and update() with children"){

				e.addChild(child);

				c.init();

				REQUIRE(child.isInit);

				c.update();

				REQUIRE(child.isUpdated);

				c.destroy();

				REQUIRE(child.isDestroyed);

			}

			c.clearChildren();
		}
	}
}