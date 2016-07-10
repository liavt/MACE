#pragma once
#include <vector>
#include <MC-System/Constants.h>

namespace mc {
	class System;

	class Module {
		friend class System;
	protected: 
		virtual void init()=0;
		virtual void update()=0;
		virtual void destroy()=0;
	
		virtual std::string getName() const =0;
	};

	class System final {
	public:
		static int addModule(Module& m);
		static void removeModule(Module& m);
		static void removeModule(std::string module);
		static void removeModule(int i );
		static Module* getModule(std::string keyword) ;
		static Module* getModule(int i);
		static bool moduleExists(std::string module);
		static bool moduleExists(Module* module);
		static Size numberOfModules();

		static void assertModule(std::string module,std::string errorMessage);
		static void assertModule(std::string module);

		static void init();
		static void terminate();
		static void update();
	private: 
		System();

		static std::vector<Module*> modules;
	};

}


