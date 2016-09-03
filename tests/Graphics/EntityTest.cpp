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

namespace mc{
	namespace gfx{

		class DummyEntity : public mc::gfx::Entity {
		public:
			DummyEntity() : Entity() {};


			bool isUpdated = false, isInit = false, isDestroyed = false, isRendered = false,isInherited=false;
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

			virtual void inherit(const Entity& e) {
				Entity::inherit(e);

				isInherited = true;
			}
		};

		class DummyEntity2D :  public mc::gfx::Entity2D {

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

		TEST_CASE("Testing the getParent() function","[entity][graphics]") {

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
				REQUIRE(*e2.getParent()==e);
			}

			c.clearChildren();

			REQUIRE_FALSE(e.hasParent());
			REQUIRE(e2.hasParent());

		}
	
		TEST_CASE("Testing the ENABLE property of an entity", "[entity][graphics]") {

			DummyEntity e = DummyEntity();


			c.addChild(e);

			SECTION("Updating an entity"){
				REQUIRE(e.getProperty(ENTITY_UPDATE_ENABLED));
				e.isUpdated = false;
				e.setProperty(ENTITY_UPDATE_ENABLED, false);

				c.updateChildren();

				REQUIRE(!e.isUpdated);
			}

			SECTION("Rendering an entity") {
				REQUIRE(e.getProperty(ENTITY_RENDER_ENABLED));
				e.isRendered = false;
				e.setProperty(ENTITY_RENDER_ENABLED, false);

				c.renderChildren();

				REQUIRE(!e.isRendered);
			}

			c.clearChildren();


		}

		TEST_CASE("Testing death","[entity][graphics]") {//the sweet embrace of death
	

			DummyEntity e = DummyEntity();
	
			c.addChild(e);

			SECTION("Killing some entities"){//RIP
				REQUIRE(c.hasChild(e));
				REQUIRE(!e.getProperty(ENTITY_DEAD));

				c.updateChildren();

				REQUIRE(!e.getProperty(ENTITY_DEAD));
				REQUIRE(e.isUpdated);

	
				e.isUpdated = false;
				e.setProperty(ENTITY_DEAD,true);

				c.updateChildren();

				REQUIRE(!e.isUpdated);
				REQUIRE(e.isDestroyed);
				REQUIRE(!c.hasChild(e));	
			}

			c.clearChildren();

	
		}

		TEST_CASE("Testing actions","[entity][graphics]") {
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

		TEST_CASE("Testing init()","[entity][graphics]") {

			DummyEntity e = DummyEntity();
			c.addChild(e);



			SECTION("Calling init() on entities"){


				REQUIRE(!e.getProperty(ENTITY_INIT));


				c.initChildren();

				REQUIRE(e.isInit);
				REQUIRE(e.getProperty(ENTITY_INIT));


			}
			c.clearChildren();

		}

		TEST_CASE("Testing inheritence","[entity][graphics]") {
			DummyEntity parent = DummyEntity(),child=DummyEntity();
			DummyEntity2D parent2d = DummyEntity2D(), child2d = DummyEntity2D();
			c.addChild(parent);
			c.addChild(parent2d);

			parent.addChild(child);
			parent2d.addChild(child2d);

			SECTION("Testing inherit()") {
				REQUIRE_FALSE(child.getProperty(ENTITY_IGNORE_PARENT));
				REQUIRE(parent.getProperty(ENTITY_PASS_DOWN));
				REQUIRE_FALSE(child.isInherited);
				REQUIRE_FALSE(parent.isInherited);

				c.updateChildren();

				REQUIRE(child.isInherited);
				//top level entitys (one whose parent is a Container) dont get inheritence
				REQUIRE_FALSE(parent.isInherited);

				SECTION("Testing pass down and ignore parent") {
					SECTION("Testing PASS_DOWN") {
						child.isInherited = false;

						REQUIRE(parent.getProperty(ENTITY_PASS_DOWN));
						parent.setProperty(ENTITY_PASS_DOWN,false);
						REQUIRE_FALSE(parent.getProperty(ENTITY_PASS_DOWN));
						
						c.updateChildren();

						//child should not have inherited from parent, as pass_down was false
						REQUIRE_FALSE(child.isInherited);
						REQUIRE_FALSE(parent.isInherited);
					}
					child.isInherited = false;
					parent.setProperty(ENTITY_PASS_DOWN, true);
					SECTION("Testing IGNORE_PARENT") {
						REQUIRE_FALSE(child.isInherited);

						REQUIRE_FALSE(child.getProperty(ENTITY_IGNORE_PARENT));
						child.setProperty(ENTITY_IGNORE_PARENT, true);
						REQUIRE(child.getProperty(ENTITY_IGNORE_PARENT));

						c.updateChildren();

						REQUIRE_FALSE(child.isInherited);
						REQUIRE_FALSE(parent.isInherited);
					}
					child.isInherited = false;
					parent.setProperty(ENTITY_PASS_DOWN, true);
					child.setProperty(ENTITY_IGNORE_PARENT, false);
				}

				//instead of repeating this code for Entity2D, i just change the variable pointer
				SECTION("Testing STRETCH_X and STRETCH_Y") {
					SECTION("Testing STRETCH_X") {
						REQUIRE_FALSE(child.getProperty(ENTITY_STRETCH_X));
						REQUIRE(child.getProperty(ENTITY_INHERIT_STRETCH_X));

						c.updateChildren();

						REQUIRE_FALSE(child.getProperty(ENTITY_STRETCH_X));
						REQUIRE(child.getProperty(ENTITY_INHERIT_STRETCH_X));

						parent.setProperty(ENTITY_STRETCH_X, true);

						c.updateChildren();

						//should have inherited it from the parent, so it should now be true
						REQUIRE(child.getProperty(ENTITY_STRETCH_X));
						REQUIRE(child.getProperty(ENTITY_INHERIT_STRETCH_X));

						child.setProperty(ENTITY_INHERIT_STRETCH_X, false);

						//invert it
						parent.setProperty(ENTITY_STRETCH_X, true);

						c.updateChildren();

						//the child should not have inherited the change, so ENTITY_STRETCH_X should stay true
						REQUIRE(child.getProperty(ENTITY_STRETCH_X));
						REQUIRE_FALSE(child.getProperty(ENTITY_INHERIT_STRETCH_X));

					}
					SECTION("Testing STRETCH_Y") {
						REQUIRE_FALSE(child.getProperty(ENTITY_STRETCH_Y));
						REQUIRE(child.getProperty(ENTITY_INHERIT_STRETCH_Y));

						c.updateChildren();

						REQUIRE_FALSE(child.getProperty(ENTITY_STRETCH_Y));
						REQUIRE(child.getProperty(ENTITY_INHERIT_STRETCH_Y));

						parent.setProperty(ENTITY_STRETCH_Y, true);

						c.updateChildren();

						//should have inherited it from the parent, so it should now be true
						REQUIRE(child.getProperty(ENTITY_STRETCH_Y));
						REQUIRE(child.getProperty(ENTITY_INHERIT_STRETCH_Y));

						child.setProperty(ENTITY_INHERIT_STRETCH_Y, false);

						//invert it
						parent.setProperty(ENTITY_STRETCH_Y, true);

						c.updateChildren();

						//the child should not have inherited the change, so ENTITY_STRETCH_Y should stay true
						REQUIRE(child.getProperty(ENTITY_STRETCH_Y));
						REQUIRE_FALSE(child.getProperty(ENTITY_INHERIT_STRETCH_Y));

					}
				}//end of STRETCH_X and STRETCH_Y test

			}

			c.clearChildren();
		}

		TEST_CASE("Testing entity properties","[entity][graphics]") {
			DummyEntity e = DummyEntity();

			REQUIRE_FALSE(e.getProperty(ENTITY_DEAD));
			e.setProperty(ENTITY_DEAD, true);

			REQUIRE(e.getProperty(ENTITY_DEAD));

			REQUIRE(e.getProperty(ENTITY_UPDATE_ENABLED));
			e.setProperty(ENTITY_UPDATE_ENABLED, false);
			REQUIRE_FALSE(e.getProperty(ENTITY_UPDATE_ENABLED));
			REQUIRE(e.getProperty(ENTITY_DEAD));
		}


		TEST_CASE("Testing entity module and updating", "[entity][graphics]") {
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