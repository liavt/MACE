/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_AUDIO_H
#define MACE__UTILITY_AUDIO_H 

#include <MACE/Core/Instance.h>
#include <MACE/Core/Interfaces.h>
#include <MACE/Utility/BitField.h>

#include <vector>

//forward declaration to prevent inclusion of alc.h
//ALCdevice and ALCcontext are typedef'd to these structs, so we can forward declare these safely
struct ALCdevice_struct;
struct ALCcontext_struct;

namespace mc {
	/**
	Thrown when an error occured trying to read or write a sound file
	*/
	MACE__DECLARE_ERROR(BadSound);

	/**
	@todo add more sound file formats, other than .wav. stb_vorbis can provide support for .ogg files
	*/
	class Sound: public Initializable {
	public:
		enum Properties: Byte {
			LOOPING = 0
		};

		Sound(const std::string& path);

		void play();
		void pause();
		void stop();

		void setVolume(float volume);
		float getVolume() { return volume; }
		
		/**
		@todo use fstream instead of cstdio
		*/
		void init() override;
		void destroy() override;

		bool getProperty(const Byte param) const;
		void setProperty(const Byte param, const bool value);

		void setLooping(const bool val);
		bool isLooping() const;

		void setProperties(const BitField& b);
		BitField& getProperties();
		const BitField& getProperties() const;
	private:
		BitField properties = 0;
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
		AudioModule();

		void init() override;
		void update() override;
		void destroy() override;

		std::string getName() const override;

		const std::vector<Sound>& getSounds() const;
	
		void addSound(Sound& s);
	private:
		std::vector<Sound> sounds = std::vector<Sound>();
		ALCdevice_struct* device;
		ALCcontext_struct* context;
	};//AudioModule
}//mc

#endif//MACE_ULITITY_AUDIO_H