#pragma once

#include <MC-System/System.h>
#include <MC-Graphics/Entity.h>
#include <MC-Window/WindowModule.h>

namespace mc {
	/**
	Namespace containing classes used for Graphics in MACE.
	*/
	namespace gfx{
		class GraphicsModule : public EntityModule, public win::WindowModule {
		public:
			GraphicsModule();

			using EntityModule::EntityModule;
			using WindowModule::WindowModule;

			void init();
			void update();
			void destroy();
			std::string getName() const;
		};
	}
}
