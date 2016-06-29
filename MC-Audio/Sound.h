#pragma once

#include <SDL/SDL_mixer.h>
#include <AL/al.h>
#include <iostream>

namespace mc {
	class Sound {
	private:
		ALuint m_buffer;
		ALuint m_source;
		Mix_Chunk* m_chunk;
	public:
		Sound(const char* file);
		void play();
		void pause();
		void stop();
		void rewind();
		void destroy();
		void setGain(float gain);
		void setPitch(float pitch);
		void setLooping(bool looping);
	};
}