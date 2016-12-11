/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_GRAPHICSCONSTANTS_H
#define MACE_GRAPHICS_GRAPHICSCONSTANTS_H
#include <MACE/System/Constants.h>
#include <string>


namespace mc {
	namespace gfx {

		using GLError = Exception;

		using ShaderError = Exception;

		/**
		@opengl
		*/
		void checkGLError();
		/**
		@opengl
		*/
		void throwShaderError[[noreturn]](const Index& shaderId, const Enum& type, const std::string& message);
		/**
		@opengl
		*/
		void throwShaderError[[noreturn]](const Index& shaderId, const Enum& type);
	}
}

#endif