/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__UTILITY_AUDIO_H
#define MACE__UTILITY_AUDIO_H 

#include <MACE/Core/Instance.h>
#include <MACE/Core/Interfaces.h>

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
	@todo fix memory leak from `new` for `buf`
	*/
	class Sound: public Initializable {
	public:
		enum Properties: Byte {
			LOOPING = 0x01
		};

		Sound(const std::string& path);

		void play();
		void pause();
		void stop();

		void setVolume(const float volume);
		float& getVolume();
		const float& getVolume() const;

		/**
		@todo use fstream instead of cstdio
		*/
		void init() override;
		void destroy() override;

		void setLooping(const bool val);
		bool isLooping() const;
	private:
		Byte properties = 0;
		unsigned int source, buffer = 0;
		unsigned int frequency = 0;
		int size = 0;
		Enum format;
		unsigned char *buf = nullptr;
		std::string path;
		float volume = 1;
		float x = 0, y = 0, z = 0;
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