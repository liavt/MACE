#pragma once

#include <MC-System/System.h>

namespace mc {
	class GraphicsModule : public Module{
	public:
		GraphicsModule();
		void init();
		void update();
		void destroy();
	};
}
