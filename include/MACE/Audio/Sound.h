/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

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