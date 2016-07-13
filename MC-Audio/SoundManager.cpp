#include <MC-Audio/SoundManager.h>

namespace mc {
	namespace aud
	{
		ALCdevice* SoundManager::m_device = NULL;
		ALCcontext* SoundManager::m_context = NULL;

		void SoundManager::init() {
			m_device = alcOpenDevice(NULL);
			m_context = alcCreateContext(m_device, NULL);
			alcMakeContextCurrent(m_context);

			Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);
		}

		void SoundManager::destroy() {
			Mix_CloseAudio();
			alcDestroyContext(m_context);
			alcCloseDevice(m_device);
		}
	}
}