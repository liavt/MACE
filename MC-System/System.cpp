#include <MC-System/System.h>
#include <SDL/SDL.h>

namespace mc {
	std::vector<Module*> System::modules;

	int System::addModule(Module* m)
	{
		modules.push_back(m);
		return (int)(modules.size() - 1);
	}
	void System::removeModule(Module * m)
	{
		removeModule(m->getName());
	}
	void System::removeModule(std::string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++) {
			if (modules[i]->getName() == module) {
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
	void System::init() {
		SDL_Init(SDL_INIT_EVERYTHING);
		for (Module* m : modules) {
			m->init();
		}
	}

	void System::terminate() {
		for (Module* m : modules) {
			m->destroy();
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
		for (Module* m : modules) {
			m->update();
		}
	}
}