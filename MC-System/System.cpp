#include <MC-System/System.h>
#include <SDL/SDL.h>
#include <MC-System/Exceptions.h>
#include <MC-System/Constants.h>

namespace mc {
	std::vector<Module*> System::modules;

	int System::addModule(Module& m)
	{
		modules.push_back(&m);
		return (int)(modules.size() - 1);
	}
	void System::removeModule(Module& m)
	{
		removeModule(m.getName());
	}
	void System::removeModule(std::string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++) {
			if (modules[i]->getName()==module) {
				removeModule(i);
				return;
			}
		}
	}
	void System::removeModule(int i)
	{
		modules.erase(modules.begin()+i);
	}
	Module * System::getModule(std::string keyword)
	{
		for (unsigned int i = 0; i < modules.size();i++) {
			if (modules[i]->getName() == keyword) {
				return modules[i];
			}
		}
		return nullptr;
	}
	Module * System::getModule(int i)
	{
		return modules[i];
	}
	bool System::moduleExists(std::string module)
	{
		for (unsigned int i = 0; i < modules.size();i++) {
			if (modules[i]->getName() == module) {
				return true;
			}
		}
		return false;
	}
	bool System::moduleExists(Module * module)
	{
		return moduleExists(module->getName());
	}
	Size System::numberOfModules()
	{
		return modules.size();
	}
	void System::assertModule(std::string module, std::string errorMessage)
	{
		if (!moduleExists(module)) {
			throw DependencyNotFound(errorMessage);
		}
	}
	void System::assertModule(std::string module)
	{
		assertModule(module, "\'"+module+"\' module has not been registered!");
	}
	void System::init() {
		SDL_Init(SDL_INIT_EVERYTHING);
		for (unsigned int i = 0; i < modules.size(); i++) {
			modules[i]->init();
		}
	}

	void System::terminate() {
		for (unsigned int i = 0; i < modules.size(); i++) {
			modules[i]->destroy();
		}
		SDL_Quit();
	}

	void System::update() {
		for (unsigned int i = 0; i < modules.size(); i++) {
			modules[i]->update();
		}
	}

	void System::tick() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
				SDL_SetWindowData(SDL_GetWindowFromID(e.window.windowID), "open", 0);
			}
		}
		for (unsigned int i = 0; i < modules.size(); i++) {
			modules[i]->tick();
		}
	}
}