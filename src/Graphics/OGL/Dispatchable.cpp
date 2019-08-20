/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/Dispatchable.h>
#include <MACE/Graphics/OGL/OGLContext.h>

namespace mc {
	namespace internal {
		namespace ogl {
			Dispatchable::Dispatchable(std::shared_ptr<Context> con) : context(con) {}

			void Dispatchable::dispatch(const DispatchFunction func) const {
				context->dispatch(std::move(func));
			}
		}//ogl
	}//internal
}//mc
