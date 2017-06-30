/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#ifndef MACE__GRAPHICS_GRAPHICS_H
#define MACE__GRAPHICS_GRAPHICS_H

namespace mc {
	/**
	Namespace containing classes used for Graphics in MACE.
	*/
	namespace gfx {
	}
}

//the following two headers should not be included by default as they are for implementation of the
//OpenGL renderer and are useless to the end user unless they are using the OpenGL abstraction
#ifdef MACE_EXPOSE_OPENGL
#	include <MACE/Graphics/OGL/OGL.h>
#	include <MACE/Graphics/OGL/GLRenderer.h>
#endif

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Components.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Window.h>

#endif