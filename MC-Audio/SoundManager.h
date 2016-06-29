#pragma once

#include <SDL/SDL_mixer.h>
#include <AL/alc.h>

namespace mc {
	class SoundManager {
	private:
		static ALCdevice* m_device;
		static ALCcontext* m_context;
	public:
		static void init();
		static void destroy();
	};
}