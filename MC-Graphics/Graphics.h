#pragma once

#include <MC-System/System.h>

namespace mc {
	class GraphicsModule : public Module{
	public:
		void init();
		void update();
		void destroy();
		char * getName();
	};
}
