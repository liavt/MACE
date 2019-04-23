/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Audio.h>
#include <MACE/Core/System.h>

#if defined(MACE_OSX) || MACE_HAS_INCLUDE(<OpenAL/al.h>)
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

#include <cstdio>

namespace mc {
	AudioModule::AudioModule() {
		device = alcOpenDevice(nullptr);
		context = alcCreateContext(device, nullptr);
		alcMakeContextCurrent(context);
	}

	void AudioModule::init() {}

	void AudioModule::update() {}

	void AudioModule::destroy() {
		for (Index i = 0; i < sounds.size(); ++i) {
			sounds[i].destroy();
		}

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}

	std::string AudioModule::getName() const {
		return "MACE/Audio";
	}

	void AudioModule::addSound(Sound & s) {
		sounds.push_back(s);

		s.init();
	}

	const std::vector<Sound>& AudioModule::getSounds() const {
		return sounds;
	}

	Sound::Sound(const std::string& p) : path(p) {}

	void Sound::init() {
		alGenBuffers(1, &buffer);
		alGenSources(1, &source);

		struct RIFFHeader {
			char chunkID[4];
			long chunkSize;//size not including chunkSize or chunkID
			char format[4];
		};

		struct WAVEFormat {
			char subChunkID[4];
			long subChunkSize;
			short audioFormat;
			short numChannels;
			long sampleRate;
			long byteRate;
			short blockAlign;
			short bitsPerSample;
		};

		struct WAVEData {
			char subChunkID[4]; //should contain the word data
			long subChunk2Size; //Stores the size of the data block
		};

		std::FILE* soundFile = nullptr;
		WAVEFormat waveFormat;
		RIFFHeader riffHeader;
		WAVEData waveData;

		try {
			soundFile = mc::os::fopen(&soundFile, path.c_str(), "rb");
			if (!soundFile)
				MACE__THROW(BadSound, path);

			if (!std::fread(&riffHeader, sizeof(RIFFHeader), 1, soundFile)) {
				MACE__THROW(BadSound, "Error loading WAVE data into struct");
			}

			if ((riffHeader.chunkID[0] != 'R' ||
				 riffHeader.chunkID[1] != 'I' ||
				 riffHeader.chunkID[2] != 'F' ||
				 riffHeader.chunkID[3] != 'F') ||
				 (riffHeader.format[0] != 'W' ||
				  riffHeader.format[1] != 'A' ||
				  riffHeader.format[2] != 'V' ||
				  riffHeader.format[3] != 'E')) {
				MACE__THROW(BadSound, "Invalid RIFF or WAVE Header");
			}

			if (!std::fread(&waveFormat, sizeof(WAVEFormat), 1, soundFile)) {
				MACE__THROW(BadSound, "Error loading WAVE data into struct");
			}

			if (waveFormat.subChunkID[0] != 'f' ||
				waveFormat.subChunkID[1] != 'm' ||
				waveFormat.subChunkID[2] != 't' ||
				waveFormat.subChunkID[3] != ' ') {
				MACE__THROW(BadSound, "Invalid Wave Format");
			}

			if (waveFormat.subChunkSize > 16) {
				std::fseek(soundFile, sizeof(short), SEEK_CUR);
			}

			if (!std::fread(&waveData, sizeof(WAVEData), 1, soundFile)) {
				MACE__THROW(BadSound, "Error loading WAVE data into struct");
			}

			if (waveData.subChunkID[0] != 'd' ||
				waveData.subChunkID[1] != 'a' ||
				waveData.subChunkID[2] != 't' ||
				waveData.subChunkID[3] != 'a') {
				MACE__THROW(BadSound, "Invalid data header");
			}

			buf = new unsigned char[waveData.subChunk2Size];

			if (!std::fread(buf, waveData.subChunk2Size, 1, soundFile)) {
				MACE__THROW(BadSound, "Error loading WAVE data into struct");
			}

			size = waveData.subChunk2Size;
			frequency = waveFormat.sampleRate;

			if (waveFormat.numChannels == 1) {
				if (waveFormat.bitsPerSample == 8) {
					format = AL_FORMAT_MONO8;
				} else if (waveFormat.bitsPerSample == 16) {
					format = AL_FORMAT_MONO16;
				}
			} else if (waveFormat.numChannels == 2) {
				if (waveFormat.bitsPerSample == 8) {
					format = AL_FORMAT_STEREO8;
				} else if (waveFormat.bitsPerSample == 16) {
					format = AL_FORMAT_STEREO16;
				}
			}

			alGenBuffers(1, &buffer);

			alBufferData(buffer, format, (void*)buf,
						 size, frequency);

			alGenSources(1, &source);
			alSourcei(source, AL_BUFFER, buffer);

			std::fclose(soundFile);
			return;
		} catch (const Error& error) {
			if (soundFile != nullptr) {
				std::fclose(soundFile);
			}

			Error::handleError(error);
		}
	}

	void Sound::play() {
		alSourcePlay(source);
	}

	void Sound::pause() {
		alSourcePause(source);
	}

	void Sound::stop() {
		alSourceStop(source);
	}

	void Sound::destroy() {
		stop();

		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
	}

	void Sound::setVolume(const float vol) {
		volume = vol;
		alSourcef(source, AL_GAIN, volume);
	}

	float & Sound::getVolume() {
		return volume;
	}

	const float & Sound::getVolume() const {
		return volume;
	}

	void Sound::setLooping(const bool val) {
		if (val) {
			properties |= LOOPING;
			alSourcei(source, AL_LOOPING, AL_TRUE);
		} else {
			properties &= ~LOOPING;
			alSourcei(source, AL_LOOPING, AL_FALSE);
		}
	}

	bool Sound::isLooping() const {
		return properties & LOOPING;
	}
}//mc
