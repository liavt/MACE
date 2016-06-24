#pragma once

#include <PL-System\Definitions.h>

namespace mc {

	byte_t convertFloatToRGBA(float color);
	float convertRGBAToFloat(byte_t color);

	/**
	*/
	byte_t trimRGBA(byte_t byte);

	
	class Color {
	public:
		byte_t getRed() const;
		byte_t getGreen() const;
		byte_t getBlue() const;
		byte_t getAlpha() const;

		void setRed(byte_t r);
		void setGreen(byte_t g);
		void setBlue(byte_t b);
		void setAlpha(byte_t a);


		float r, g, b, a;

		Color(float red = 0,float green=0, float blue =0, float alpha=1);
		Color(byte_t red = 0, byte_t green = 0, byte_t blue = 0, byte_t alpha = 255);
	};
}