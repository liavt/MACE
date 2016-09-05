#pragma once
#include <MACE/System/Constants.h>
#include <string>

namespace mc {
	namespace gfx {
		void checkGLError();
		void throwShaderError(const Index& shaderId, const Enum& type, const std::string& message);
		void throwShaderError(const Index& shaderId, const Enum& type);
	}
}