#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace mc {
	class SoundManager {
	public:
		static void init();
		static void destroy();
	private:
		static ALCdevice* m_device;
		static ALCcontext* m_context;
	};
}