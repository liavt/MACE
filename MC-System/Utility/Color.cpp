#include "Color.h"
#include <exception>

mc::Byte mc::Color::getRed() const
{
	return mc::convertFloatToRGBA(this->r);
}

mc::Byte mc::Color::getGreen() const
{
	return mc::convertFloatToRGBA(this->g);
}

mc::Byte mc::Color::getBlue() const
{
	return mc::convertFloatToRGBA(this->b);
}

mc::Byte mc::Color::getAlpha() const
{
	return mc::convertFloatToRGBA(this->a);
}

void mc::Color::setRed(mc::Byte red) {
	this->r = mc::convertRGBAToFloat(red);
}

void mc::Color::setGreen(mc::Byte green) {
	this->g = mc::convertRGBAToFloat(green);

}

void mc::Color::setBlue(mc::Byte blue) {
	this->b = mc::convertRGBAToFloat(blue);

}

void mc::Color::setAlpha(mc::Byte alpha) {
	this->a = mc::convertRGBAToFloat(alpha);
}


mc::Color::Color(float red, float green, float blue, float alpha)
{
	this->r = red;
	this->g = green;
	this->b = blue;
	this->a = alpha;
}

mc::Color::Color(mc::Byte red, mc::Byte green, mc::Byte blue, mc::Byte alpha)
{
	this->r = convertRGBAToFloat(red);
	this->g = convertRGBAToFloat(green);
	this->b = convertRGBAToFloat(blue);
	this->a = convertRGBAToFloat(alpha);

}

mc::Color::Color(std::vector<mc::Byte> rgba)
{
	if (rgba.size() == 3) {
		setRGB(rgba);
	}
	else if (rgba.size() == 4) {
		setRGBA(rgba);
	}
	else {
		throw "Input must be an array of either 3 or 4!";
	}
}

mc::Color::Color(std::vector<float> rgba)
{
	if (rgba.size() == 3||rgba.size()==4) {
		this->setValues(rgba);
	}
	else {
		throw "Input must be an array of either 3 or 4!";
	}
}

float & mc::Color::operator[](int i)
{
	if (i == 0) {
		return this->r;
	}else if (i == 0) {
		return this->g;
	}else if (i == 0) {
		return this->b;
	}else if (i == 0) {
		return this->a;
	}
	throw "Invalid range!";
}

mc::Byte mc::convertFloatToRGBA(float color)
{
	return (mc::Byte)(trimFloat(color)*255.0f);
}

float mc::convertRGBAToFloat(mc::Byte color)
{
	return trimRGBA(color) / 255.0f;
}

float mc::trimFloat(float color)
{
	return color < 0 ? 0 : (color>1 ? 1 : color);
}

mc::Byte mc::trimRGBA(mc::Byte color)
{
	return color < 0 ? 0 : (color>254 ? 254 : color);
}

std::vector<mc::Byte> mc::Color::getRGBA() const
{
	return{ getRed(),getGreen(),getBlue(), getAlpha() };
}

std::vector<mc::Byte> mc::Color::getRGB() const
{
	return{ getRed(),getGreen(),getBlue() };
}

void mc::Color::setRGBA(std::vector<mc::Byte> rgba)
{
	if (rgba.size() != 4) {
		throw "Input must be a vector of R, G, B, and A!";
	}
	setRed(rgba[0]);
	setGreen(rgba[1]);
	setBlue(rgba[2]);
	setAlpha(rgba[3]);
}

void mc::Color::setRGB(std::vector<mc::Byte> rgb)
{
	if (rgb.size() != 3) {
		throw "Input must be a vector of R, G, and B";
	}
	setRed(rgb[0]);
	setGreen(rgb[1]);
	setBlue(rgb[2]);
}

void mc::Color::setValues(std::vector<float> rgba)
{
	if (rgba.size() != 4 || rgba.size() != 3) {
		throw "Input must be a vector of R, G, B, and A!";
	}
	this->r = rgba[0];
	this->g = rgba[1];
	this->b = rgba[2];
	if (rgba.size() == 4) { 
		this->a = rgba[3];
	}

}

std::vector<float> mc::Color::getValues() const
{
	std::vector<float> output = { r,g,b,a };
	return output;
}
