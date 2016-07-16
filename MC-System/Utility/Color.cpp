#include <MC-System/Utility/Color.h>
#include <exception>

mc::Byte mc::Color::getRed() const
{
	return mc::Color::convertFloatToRGBA(this->r);
}

mc::Byte mc::Color::getGreen() const
{
	return mc::Color::convertFloatToRGBA(this->g);
}

mc::Byte mc::Color::getBlue() const
{
	return mc::Color::convertFloatToRGBA(this->b);
}

mc::Byte mc::Color::getAlpha() const
{
	return mc::Color::convertFloatToRGBA(this->a);
}

void mc::Color::setRed(mc::Byte red) {
	this->r = mc::Color::convertRGBAToFloat(red);
}

void mc::Color::setGreen(mc::Byte green) {
	this->g = mc::Color::convertRGBAToFloat(green);

}

void mc::Color::setBlue(mc::Byte blue) {
	this->b = mc::Color::Color::convertRGBAToFloat(blue);

}

void mc::Color::setAlpha(mc::Byte alpha) {
	this->a = mc::Color::convertRGBAToFloat(alpha);
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

mc::Color::Color(std::array<mc::Byte, 4> rgba)
{
	this->setRGBA(rgba);
}

mc::Color::Color(std::array<float,4> rgba)
{
	this->setValues(rgba);
}

mc::Color::Color()
{
	Color(0.0f,0.0f,0.0f,1.0f);
}

float & mc::Color::operator[](int i)
{
	if (i == 0) {
		return this->r;
	}else if (i == 1) {
		return this->g;
	}else if (i == 2) {
		return this->b;
	}else if (i == 3) {
		return this->a;
	}
	throw "Invalid range!";
}

mc::Byte mc::Color::convertFloatToRGBA(float color)
{
	return (mc::Byte)(trimFloat(color)*254.0f);
}

float mc::Color::convertRGBAToFloat(mc::Byte color)
{
	return color / 254.0f;
}

float mc::Color::trimFloat(float color)
{
	return color < 0.0f ? 0.0f : (color>1.0f ? 1.0f : color);
}

std::array<mc::Byte, 4> mc::Color::getRGBA() const
{
	return{ getRed(),getGreen(),getBlue(), getAlpha() };
}

std::array<mc::Byte, 3> mc::Color::getRGB() const
{
	return{ getRed(),getGreen(),getBlue() };
}

void mc::Color::setRGBA(std::array<mc::Byte, 4> rgba)
{
	setRed(rgba[0]);
	setGreen(rgba[1]);
	setBlue(rgba[2]);
	setAlpha(rgba[3]);
}

void mc::Color::setRGB(std::array<mc::Byte, 3> rgb)
{
	setRed(rgb[0]);
	setGreen(rgb[1]);
	setBlue(rgb[2]);
}

void mc::Color::setValues(std::array<float, 3> rgb)
{
	this->r = rgb[0];
	this->g = rgb[1];
	this->b = rgb[2];
}

void mc::Color::setValues(std::array<float, 4> rgba)
{
	this->r = rgba[0];
	this->g = rgba[1];
	this->b = rgba[2];
	this->a = rgba[3];
}

std::array<float, 4> mc::Color::getValues() const
{
	std::array<float, 4> output = { r,g,b,a };
	return output;
}
