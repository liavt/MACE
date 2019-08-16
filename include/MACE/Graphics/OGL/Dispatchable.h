/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_DISPATCHABLE_H
#define MACE__GRAPHICS_OGL_DISPATCHABLE_H

#include <MACE/Core/Constants.h>

#include <functional>

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

namespace mc {
	namespace internal {
		namespace ogl {
			class Context;

			using DispatchFunction = std::function<void()>;

			class MACE_NOVTABLE Dispatchable {
			protected:
				std::shared_ptr<Context> context;

				Dispatchable(std::shared_ptr<Context> context);

				void dispatch(const DispatchFunction func) const;
			};
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_DISPATCHABLE_H
