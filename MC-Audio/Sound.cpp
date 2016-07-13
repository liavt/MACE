#include <MC-Audio/Sound.h>

namespace mc {
	namespace aud
	{
		Sound::Sound(const char* file) {
			m_chunk = Mix_LoadWAV(file);
			alGenBuffers(1, &m_buffer);
			alGenSources(1, &m_source);

			alBufferData(m_buffer, AL_FORMAT_STEREO16, m_chunk->abuf, m_chunk->alen, 44100);

			alSourcei(m_source, AL_BUFFER, m_buffer);
			alSourcef(m_source, AL_PITCH, 1.0f);
			alSourcef(m_source, AL_GAIN, 1.0f);
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
		}

		void Sound::play() {
			alSourcePlay(m_source);
		}

		void Sound::pause() {
			alSourcePause(m_source);
		}

		void Sound::stop() {
			alSourceStop(m_source);
		}

		void Sound::rewind() {
			alSourceRewind(m_source);
		}

		void Sound::destroy() {
			alDeleteSources(1, &m_source);
			alDeleteBuffers(1, &m_buffer);
		}


		void Sound::setGain(float gain) {
			alSourcef(m_source, AL_GAIN, gain);
		}

		void Sound::setPitch(float pitch) {
			alSourcef(m_source, AL_PITCH, pitch);
		}

		void Sound::setLooping(bool looping) {
			if (looping) {
				alSourcei(m_source, AL_LOOPING, AL_TRUE);
			}
			else {
				alSourcei(m_source, AL_LOOPING, AL_FALSE);
			}
		}
	}
}