#include <MC-Audio/SoundManager.h>

namespace mc {
	ALCdevice* SoundManager::m_device = 0;
	ALCcontext* SoundManager::m_context = 0;

	void SoundManager::init() {
		m_device = alcOpenDevice(0);
		m_context = alcCreateContext(m_device, 0);
		alcMakeContextCurrent(m_context);

		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	}

	void SoundManager::destroy() {
		alcMakeContextCurrent(0);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);
	}
}