#include "Color.h"

byte_t pl::Color::getRed() const
{
	return (byte_t)((r < 0 ? 0 : (r>1 ? 1 : r))*255.0f);
}

byte_t pl::Color::getGreen() const
{
	return (byte_t)((g < 0 ? 0 : (g>1 ? 1 : g))*255.0f);
}

byte_t pl::Color::getBlue() const
{
	return (byte_t)((b < 0 ? 0 : (b>1 ? 1 : b))*255.0f);
}

byte_t pl::Color::getAlpha() const
{
	return (byte_t)((a < 0 ? 0 : (a>1 ? 1 : a))*255.0f);
}

void pl::Color::setRed(byte_t red) {
	r = red;
}

void pl::Color::setGreen(byte_t green) {
	g = green;

}

void pl::Color::setBlue(byte_t blue) {
	b = blue;

}

void pl::Color::setAlpha(byte_t alpha) {
	a = alpha;
}

pl::Color::Color(float red, float green, float blue, float alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}


