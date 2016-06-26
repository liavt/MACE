#pragma once

#include <iostream>

namespace mc {
	class System {
	public:
		static void init();
		static void terminate();
		static void pollevents();
	};

}

#include <MC-System/Definitions.h>
#include <MC-System/Utility/Utils.h>

