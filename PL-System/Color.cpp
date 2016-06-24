#include "Color.h"
#include <exception>

byte_t mc::Color::getRed() const
{
	return mc::convertFloatToRGBA(r);
}

byte_t mc::Color::getGreen() const
{
	return mc::convertFloatToRGBA(g);
}

byte_t mc::Color::getBlue() const
{
	return mc::convertFloatToRGBA(b);
}

byte_t mc::Color::getAlpha() const
{
	return mc::convertFloatToRGBA(a);
}

void mc::Color::setRed(byte_t red) {
	r = mc::convertRGBAToFloat(red);
}

void mc::Color::setGreen(byte_t green) {
	g = mc::convertRGBAToFloat(green);

}

void mc::Color::setBlue(byte_t blue) {
	b = mc::convertRGBAToFloat(blue);

}

void mc::Color::setAlpha(byte_t alpha) {
	a = mc::convertRGBAToFloat(alpha);
}

byte_t * mc::Color::getRGBA() const
{
	byte_t output[4];
	output[0] = getRed();
	output[1] = getGreen();
	output[2] = getBlue();
	output[3] = getAlpha();
	return output;
}

byte_t * mc::Color::getRGB() const
{
	byte_t output[3];
	output[0] = getRed();
	output[1] = getGreen();
	output[2] = getBlue();
	return output;
}

void mc::Color::setRGBA(byte_t rgba[])
{
	if (sizeof(rgba) / sizeof(byte_t)!=4) {
		throw "Input needs to be an array of 4!";
	}
	setRed(rgba[0]);
	setGreen(rgba[1]);
	setBlue(rgba[2]);
	setAlpha(rgba[3]);
}

void mc::Color::setRGB(byte_t rgb[])
{
	if (sizeof(rgb) / sizeof(byte_t) != 3) {
		throw "Input needs to be an array of 3!";
	}
	setRed(rgb[0]);
	setGreen(rgb[1]);
	setBlue(rgb[2]);
}

mc::Color::Color(float red, float green, float blue, float alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

mc::Color::Color(byte_t red, byte_t green, byte_t blue, byte_t alpha)
{
	r = convertRGBAToFloat(red);
	g = convertRGBAToFloat(green);
	b = convertRGBAToFloat(blue);
	a = convertRGBAToFloat(alpha);

}

byte_t mc::convertFloatToRGBA(float color)
{
	return trimFloat(color)*255.0f;
}

float mc::convertRGBAToFloat(byte_t color)
{
	return trimRGBA(color) / 255.0f;
}

float mc::trimFloat(float color)
{
	return color<0 ? 0 : (color>1 ? 1 : color);
}

byte_t mc::trimRGBA(byte_t color)
{
	return color<0 ? 0 : (color>254 ? 254 : color);
}
