#pragma once

#include <PL-System\Definitions.h>

namespace pl {
	
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
	};
}