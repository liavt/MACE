#include <MC-Testing/Catch.h>
#include <MC-System/System.h>

class TestModule :public mc::Module {
public:
	TestModule() : mc::Module() {};

	int x=0;
	bool isInit = false;
	

	void init() {
		isInit = true;
	};
	void update() {
		x++;
	};
	void destroy() {
		isInit = false;
	};
	std::string getName() const{
		return "Test";
	};
};

TEST_CASE("Adding and removing modules", "[module][system]") {
	TestModule m=TestModule();

	mc::System::addModule(&m);

	REQUIRE(mc::System::moduleExists(m.getName()));
	REQUIRE(mc::System::moduleExists(&m));

	mc::System::removeModule(&m);

	REQUIRE_FALSE(mc::System::moduleExists(m.getName()));
	REQUIRE_FALSE(mc::System::moduleExists(&m));
}

TEST_CASE("Modules getting updated","[module][system]") {
	TestModule m = TestModule();

	mc::System::addModule(&m);

	REQUIRE_FALSE(m.isInit);
	REQUIRE(m.x == 0);

	mc::System::init();

	REQUIRE(m.isInit);
	REQUIRE(m.x == 0);

	for (unsigned int i = 0; i < 10; i++) {
		REQUIRE(m.x == i);
		mc::System::update();
	}

	mc::System::terminate();

	REQUIRE(m.x == 10);
	REQUIRE_FALSE(m.isInit);

	mc::System::removeModule(&m);

	mc::System::init();

	REQUIRE_FALSE(m.isInit);
	REQUIRE(m.x == 10);

	for (unsigned int i = 0; i < 10; i++) {
		REQUIRE(m.x == 10);
		mc::System::update();
	}

	mc::System::terminate();

}