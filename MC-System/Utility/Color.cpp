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

mc::Color::Color(std::array<float,4> rgba)
{
	this->setValues(rgba);
}

mc::Color::Color(Color & copy)
{
	Color(copy.r,copy.g,copy.b,copy.a);
}

mc::Color::Color()
{
	Color(0.0f,0.0f,0.0f,1.0f);
}

bool mc::Color::operator==(Color & other) const
{
	return r==other.r&&g==other.g&&b==other.b&&a==other.a;
}

bool mc::Color::operator!=(Color & other) const
{
	return !(*this == other);
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
