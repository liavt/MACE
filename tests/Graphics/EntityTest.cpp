/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.h>
#include <iostream>
#include <MACE/Graphics/Entity.h>
#include <type_traits>
#include <MACE/Graphics/Entity2D.h>

namespace mc {
	namespace gfx {

		class DummyEntity: public mc::gfx::Entity {
		public:
			DummyEntity() : Entity() {};


			bool isUpdated = false, isInit = false, isDestroyed = false, isRendered = false;
		protected:

			virtual void customUpdate() {
				isUpdated = true;
			}

			virtual void customInit() {
				isInit = true;
			}

			virtual void customDestroy() {
				isDestroyed = true;
			}

			virtual void customRender() {
				isRendered = true;
			}
		};

		class DummyEntity2D: public mc::gfx::Entity2D {

		public:
			DummyEntity2D() : mc::gfx::Entity2D() {};


			bool isUpdated = false, isInit = false, isDestroyed = false, isRendered = false;
		protected:

			virtual void customUpdate() {
				isUpdated = true;
			}

			virtual void customInit() {
				isInit = true;
			}

			virtual void customDestroy() {
				isDestroyed = true;
			}

			virtual void customRender() {
				isRendered = true;
			}
		};

		class DummyAction: public mc::gfx::Component {
		public:
			int updates;
			bool destroySelf = false;
			bool isInit = false;
			bool destroyed = false;

			void init(Entity* e) {
				isInit = true;
			}

			bool update(Entity* e) {
				updates++;
				if( destroySelf )return true;
				else return false;
			}

			void destroy(Entity* e) {
				destroyed = true;
			}
		};

		Group c = Group();

		TEST_CASE("Testing the getParent() function", "[entity][graphics]") {

			DummyEntity e = DummyEntity();
			DummyEntity e2 = DummyEntity();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE_FALSE(e2.hasParent());

			c.addChild(e);

			SECTION("hasParent()") {
				REQUIRE(e.hasParent());
				REQUIRE_FALSE(e2.hasParent());
			}

			e.addChild(e2);

			SECTION("Checking whether it recognizes the parent") {
				REQUIRE(e.hasParent());
				REQUIRE(*e2.getParent() == e);
			}

			c.clearChildren();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE(e2.hasParent());

		}

		TEST_CASE("Testing the ENABLE property of an entity", "[entity][graphics]") {

			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Updating an entity") {
				REQUIRE(!e.getProperty(ENTITY_UPDATE_DISABLED));
				e.isUpdated = false;
				e.setProperty(ENTITY_UPDATE_DISABLED, true);

				c.update();

				REQUIRE(!e.isUpdated);
			}

			SECTION("Rendering an entity") {
				REQUIRE(!e.getProperty(ENTITY_RENDER_DISABLED));
				e.isRendered = false;
				e.setProperty(ENTITY_RENDER_DISABLED, true);

				c.render();

				REQUIRE(!e.isRendered);
			}

			c.clearChildren();


		}

		TEST_CASE("Testing death", "[entity][graphics]") {//the sweet embrace of death


			DummyEntity e = DummyEntity();

			c.addChild(e);

			SECTION("Killing some entities") {//RIP
				REQUIRE(c.hasChild(e));
				REQUIRE(!e.getProperty(ENTITY_DEAD));

				c.update();

				REQUIRE(!e.getProperty(ENTITY_DEAD));
				REQUIRE(e.isUpdated);


				e.isUpdated = false;
				e.setProperty(ENTITY_DEAD, true);

				c.update();

				REQUIRE(!e.isUpdated);
				REQUIRE(e.isDestroyed);
				REQUIRE(!c.hasChild(e));
			}

			c.clearChildren();


		}

		TEST_CASE("Testing actions", "[entity][graphics]") {
			DummyAction a = DummyAction();
			DummyEntity e = DummyEntity();
			Group c = Group();

			SECTION("Testing default values") {
				REQUIRE_FALSE(a.isInit);
				REQUIRE_FALSE(a.destroyed);
				REQUIRE(a.updates == 0);
				REQUIRE_FALSE(a.destroySelf);
			}

			e.addComponent(a);
			c.addChild(e);

			SECTION("Testing if it gets init") {
				REQUIRE(a.isInit);
			}

			SECTION("Testing update") {
				for( Index i = 0; i < 10; i++ ) {
					REQUIRE(a.updates == i);
					c.update();
				}
			}

			SECTION("Testing destruction") {
				a.destroySelf = true;
				c.update();
				REQUIRE(a.destroyed);
			}

			c.clearChildren();



		}

		TEST_CASE("Testing init()", "[entity][graphics]") {

			DummyEntity e = DummyEntity();
			c.addChild(e);



			SECTION("Calling init() on entities") {


				REQUIRE(!e.getProperty(ENTITY_INIT));


				c.init();

				REQUIRE(e.isInit);
				REQUIRE(e.getProperty(ENTITY_INIT));


			}
			c.clearChildren();

		}

		TEST_CASE("Testing entity properties", "[entity][graphics]") {
			DummyEntity e = DummyEntity();

			REQUIRE_FALSE(e.getProperty(ENTITY_DEAD));
			e.setProperty(ENTITY_DEAD, true);

			REQUIRE(e.getProperty(ENTITY_DEAD));

			REQUIRE(!e.getProperty(ENTITY_UPDATE_DISABLED));
			e.setProperty(ENTITY_UPDATE_DISABLED, true);
			REQUIRE(e.getProperty(ENTITY_UPDATE_DISABLED));
			REQUIRE(e.getProperty(ENTITY_DEAD));
		}


		TEST_CASE("Testing entity module and updating", "[entity][graphics]") {
			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Testing init(), destroy(), render() and destroy()") {

				REQUIRE(e.isUpdated == false);

				c.init();

				REQUIRE(e.isInit);

				c.update();

				REQUIRE(e.isUpdated);

				c.render();

				REQUIRE(e.isRendered);

				c.destroy();

				REQUIRE(e.isDestroyed);

			}

			DummyEntity child = DummyEntity();

			SECTION("Testing init() destroy() render() and update() with children") {

				e.addChild(child);

				c.init();

				REQUIRE(child.isInit);

				c.update();

				REQUIRE(child.isUpdated);

				c.render();

				REQUIRE(child.isRendered);

				c.destroy();

				REQUIRE(child.isDestroyed);

			}

			c.clearChildren();
		}
	}
}