#define CATCH_CONFIG_RUNNER
#include <MC-Testing/Catch.h>

int main(int argc, char* const argv[])
{

	int result = Catch::Session().run(argc, argv);

	system("PAUSE");

	return result;
}