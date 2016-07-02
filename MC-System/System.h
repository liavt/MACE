#pragma once
#include <vector>

namespace mc {
	class Module {
	public: 
		virtual void init()=0;
		virtual void update()=0;
		virtual void destroy()=0;
	
		virtual char* getName()=0;
	};

	class System final {
	public:
		static int addModule(Module* m);
		static void removeModule(Module* m);
		static void removeModule(std::string module);
		static void removeModule(int i );
		static Module* getModule(std::string keyword) ;
		static Module* getModule(int i);


		static void init();
		static void terminate();
		static void update();
	private: 
		System();

		static std::vector<Module*> modules;
	};

}


