/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.h>
#include <iostream>
#include <MC-Graphics/Entity.h>

namespace mc{
	namespace gfx{

		class DummyEntity : public mc::gfx::Entity {
		public:
			DummyEntity() : Entity() {};


			bool isUpdated = false, isInit = false, isDestroyed = false, isRendered = false;
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

			void customRender() {
				isRendered = true;
			}
		};

		class DummyAction : public mc::gfx::Action {
		public:
			int updates;
			bool destroySelf = false;
			bool isInit = false;
			bool destroyed = false;
			
			void init(Entity& e) {
				isInit = true;
			}

			bool update(Entity& e) {
				updates++;
				if (destroySelf)return true;
				else return false;
			}

			void destroy(Entity& e) {
				destroyed = true;
			}
		};

		Container c = Container();

		TEST_CASE("Testing the transformations","[entity][system]") {
			DummyEntity e = DummyEntity();
			DummyEntity e2 = DummyEntity();

			c.addChild(e);

			e.addChild(e2);

			SECTION("Base TransformMatrix") {
				e.translate(1, 1, 1);

				Vector4f v = {1,1,2,1};
				REQUIRE(e.getFinalTransformation()*v == Vector4f({2,2,3,1}));
			}

			e.getBaseTransformation().reset();

			SECTION("Inherited transformation") {
				e.translate(2,3,1);
				REQUIRE(e2.getFinalTransformation()*Vector4f({ 1,2,3,1 }) == Vector4f({3,5,4,1}));
				
				e2.translate(1, 2, 3);
				REQUIRE(e2.getFinalTransformation()*Vector4f({ 1,2,3,1 }) == Vector4f({ 4,7,7,1 }));

			}

			c.clearChildren();
		}

		TEST_CASE("Testing the getParent() function","[entity][system]") {

			DummyEntity e = DummyEntity();
			DummyEntity e2 = DummyEntity();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE_FALSE(e2.hasParent());

			c.addChild(e);

			SECTION("hasParent()") {
				REQUIRE_FALSE(e.hasParent());
				REQUIRE_FALSE(e2.hasParent());
			}

			e.addChild(e2);

			SECTION("Checking whether it recognizes the parent"){
				REQUIRE_FALSE(e.hasParent());
				REQUIRE_FALSE(e.hasParent());
				REQUIRE(e2.getParent()==e);
			}

			c.clearChildren();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE(e2.hasParent());

		}
	
		TEST_CASE("Testing the ENABLE property of an entity", "[entity][system]") {

			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Updating an entity"){
				e.isUpdated = false;
				e.setProperty(ENTITY_PROPERTY_UPDATE_ENABLED, false);

				c.updateChildren();

				REQUIRE(!e.isUpdated);
			}

			c.clearChildren();


		}

		TEST_CASE("Testing death","[entity][system]") {//the sweet embrace of death
	

			DummyEntity e = DummyEntity();
	
			c.addChild(e);

			SECTION("Killing some entities"){//RIP
				REQUIRE(c.hasChild(e));
				REQUIRE(!e.getProperty(ENTITY_PROPERTY_DEAD));

				c.updateChildren();

				REQUIRE(!e.getProperty(ENTITY_PROPERTY_DEAD));
				REQUIRE(e.isUpdated);

	
				e.isUpdated = false;
				e.setProperty(ENTITY_PROPERTY_DEAD,true);

				c.updateChildren();

				REQUIRE(!e.isUpdated);
				REQUIRE(e.isDestroyed);
				REQUIRE(!c.hasChild(e));	
			}

			c.clearChildren();

	
		}

		TEST_CASE("Testing actions","[entity][system]") {
			DummyAction a = DummyAction();
			DummyEntity e = DummyEntity();
			Container c = Container();

			SECTION("Testing default values") {
				REQUIRE_FALSE(a.isInit);
				REQUIRE_FALSE(a.destroyed);
				REQUIRE(a.updates==0);
				REQUIRE_FALSE(a.destroySelf);
			}

			e.addAction(a);
			c.addChild(e);

			SECTION("Testing if it gets init") {
				REQUIRE(a.isInit);
			}

			SECTION("Testing update") {
				for (Index i = 0; i < 10; i++) {
					REQUIRE(a.updates==i);
					c.updateChildren();
				}
			}

			SECTION("Testing destruction") {
				a.destroySelf = true;
				c.updateChildren();
				REQUIRE(a.destroyed);
			}

			c.clearChildren();



		}

		TEST_CASE("Testing init()","[entity][system]") {

			DummyEntity e = DummyEntity();
			c.addChild(e);



			SECTION("Calling init() on entities"){


				REQUIRE(!e.getProperty(ENTITY_PROPERTY_INIT));


				c.initChildren();

				REQUIRE(e.isInit);
				REQUIRE(e.getProperty(ENTITY_PROPERTY_INIT));


			}
			c.clearChildren();

		}

		TEST_CASE("Testing entity dirtiness","[entity][system]") {
	
			DummyEntity e= DummyEntity();

			c.addChild(e);

			SECTION("Seeing if dirty gets updated"){
		
				REQUIRE(e.getProperty(ENTITY_PROPERTY_DIRTY));
		
				c.updateChildren();
		
				REQUIRE(!e.getProperty(ENTITY_PROPERTY_DIRTY));
			}
	
			c.clearChildren();

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

			SECTION("Testing init(), destroy(), render() and destroy()") {

				REQUIRE(e.isUpdated == false);

				c.initChildren();

				REQUIRE(e.isInit);

				c.updateChildren();

				REQUIRE(e.isUpdated);

				c.renderChildren();

				REQUIRE(e.isRendered);

				c.destroyChildren();

				REQUIRE(e.isDestroyed);

			}

			DummyEntity child = DummyEntity();

			SECTION("Testing init() destroy() render() and update() with children"){

				e.addChild(child);

				c.initChildren();

				REQUIRE(child.isInit);

				c.updateChildren();

				REQUIRE(child.isUpdated);

				c.renderChildren();

				REQUIRE(child.isRendered);

				c.destroyChildren();

				REQUIRE(child.isDestroyed);

			}

			c.clearChildren();
		}
	}
}