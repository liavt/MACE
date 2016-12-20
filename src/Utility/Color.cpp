/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Color.h>

namespace mc {
	namespace {
		float trimFloat(const float& color) {
			return color < 0.0f ? 0.0f : (color>1.0f ? 1.0f : color);
		}

		Byte convertFloatToRGBA(const float& color) {
			return (mc::Byte)(trimFloat(color)*254.0f);
		}

		float convertRGBAToFloat(const Byte& color) {
			return color / 254.0f;

		}
	}//anon namespace

	const Color Color::RED = Color(255, 0, 0), Color::GREEN = Color(0, 255, 0),
		Color::DARK_BLUE = Color(0, 0, 255), Color::CYAN = Color(0, 255, 255),
		Color::LIGHT_BLUE = Color(50, 200, 255), Color::DARK_RED = Color(150, 0, 0),
		Color::DARK_GREEN = Color(0, 150, 0), Color::PURPLE = Color(100, 0, 255),
		Color::MAGENTA = Color(255, 0, 255), Color::WHITE = Color(255, 255, 255),
		Color::BLACK = Color(0, 0, 0), Color::YELLOW = Color(255, 255, 0),
		Color::DARK_GRAY = Color(100, 100, 100), Color::LIGHT_GRAY = Color(200, 200, 200),
		Color::ORANGE = Color(255, 125, 0), Color::GRAY = Color(150,150,150),
		Color::DARK_ORANGE = Color(255, 100, 0), Color::LIGHT_ORANGE = Color(255, 150, 0);

	Byte Color::getRed() const {
		return convertFloatToRGBA(this->r);
	}

	Byte Color::getGreen() const {
		return convertFloatToRGBA(this->g);
	}

	Byte Color::getBlue() const {
		return convertFloatToRGBA(this->b);
	}

	Byte Color::getAlpha() const {
		return convertFloatToRGBA(this->a);
	}

	void Color::setRed(const Byte& red) {
		this->r = convertRGBAToFloat(red);
	}

	void Color::setGreen(const Byte& green) {
		this->g = convertRGBAToFloat(green);

	}

	void Color::setBlue(const Byte& blue) {
		this->b = convertRGBAToFloat(blue);

	}

	void Color::setAlpha(const Byte& alpha) {
		this->a = convertRGBAToFloat(alpha);
	}


	Color::Color(const float& red, const float& green, const float& blue, const float& alpha) noexcept : r(red), g(green), b(blue), a(alpha) {}

	Color::Color(const std::array<float, 4>& rgba) {
		this->setValues(rgba);
	}

	Color::Color(const Color & copy) noexcept : Color(copy.r, copy.g, copy.b, copy.a) {}

	Color::Color() noexcept : Color(0, 0, 0, 1) {}

	bool Color::operator==(const Color & other) const {
		return r == other.r&&g == other.g&&b == other.b&&a == other.a;
	}

	bool Color::operator!=(const Color & other) const {
		return !operator==(other);
	}

	void Color::setValues(const std::array<float, 3>& rgb) {
		this->r = rgb[0];
		this->g = rgb[1];
		this->b = rgb[2];
	}

	void Color::setValues(const std::array<float, 4>& rgba) {
		this->r = rgba[0];
		this->g = rgba[1];
		this->b = rgba[2];
		this->a = rgba[3];
	}

	std::array<float, 4> Color::getValues() const {
		std::array<float, 4> output = { r,g,b,a };
		return output;
	}

	bool Color::operator<(const Color& other) const {
		//the real g right here
		return other.r < r && other.g < g && other.b < b && other.a < a;
	}

	bool Color::operator<=(const Color& other) const {
		return operator<(other) || operator==(other);
	}

	bool Color::operator>(const Color& other) const {
		//the real g right here
		return !operator<=(other);
	}

	bool Color::operator>=(const Color& other) const {
		//the real g right here
		return !operator<(other);
	}
}//mc