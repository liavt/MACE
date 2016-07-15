#pragma once

#include <iostream>

namespace mc {
	/**
	Namespace with classes relating to audio and sound playback
	*/
	namespace aud
	{
		class Sound {
		private:
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
}