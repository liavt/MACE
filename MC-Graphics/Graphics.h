#pragma once

#include <MC-System/System.h>
#include <MC-Graphics/Entity.h>

namespace mc {
	class GraphicsModule : public EntityModule {
	public:
		GraphicsModule();

		using EntityModule::EntityModule;

		void init();
		void update();
		void destroy();
		std::string getName() const;
	};
}
