#include <MC-System/System.h>
#include <SDL/SDL.h>
#include <MC-System/Exceptions.h>
#include <MC-System/Constants.h>

namespace mc {
	std::vector<Module*> System::modules;

	unsigned int System::addModule(Module& m)
	{
		modules.push_back(&m);
		return (unsigned int)(modules.size() - 1);
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
		throw ObjectNotFoundInArray("No module by the name of "+module+" found!");
	}
	void System::removeModule(unsigned int i)
	{
		if (i<0 || i>numberOfModules())throw IndexOutOfBounds("Input is not a valid index!");
		modules.erase(modules.begin()+i);
	}
	Module * System::getModule(std::string keyword)
	{
		for (unsigned int i = 0; i < modules.size();i++) {
			if (modules[i]->getName() == keyword) {
				return modules[i];
			}
		}
		throw ObjectNotFoundInArray("No module by the name of " + keyword + " found!");
	}
	Module * System::getModule(unsigned int i)
	{
		if (i<0 || i>numberOfModules())throw IndexOutOfBounds("Input is not a valid index!");
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
	bool System::moduleExists(const Module * module)
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

	unsigned int System::indexOf(const Module& m) {
		return indexOf(m.getName());
	}

	unsigned int System::indexOf(std::string name) {
		for (unsigned int i = 0; i < modules.size(); i++) {
			if (modules[i]->getName() ==name) {
				return i;
			}
		}
		return -1;
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
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
				SDL_SetWindowData(SDL_GetWindowFromID(e.window.windowID), "open", 0);
			}
		}
		for (unsigned int i = 0; i < modules.size(); i++) {
			modules[i]->update();
		}
	}
}