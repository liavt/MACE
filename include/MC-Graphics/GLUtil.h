#pragma once
#include <MC-System/Constants.h>
#include <GL/glew.h>
#include <string>

namespace mc {
	namespace gfx {
		void checkGLError();
		void throwShaderError(const Index& shaderId, const GLenum& type, const std::string& message);
		void throwShaderError(const Index& shaderId, const GLenum& type);
	}
}