/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/System/Utility/Color.h>
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

void mc::Color::setRed(const mc::Byte& r) {
	this->r = mc::Color::convertRGBAToFloat(r);
}

void mc::Color::setGreen(const mc::Byte& g) {
	this->g = mc::Color::convertRGBAToFloat(g);

}

void mc::Color::setBlue(const mc::Byte& b) {
	this->b = mc::Color::Color::convertRGBAToFloat(b);

}

void mc::Color::setAlpha(const mc::Byte& a) {
	this->a = mc::Color::convertRGBAToFloat(a);
}


mc::Color::Color(const float& red, const float& green, const float& blue, const float& alpha) : r(red),g(green),b(blue),a(alpha)
{
}

mc::Color::Color(const std::array<float,4>& rgba)
{
	this->setValues(rgba);
}

mc::Color::Color(const Color & copy) : r(copy.r),g(copy.g),b(copy.b),a(copy.a)
{
}

mc::Color::Color() : r(0.0f),g(0.0f),b(0.0f),a(1.0f)
{
}

bool mc::Color::operator==(const Color & other) const
{
	return r==other.r&&g==other.g&&b==other.b&&a==other.a;
}

bool mc::Color::operator!=(const Color & other) const
{
	return !(*this == other);
}

mc::Byte mc::Color::convertFloatToRGBA(const float& color)
{
	return (mc::Byte)(trimFloat(color)*254.0f);
}

float mc::Color::convertRGBAToFloat(const mc::Byte& color)
{
	return color / 254.0f;
}

float mc::Color::trimFloat(const float& color)
{
	return color < 0.0f ? 0.0f : (color>1.0f ? 1.0f : color);
}

void mc::Color::setValues(const std::array<float, 3>& rgb)
{
	this->r = rgb[0];
	this->g = rgb[1];
	this->b = rgb[2];
}

void mc::Color::setValues(const std::array<float, 4>& rgba)
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
