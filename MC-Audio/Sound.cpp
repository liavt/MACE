#include <MC-Audio/Sound.h>

namespace mc {
	Sound::Sound(const char* fileName) {
		alGenBuffers(1, &m_bufferID);
		alGenSources(1, &m_sourceID);
		alSource3f(m_sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

		LoadOGG(fileName);

		alBufferData(m_bufferID, m_format, &m_bufferData[0], static_cast<ALsizei>(m_bufferData.size()), m_freq);

		alSourcei(m_sourceID, AL_BUFFER, m_bufferID);
	}

	void Sound::play() {
		alSourcePlay(m_sourceID);
	}

	void Sound::stop() {
		alSourceStop(m_sourceID);
		alSourceRewind(m_sourceID);
	}

	void Sound::rewind() {
		alSourceRewind(m_sourceID);
	}

	void Sound::pause() {
		alSourcePause(m_sourceID);
	}

	void Sound::destroy() {
		alDeleteSources(1, &m_sourceID);
		alDeleteBuffers(1, &m_bufferID);
	}

	void Sound::LoadOGG(const char *fileName) {
		int endian = 0;
		int bitStream;
		long bytes;
		char charArray[BUFFER_SIZE];
		vorbis_info *pInfo;
		OggVorbis_File oggFile;

		FILE *f;
		f = fopen(fileName, "rb");
		ov_open(f, &oggFile, NULL, 0);
		pInfo = ov_info(&oggFile, -1);

		if (pInfo->channels == 1) {
			m_format = AL_FORMAT_MONO16;
		} else {
			m_format = AL_FORMAT_STEREO16;
		}

		m_freq = pInfo->rate;

		do {
			bytes = ov_read(&oggFile, charArray, BUFFER_SIZE, endian, 2, 1, &bitStream);
			m_bufferData.insert(m_bufferData.end(), charArray, charArray + bytes);
		} while (bytes > 0);

		ov_clear(&oggFile);
	}
}