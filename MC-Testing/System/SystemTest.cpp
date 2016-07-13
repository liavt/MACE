#include <MC-Testing/Catch.h>
#include <MC-System/System.h>

class TestModule :public mc::Module {
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
	std::string getName() const{
		return "Test";
	};
};

TEST_CASE("Adding and removing modules", "[module][system]") {
	TestModule* m= new TestModule();
	TestModule* m2 = new TestModule();


	mc::System::addModule(*m);

	REQUIRE(mc::System::moduleExists(m->getName()));
	REQUIRE(mc::System::moduleExists(m));

	mc::System::addModule(*m2);

	REQUIRE(mc::System::moduleExists(m2->getName()));
	REQUIRE(mc::System::moduleExists(m2));
	mc::System::update();

	mc::System::removeModule(*m);

	REQUIRE(mc::System::moduleExists(m2));

	mc::System::removeModule(*m2);

	REQUIRE(!mc::System::moduleExists(m2));

	delete m;

}

TEST_CASE("Modules getting updated","[module][system]") {
	TestModule m = TestModule();

	mc::System::addModule(m);

	REQUIRE_FALSE(m.isInit);
	REQUIRE(m.updates == 0);

	mc::System::init();

	REQUIRE(m.isInit);
	REQUIRE(m.updates == 0);

	for (unsigned int i = 0; i < 10; i++) {
		REQUIRE(m.updates == i);
		mc::System::update();
	}

	mc::System::terminate();

	REQUIRE(m.updates == 10);
	REQUIRE_FALSE(m.isInit);

	mc::System::removeModule(m);

	mc::System::init();

	REQUIRE_FALSE(m.isInit);
	REQUIRE(m.updates == 10);

	for (unsigned int i = 0; i < 10; i++) {
		REQUIRE(m.updates == 10);
		mc::System::update();
	}

	mc::System::terminate();

}