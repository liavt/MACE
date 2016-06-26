#pragma once

#include <MC-System\Definitions.h>
#include <vector>

namespace mc {

	Byte convertFloatToRGBA(float color);
	float convertRGBAToFloat(Byte color);

	/**
	If the inputted {@code float} is more than 1, {@code trimFloat} returns 1.
	<p>
	If the inputted {@code float} is less than 0, {@code trimFloat} returns 0.

	@param {@code float} to trim
	@returns A {@code float} guaranteed to be between 0 and 1.
	*/
	float trimFloat(float color);

	/**
	If the inputted {@code Byte} is more than 254, {@code trimRGBA} returns 254.
	<p>
	If the inputted {@code Byte} is less than 0, {@code trimRGBA} returns 0.

	@param {@code Byte} to trim
	@returns A {@code Byte} guaranteed to be between 0 and 255.
	*/
	Byte trimRGBA(Byte color);

	
	class Color {
	public:
		Byte getRed() const;
		Byte getGreen() const;
		Byte getBlue() const;
		Byte getAlpha() const;

		void setRed(Byte r);
		void setGreen(Byte g);
		void setBlue(Byte b);
		void setAlpha(Byte a);

		std::vector<Byte> getRGBA() const;
		std::vector<Byte> getRGB() const;

		void setRGBA(std::vector<Byte> rgba);
		void setRGB(std::vector<Byte> rgb);

		void setValues(std::vector<float> rgba);
		std::vector<float> getValues() const;

		float r, g, b, a;

		Color(float red = 0,float green=0, float blue =0, float alpha=1);
		Color(Byte red = 0, Byte green = 0, Byte blue = 0, Byte alpha = 255);
		Color(std::vector<Byte> rgba);
		Color(std::vector<float> values);

		float& operator[](int i);
	};
}