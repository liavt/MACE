#pragma once
#include <vector>
#include <string>


namespace mc {
	class Module {
	public: 
		virtual void init();
		virtual void update();
		virtual void destroy();
	
		std::string getName();

	protected:
		std::string name;
	};

	class System final {
	public:
		static int addModule(const Module m);
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

		static std::vector<Module> modules;
	};

}


