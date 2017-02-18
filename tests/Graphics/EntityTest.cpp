/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Entity2D.h>

namespace mc {
	namespace gfx {

		class DummyEntity: public mc::gfx::Entity {
		public:
			DummyEntity() : Entity() {};

			using Entity::init;
			using Entity::update;
			using Entity::render;
			using Entity::destroy;

			bool isUpdated = false, isInit = false, isDestroyed = false, isRendered = false, isCleaned = false;
		protected:

			virtual void onUpdate() override {
				isUpdated = true;
			}

			virtual void onInit() override {
				isInit = true;
			}

			virtual void onDestroy() override {
				isDestroyed = true;
			}

			virtual void onRender() override {
				isRendered = true;
			}

			virtual void onClean() override {
				isCleaned = true;
			}
		};

		class DummyEntity2D: public mc::gfx::Entity2D {

		public:
			DummyEntity2D() : mc::gfx::Entity2D() {};


			bool isUpdated = false, isInit = false, isDestroyed = false, isRendered = false, isCleaned = false;

			using Entity2D::init;
			using Entity2D::update;
			using Entity2D::render;
			using Entity2D::destroy;

		protected:

			virtual void onUpdate() override {
				isUpdated = true;
			}

			virtual void onInit() override {
				isInit = true;
			}

			virtual void onDestroy() override {
				isDestroyed = true;
			}

			virtual void onRender() override {
				isRendered = true;
			}

			virtual void onClean() override{
				isCleaned = true;
			}
		};

		class DummyGroup: public mc::gfx::Group {
		public:
			using Entity::init;
			using Entity::update;
			using Entity::render;
			using Entity::destroy;
		};

		class DummyComponent: public mc::gfx::Component {
		public:
			int updates;
			bool destroySelf = false;
			bool isInit = false;
			bool destroyed = false;

			void init(Entity* e) override {
				isInit = true;
			}

			bool update(Entity* e) override {
				updates++;
				if( destroySelf )return true;
				else return false;
			}

			void destroy(Entity* e) override {
				destroyed = true;
			}

			void render(Entity* e) override {
			
			}
		};

		DummyGroup c = DummyGroup();

		TEST_CASE("Testing dirtiness") {
			DummyEntity e = DummyEntity();
			DummyEntity2D e2 = DummyEntity2D();

			SECTION("Testing what makes something dirty") {
				REQUIRE_FALSE(e.getProperty(Entity::DIRTY));
				e.setProperty(Entity::MAINTAIN_X, true);
				REQUIRE(e.getProperty(Entity::DIRTY));
				e.reset();
				REQUIRE_FALSE(e.getProperty(Entity::DIRTY));
			}

			SECTION("Testing clean()") {
				e.reset();
				REQUIRE_FALSE(e.getProperty(Entity::DIRTY));
				REQUIRE_FALSE(e.isCleaned);

				e.setProperty(Entity::DIRTY, true);
				REQUIRE(e.getProperty(Entity::DIRTY));
				REQUIRE_FALSE(e.isCleaned);

				e.update();
				e.render();

				REQUIRE_FALSE(e.getProperty(Entity::DIRTY));
				REQUIRE(e.isCleaned);
			}
		}

		TEST_CASE("Testing the getParent() function", "[entity][graphics]") {

			DummyEntity e = DummyEntity();
			DummyEntity e2 = DummyEntity();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE_FALSE(e2.hasParent());
			REQUIRE(c.isEmpty());

			c.addChild(e);

			REQUIRE(!c.isEmpty());

			SECTION("hasParent()") {
				REQUIRE(e.hasParent());
				REQUIRE_FALSE(e2.hasParent());
			}

			e.addChild(e2);

			SECTION("Checking whether it recognizes the parent") {
				REQUIRE(e.hasParent());
				REQUIRE(*e2.getParent() == e);
			}

			c.reset();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE(e2.hasParent());
			REQUIRE(c.isEmpty());
		}

		TEST_CASE("Testing getRoot()") {
			REQUIRE(c.getRoot() == &c);
			REQUIRE_FALSE(c.hasParent());

			DummyEntity e = DummyEntity();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE(e.getRoot() == &e);

			c.addChild(e);

			REQUIRE(c.getRoot() == &c);
			REQUIRE(e.hasParent());
			REQUIRE(e.getRoot() == &c);
			
			DummyEntity e1 = DummyEntity();

			REQUIRE(e1.getRoot() == &e1);

			c.addChild(e1);

			REQUIRE(c.getRoot() == &c);
			REQUIRE(e.getRoot() == &c);
			REQUIRE(e1.getRoot() == &c);

			c.reset();
			
		}

		TEST_CASE("Testing the ENABLE property of an entity", "[entity][graphics]") {

			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Updating an entity") {
				REQUIRE(!e.getProperty(Entity::DISABLED));
				e.isUpdated = false;
				e.setProperty(Entity::DISABLED, true);

				c.update();

				REQUIRE(!e.isUpdated);
			}

			SECTION("Rendering an entity") {
				REQUIRE(!e.getProperty(Entity::DISABLED));
				e.isRendered = false;
				e.setProperty(Entity::DISABLED, true);

				c.render();

				REQUIRE(!e.isRendered);
			}

			c.reset();
		}

		TEST_CASE("Testing death", "[entity][graphics]") {//the sweet embrace of death


			DummyEntity e = DummyEntity();

			c.addChild(e);

			SECTION("Killing some entities") {//RIP

				c.init();

				REQUIRE(c.hasChild(e));
				REQUIRE(!e.getProperty(Entity::DEAD));

				c.update();

				REQUIRE(!e.getProperty(Entity::DEAD));
				REQUIRE(e.isUpdated);


				e.isUpdated = false;
				e.setProperty(Entity::DEAD, true);

				c.update();

				REQUIRE(!e.isUpdated);
				REQUIRE(e.isDestroyed);
				REQUIRE(!c.hasChild(e));
			}

			c.reset();
		}

		TEST_CASE("Testing actions", "[entity][graphics]") {
			DummyComponent a = DummyComponent();
			DummyEntity e = DummyEntity();

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

			c.reset();
		}

		TEST_CASE("Testing init()", "[entity][graphics]") {
			DummyEntity e = DummyEntity();
			c.addChild(e);



			SECTION("Calling init() on entities") {


				REQUIRE(!e.getProperty(Entity::INIT));


				c.init();

				REQUIRE(e.isInit);
				REQUIRE(e.getProperty(Entity::INIT));


			}

			c.reset();
		}

		TEST_CASE("Testing entity properties", "[entity][graphics]") {
			DummyEntity e = DummyEntity();

			REQUIRE_FALSE(e.getProperty(Entity::DEAD));
			e.setProperty(Entity::DEAD, true);

			REQUIRE(e.getProperty(Entity::DEAD));

			REQUIRE(!e.getProperty(Entity::DISABLED));
			e.setProperty(Entity::DISABLED, true);
			REQUIRE(e.getProperty(Entity::DISABLED));
			REQUIRE(e.getProperty(Entity::DEAD));
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

			c.reset();

			DummyEntity child = DummyEntity();

			SECTION("Testing init() destroy() render() and update() with children") {

				e.addChild(child);
				c.addChild(e);

				c.init();

				REQUIRE(child.isInit);

				c.update();

				REQUIRE(child.isUpdated);

				c.render();

				REQUIRE(child.isRendered);

				c.destroy();

				REQUIRE(child.isDestroyed);

			}

			c.reset();
		}
	}
}