/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Audio.h>
#include <AL/al.h>

namespace mc {
	void AudioModule::init() {
		for( Index i = 0; i < sounds.size(); ++i ) {
			sounds[i].init();
		}
	}

	void AudioModule::update() {

	}

	void AudioModule::destroy() {
		for( Index i = 0; i < sounds.size(); ++i ) {
			sounds[i].destroy();
		}
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

	void Sound::init() {
		
	}

	void Sound::destroy() {
	
	}

	bool Sound::getProperty(const Byte param) const {
		return properties.getBit(param);
	}

	void Sound::setProperty(const Byte param, const bool value) {
		properties.setBit(param, value);
	}

	void Sound::setLooping(const bool val) {
		properties.setBit(LOOPING, val);
	}

	bool Sound::isLooping() const {
		return properties.getBit(LOOPING);
	}

	void Sound::setProperties(const BitField & b) {
		properties = b;
	}
	BitField & Sound::getProperties() {
		return properties;
	}
	const BitField & Sound::getProperties() const {
		return properties;
	}
}//mc
