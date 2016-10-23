/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/GLUtil.h>
#include <memory>
#include <MACE/Graphics/GraphicsConstants.h>
#include <MACE/System/Constants.h>
#include <iostream>
#include <GL/glew.h>

namespace mc {
	namespace gfx {
		void checkGLError()
		{
#ifdef MACE_ERROR_CHECK
			GLenum result = GL_NO_ERROR;
			while ((result = glGetError()) != GL_NO_ERROR) {
				switch (result) {
				case GL_INVALID_ENUM:
					throw GLError("GL_INVALID_ENUM! An unacceptable value is specified for an enumerated argument!");
					break;
				case GL_INVALID_VALUE:
					throw GLError("GL_INVALID_VALUE! A numeric argument is out of range!");
					break;
				case GL_INVALID_OPERATION:
					throw GLError("GL_INVALID_OPERATION! The specified operation is not allowed in the current state!");
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					throw GLError("GL_INVALID_FRAMEBUFFER_OPERATION! The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE!)");
					break;
				case GL_STACK_OVERFLOW:
					throw GLError("GL_STACK_OVERFLOW! A stack pushing operation cannot be done because it would overflow the limit of that stack's size!");
					break;
				case GL_STACK_UNDERFLOW:
					throw GLError("GL_STACK_UNDERFLOW! A stack popping operation cannot be done because the stack is already at its lowest point.");
					break;
				case GL_OUT_OF_MEMORY:
					throw GLError("GL_OUT_OF_MEMORY! There is not enough memory left to execute the command!");
					break;
				default:
					throw GLError("OpenGL has errored with an error code of " + std::to_string(result));
					break;
				}
			}
#endif
		}
		void throwShaderError(const Index& shaderId, const Enum& type, const std::string& message)
		{
			if (type == 0 || type == GL_PROGRAM) {
				throw ShaderError("Error generating shader program with message \"" + message + "\"");
			}
			else {
				std::unique_ptr<GLchar[]> log_string = std::unique_ptr<GLchar[]>(new char[1024]);
				glGetShaderInfoLog(shaderId, 1024, 0, log_string.get());
				std::string friendlyType = std::to_string(type);//a more human friendly name for type, like VERTEX_SHADER instead of 335030
				if (type == GL_VERTEX_SHADER) {
					friendlyType = "VERTEX";
				}
				else if (type == GL_FRAGMENT_SHADER) {
					friendlyType = "FRAGMENT";
				}
				else if (type == GL_COMPUTE_SHADER) {
					friendlyType = "COMPUTE";
				}
				else if (type == GL_GEOMETRY_SHADER) {
					friendlyType = "GEOMETERY";
				}
				else if (type == GL_TESS_CONTROL_SHADER) {
					friendlyType = "TESSELATION CONTROL";
				}
				else if (type == GL_TESS_EVALUATION_SHADER) {
					friendlyType = "TESSELATION EVALUATION";
				}
				throw ShaderError("Error generating shader of type " + friendlyType + " with message \"" + message + "\" and GLSL error " + log_string.get());
			}
		}
		void throwShaderError(const Index & shaderId, const Enum & type)
		{
			throwShaderError(shaderId, type, "No message was specified");
		}
	}
}