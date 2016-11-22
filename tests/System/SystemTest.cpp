/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.h>
#include <MACE/System/System.h>

namespace mc {
	class TestModule:public mc::Module {
	public:
		TestModule() : mc::Module() {};

		int updates = 0;
		bool isInit = false;


		void init() {
			isInit = true;
		};

		void update() {
			updates++;
		}
		void destroy() {
			isInit = false;
		};
		std::string getName() const {
			return "Test";
		};
	};

	TEST_CASE("Testing reset() and numberOfModules()") {
		System::reset();


		REQUIRE(System::numberOfModules() == 0);

		TestModule m = TestModule();
		TestModule m1 = TestModule();
		TestModule m2 = TestModule();

		System::addModule(m);

		SECTION("Testing with 1 module") {
			REQUIRE(System::moduleExists(&m));
			REQUIRE_FALSE(System::moduleExists(&m1));
			REQUIRE_FALSE(System::moduleExists(&m2));
			REQUIRE(System::numberOfModules() == 1);
		}

		System::addModule(m1);

		SECTION("Testing with 2 modules") {
			REQUIRE(System::moduleExists(&m));
			REQUIRE(System::moduleExists(&m1));
			REQUIRE_FALSE(System::moduleExists(&m2));
			REQUIRE(System::numberOfModules() == 2);
		}

		System::addModule(m2);

		SECTION("Testing with 3 modules") {
			REQUIRE(System::moduleExists(&m));
			REQUIRE(System::moduleExists(&m1));
			REQUIRE(System::moduleExists(&m2));
			REQUIRE(System::numberOfModules() == 3);
		}

		System::reset();

		SECTION("Testing reset()") {
			REQUIRE(System::numberOfModules() == 0);
			REQUIRE_FALSE(System::moduleExists(&m));
			REQUIRE_FALSE(System::moduleExists(&m1));
			REQUIRE_FALSE(System::moduleExists(&m2));

			//testing flags. 8 because it is a byte.
			for( Index i = 0; i < 8; i++ ) {
				REQUIRE_FALSE(System::getFlag(i));
			}
		}

	}

	TEST_CASE("Testing modules", "[module][system]") {
		TestModule m = TestModule();
		TestModule m2 = TestModule();

		System::reset();

		mc::System::addModule(m2);

		SECTION("Testing moduleExists()") {
			REQUIRE(System::moduleExists(&m2));
			REQUIRE_FALSE(System::moduleExists(&m));
		}

		mc::System::addModule(m);

		SECTION("Testing init() and update() with modules") {
			REQUIRE(mc::System::moduleExists(m.getName()));
			REQUIRE(mc::System::moduleExists(&m));
			REQUIRE(System::moduleExists(&m2));
			REQUIRE(System::moduleExists(m2.getName()));

			mc::System::init();

			REQUIRE(mc::System::moduleExists(m2.getName()));
			REQUIRE(mc::System::moduleExists(&m2));
			mc::System::update();
		}

		mc::System::removeModule(m);

		SECTION("Testing moduleExists()") {
			REQUIRE_FALSE(mc::System::moduleExists(&m));
			REQUIRE(mc::System::moduleExists(&m2));
		}

		mc::System::removeModule(m2);


	}

	TEST_CASE("Testing flags") {
		System::reset();

		TestModule m = TestModule();

		System::addModule(m);
		SECTION("Testing with module") {
			SECTION("Testing init() and update()") {
				System::init();

				REQUIRE(System::getFlag(SYSTEM_FLAG_INIT));
				REQUIRE_FALSE(System::getFlag(SYSTEM_FLAG_DESTROYED));
				REQUIRE_FALSE(System::getFlag(SYSTEM_FLAG_STOP_REQUESTED));


				for( Index i = 0; i < 10; i++ ) {
					REQUIRE(m.updates == i);
					mc::System::update();
					REQUIRE(System::isRunning());
					REQUIRE_FALSE(System::getFlag(SYSTEM_FLAG_STOP_REQUESTED));
				}

				System::requestStop();

				REQUIRE(System::getFlag(SYSTEM_FLAG_STOP_REQUESTED));
				REQUIRE(!System::isRunning());

				System::terminate();

				REQUIRE(System::getFlag(SYSTEM_FLAG_DESTROYED));
				REQUIRE_FALSE(System::getFlag(SYSTEM_FLAG_INIT));
				REQUIRE_FALSE(System::getFlag(SYSTEM_FLAG_STOP_REQUESTED));

			}
		}

		System::removeModule(m);
	}

	TEST_CASE("Modules getting updated", "[module][system]") {
		System::reset();

		TestModule m = TestModule();


		REQUIRE(System::numberOfModules() == 0);

		mc::System::addModule(m);

		SECTION("Testing with module") {
			SECTION("Testing init() and update()") {

				REQUIRE_FALSE(m.isInit);
				REQUIRE(m.updates == 0);

				mc::System::init();

				REQUIRE(m.isInit);
				REQUIRE(m.updates == 0);

				for( Index i = 0; i < 10; i++ ) {
					REQUIRE(m.updates == i);
					mc::System::update();
				}

				mc::System::terminate();

				REQUIRE(m.updates == 10);
				REQUIRE_FALSE(m.isInit);

			}
		}

		mc::System::removeModule(m);

	}
}