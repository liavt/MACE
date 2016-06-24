#pragma once

#include <vector>
#include <PL-System\Definitions.h>

namespace mc {

	byte_t convertFloatToRGBA(float color);
	float convertRGBAToFloat(byte_t color);

	/**
	If the inputted {@code float} is more than 1, {@code trimFloat} returns 1.
	<p>
	If the inputted {@code float} is less than 0, {@code trimFloat} returns 0.

	@param {@code float} to trim
	@returns A {@code float} guaranteed to be between 0 and 1.
	*/
	float trimFloat(float color);

	/**
	If the inputted {@code byte_t} is more than 254, {@code trimRGBA} returns 254.
	<p>
	If the inputted {@code byte_t} is less than 0, {@code trimRGBA} returns 0.

	@param {@code byte_t} to trim
	@returns A {@code byte_t} guaranteed to be between 0 and 255.
	*/
	byte_t trimRGBA(byte_t color);

	
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

		std::vector<byte_t> getRGBA() const;
		std::vector<byte_t> getRGB() const;

		void setRGBA(std::vector<byte_t> rgba);
		void setRGB(std::vector<byte_t> rgb);

		void setValues(std::vector<float> rgba);
		std::vector<float> getValues() const;

		float r, g, b, a;

		Color(float red = 0,float green=0, float blue =0, float alpha=1);
		Color(byte_t red = 0, byte_t green = 0, byte_t blue = 0, byte_t alpha = 255);
		Color(std::vector<byte_t> rgba);
		Color(std::vector<float> values);

		float& operator[](int i);
	};
}