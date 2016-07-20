#pragma once

#include <MC-System/System.h>
#include <MC-Window/WindowModule.h>
#include <MC-Graphics/Entity.h>

namespace mc {
	/**
	Namespace containing classes used for Graphics in MACE.
	*/
	namespace gfx{

		class GraphicsModule : public mc::gfx::EntityModule {
		public:
			GraphicsModule();

			using EntityModule::EntityModule;

			void init();
			void update();
			void destroy();
			std::string getName() const;
		};
	}
}

