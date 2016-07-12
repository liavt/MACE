#pragma once

#include <MC-System/System.h>

namespace mc {
	class GraphicsModule : public Module{
	public:
		void init();
		void tick();
		void destroy();
		std::string getName() const;
	};
}
