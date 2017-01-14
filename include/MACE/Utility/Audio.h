/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma
#ifndef MACE_UTILITY_AUDIO_H
#define MACE_UTILITY_AUDIO_H 

#include <MACE/Core/Module.h>
#include <MACE/Utility/BitField.h>

#include <vector>

namespace mc {
	class Sound {
	public:
		enum Properties: Byte {
			LOOPING
		};

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
	};//Sound

	class AudioModule: public Module {
		void init() override;
		void update() override;
		void destroy() override;

		std::string getName() const override;

		void addSound(Sound& s);

		const std::vector<Sound>& getSounds() const;
	private:
		std::vector<Sound> sounds = std::vector<Sound>();
	};//AudioModule
};//mc

#endif//MACE_ULITITY_AUDIO_H