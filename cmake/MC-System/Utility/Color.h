#pragma once

#include "Constants.h"
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

		Color(float red,float green, float blue, float alpha=1.0f);
		Color(Byte red, Byte green, Byte blue , Byte alpha = 254);
		Color(std::array<Byte,4> rgba);
		Color(std::array<float,4> values);
		Color();

		float& operator[](int i);
	};
}