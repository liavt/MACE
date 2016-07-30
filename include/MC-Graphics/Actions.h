#pragma once
#include <MC-Graphics/Entity.h>

namespace mc {
	namespace gfx {
		class TransformationAction : public Action {
			void init(Entity& e);
			bool update(Entity& e);
			void destroy(Entity& e);
		};
	}
}