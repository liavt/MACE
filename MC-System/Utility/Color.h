#pragma once

#include <MC-System\Definitions.h>
#include <array>

namespace mc {	
	class Color {
	public:

		static Byte convertFloatToRGBA(float color);
		static float convertRGBAToFloat(Byte color);

		/**
		If the inputted {@code float} is more than 1, {@code trimFloat} returns 1.
		<p>
		If the inputted {@code float} is less than 0, {@code trimFloat} returns 0.

		@param {@code float} to trim
		@returns A {@code float} guaranteed to be between 0 and 1.
		*/
		static float trimFloat(float color);

		/**
		If the inputted {@code Byte} is more than 254, {@code trimRGBA} returns 254.
		<p>
		If the inputted {@code Byte} is less than 0, {@code trimRGBA} returns 0.

		@param {@code Byte} to trim
		@returns A {@code Byte} guaranteed to be between 0 and 255.
		*/
		static Byte trimRGBA(Byte color);


		Byte getRed() const;
		Byte getGreen() const;
		Byte getBlue() const;
		Byte getAlpha() const;

		void setRed(Byte r);
		void setGreen(Byte g);
		void setBlue(Byte b);
		void setAlpha(Byte a);

		std::array<Byte,4> getRGBA() const;
		std::array<Byte,3> getRGB() const;

		void setRGBA(std::array<Byte,4> rgba);
		void setRGB(std::array<Byte,3> rgb);

		void setValues(std::array<float,4> rgba);
		void setValues(std::array<float, 3> rgb);
		std::array<float,4> getValues() const;

		float r, g, b, a;

		Color(float red = 0,float green=0, float blue =0, float alpha=1);
		Color(Byte red = 0, Byte green = 0, Byte blue = 0, Byte alpha = 255);
		Color(std::array<Byte,4> rgba);
		Color(std::array<float,4> values);

		float& operator[](int i);
	};
}