#pragma once

#include <AL/al.h>
#include <vorbis/vorbisfile.h>
#include <cstdio>
#include <iostream>
#include <vector>

namespace mc {
#define BUFFER_SIZE 32768 // 32 KB buffers
	
	class Sound {
	public:
		Sound(const char* fileName);
		void play();
		void stop();
		void pause();
		void destroy();
		void rewind();
	private:
		ALint m_state;
		ALuint m_bufferID;
		ALuint m_sourceID;
		ALenum m_format;
		ALsizei m_freq;
		std::vector<char> m_bufferData;

		void LoadOGG(const char *fileName);
	};
}