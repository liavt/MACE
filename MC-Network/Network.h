#pragma once
#include <MC-System/System.h>
namespace mc{
	class NetworkModule : public Module {
	public:
		void init();
		void update();
		void destroy();
		std::string getName() const;
	};
}