/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_UTILITY_AUDIO_H
#define MACE_UTILITY_AUDIO_H 

#include <MACE/Core/Module.h>
#include <MACE/Utility/BitField.h>

#ifdef MACE_OSX
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

#include <vector>

namespace mc {
	class Sound {
	public:
		enum Properties: Byte {
			LOOPING = 0
		};

		Sound(std::string path);

		void play();
		void pause();
		void stop();

		void setVolume(float volume);
		float getVolume() { return volume; }
		
		void init();
		void destroy();

		bool getProperty(const Byte param) const;
		void setProperty(const Byte param, const bool value);

		void setLooping(const bool val);
		bool isLooping() const;

		void setProperties(const BitField& b);
		BitField& getProperties();
		const BitField& getProperties() const;
	private:
		BitField properties;
		unsigned int source, buffer;
		unsigned int frequency;
		int size;
		Enum format;
		unsigned char *buf;
		std::string path;
		float volume;
		float x, y, z;
	};//Sound

	class AudioModule: public Module {
	public:
		void init() override;
		void update() override;
		void destroy() override;

		std::string getName() const override;

		const std::vector<Sound>& getSounds() const;
	
		void addSound(Sound& s);

		AudioModule();
	private:
		std::vector<Sound> sounds = std::vector<Sound>();
		ALCdevice* device;
		ALCcontext* context;
	};//AudioModule
};//mc

#endif//MACE_ULITITY_AUDIO_H