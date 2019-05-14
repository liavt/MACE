/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once

#ifndef MACE__GRAPHICS_GRAPHICS_H
#define MACE__GRAPHICS_GRAPHICS_H

namespace mc {
	/**
	Namespace containing classes used for Graphics in MACE.
	*/
	namespace gfx {
		/**
		All enum constants used by any graphics class are stored in this namespace for organization.
		<p>
		To prevent name conflicts, it is recommended to use `using` statements on individual enums,
		instead of the entire namespace:
		{@code
			using namespace mc::gfx::Enums;//not recommended

			using mc::gfx::Enums::Format;//recommended
		}

		@note A MACE header file will never bring any enum from this namespace into `gfx`
		*/
		namespace Enums {
		}
	}
}

//the following two headers should not be included by default as they are for implementation of the
//OpenGL renderer and are useless to the end user unless they are using the OpenGL abstraction
#ifdef MACE_EXPOSE_OPENGL
#	include <MACE/Graphics/OGL/OGL33.h>
#	include <MACE/Graphics/OGL/OGL33Renderer.h>
#	include <MACE/Graphics/OGL/OGL33Context.h>
#endif

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Components.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Window.h>

#endif