/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Color.h>

#include <sstream>

namespace mc {
	namespace {
		Byte convertFloatToRGBA(const float& color) {
			return static_cast<Byte>(color * 255.0f);
		}

		float convertRGBAToFloat(const Byte& color) {
			return color / 255.0f;

		}
	}//anon namespace

	namespace Colors {
		const Color RED = Color(200, 0, 0), LIGHT_RED = Color(255, 0, 0),
			DARK_BLUE = Color(0, 0, 255), BLUE = Color(0, 0, 200), CYAN = Color(0, 255, 255),
			LIGHT_BLUE = Color(50, 200, 255), DARK_RED = Color(150, 0, 0),
			DARK_GREEN = Color(0, 50, 0), GREEN = Color(0, 150, 0),
			LIGHT_GREEN = Color(0, 250, 0), PURPLE = Color(100, 0, 255),
			MAGENTA = Color(255, 0, 255), WHITE = Color(255, 255, 255),
			BLACK = Color(0, 0, 0), YELLOW = Color(255, 255, 0),
			DARK_GRAY = Color(100, 100, 100), LIGHT_GRAY = Color(200, 200, 200),
			ORANGE = Color(255, 125, 0), GRAY = Color(150, 150, 150),
			DARK_ORANGE = Color(255, 100, 0), LIGHT_ORANGE = Color(255, 150, 0),
			INVISIBLE = Color(0, 0, 0, 0);
	}

	//AbstractColor

	float * AbstractColor::end() {
		return begin() + size();
	}

	const float * AbstractColor::end() const {
		return begin() + size();
	}

	float & AbstractColor::getComponent(const Index i) {
		if (i >= size()) {
			MACE__THROW(OutOfBounds, "Component " + std::to_string(i) + " is greater than the size of this Color, " + std::to_string(size()));
		}

		return begin()[i];
	}

	const float & AbstractColor::getComponent(const Index i) const {
		if (i >= size()) {
			MACE__THROW(OutOfBounds, "Component " + std::to_string(i) + " is greater than the size of this Color, " + std::to_string(size()));
		}

		return begin()[i];
	}

	float & AbstractColor::operator[](const Index i) {
		return begin()[i];
	}

	const float & AbstractColor::operator[](const Index i) const {
		return begin()[i];
	}

	//AlphaColor

	AlphaColor::AlphaColor(const float alp) : a(alp) {}

	AlphaColor::AlphaColor(const Byte alp) {
		setAlpha(alp);
	}

	Byte AlphaColor::getAlpha() const {
		return convertFloatToRGBA(this->a);
	}

	void AlphaColor::setAlpha(const Byte& alpha) {
		this->a = convertRGBAToFloat(alpha);
	}

	//RGBColor

	RGBColor::RGBColor(const float red, const float green, const float blue) : r(red), g(green), b(blue) {}

	RGBColor::RGBColor(const Byte red, const Byte green, const Byte blue) {
		setRed(red);
		setGreen(green);
		setBlue(blue);
	}

	RGBColor RGBColor::lighten() const {
		return RGBColor(math::min(r + 0.05f, 1.0f), math::min(g + 0.05f, 1.0f), math::min(b + 0.05f, 1.0f));
	}

	RGBColor RGBColor::darken() const {
		return RGBColor(math::max(r - 0.05f, 0.0f), math::max(g - 0.05f, 0.0f), math::max(b - 0.05f, 0.0f));
	}

	Byte RGBColor::getRed() const {
		return convertFloatToRGBA(this->r);
	}

	Byte RGBColor::getGreen() const {
		return convertFloatToRGBA(this->g);
	}

	Byte RGBColor::getBlue() const {
		return convertFloatToRGBA(this->b);
	}

	void RGBColor::setRed(const Byte& red) {
		this->r = convertRGBAToFloat(red);
	}

	void RGBColor::setGreen(const Byte& green) {
		this->g = convertRGBAToFloat(green);

	}

	void RGBColor::setBlue(const Byte& blue) {
		this->b = convertRGBAToFloat(blue);

	}

	void RGBColor::setValues(const std::array<float, 3>& rgb) {
		this->r = rgb[0];
		this->g = rgb[1];
		this->b = rgb[2];
	}

	unsigned int RGBColor::toUnsignedInt() const {
		const Byte red = convertFloatToRGBA(r);
		const Byte green = convertFloatToRGBA(g);
		const Byte blue = convertFloatToRGBA(b);
		return red << 24 | green << 16 | blue << 8;
	}

	std::string RGBColor::toHex() const {
		std::ostringstream out = std::ostringstream();
		out << std::hex << toUnsignedInt();
		return out.str();
	}

	Size RGBColor::size() const {
		return 3;
	}

	float * RGBColor::begin() {
		return &r;
	}

	const float * RGBColor::begin() const {
		return &r;
	}

	//RGBAColor

	RGBAColor::RGBAColor(const float red, const float green, const float blue, const float alpha) : RGBColor(red, green, blue), AlphaColor(alpha) {}

	RGBAColor::RGBAColor(const Byte red, const Byte green, const Byte blue, const Byte alpha) : RGBColor(red, green, blue), AlphaColor(alpha) {}

	void RGBAColor::setValues(const std::array<float, 4>& rgba) {
		this->r = rgba[0];
		this->g = rgba[1];
		this->b = rgba[2];
		this->a = rgba[3];
	}

	std::array<float, 4> RGBAColor::getValues() const {
		return { { r,g,b,a } };
	}

	Vector<float, 4> RGBAColor::toVector() const {
		return Vector<float, 4>({ r, g, b, a });
	}

	unsigned int RGBAColor::toUnsignedInt() const {
		const Byte alpha = convertFloatToRGBA(a);
		return RGBColor::toUnsignedInt() | alpha;
	}

	const float * RGBAColor::flatten(float arr[4]) const {
		arr[0] = r;
		arr[1] = g;
		arr[2] = b;
		arr[3] = a;

		return arr;
	}

	Size RGBAColor::size() const {
		return 4;
	}

	float * RGBAColor::begin() {
		return &r;
	}

	const float * RGBAColor::begin() const {
		return &r;
	}

	//Color

	Color::Color(const int red, const int green, const int blue, const int alpha) noexcept: Color(static_cast<Byte>(red), static_cast<Byte>(green), static_cast<Byte>(blue), static_cast<Byte>(alpha)) {}

	Color::Color(const Byte red, const Byte green, const Byte blue, const Byte alpha) noexcept {
		setRed(red);
		setGreen(green);
		setBlue(blue);
		setAlpha(alpha);
	}

	Color::Color(const unsigned int value) noexcept {
		setRed(value >> 24 & 255);
		setGreen(value >> 16 & 255);
		setBlue(value >> 8 & 255);
		setAlpha(value & 255);
	}

	Color::Color(const float red, const float green, const float blue, const float alpha) noexcept:  RGBAColor(red, green, blue, alpha) {}

	Color::Color(const std::array<float, 4>& rgba) {
		this->setValues(rgba);
	}

	Color::Color(const Vector<float, 4>& values) : Color(values.x(), values.y(), values.z(), values.w()) {}

	Color::Color(const Color & copy) noexcept : Color(copy.r, copy.g, copy.b, copy.a) {}

	Color::Color(const Color & copy, const float alpha) noexcept : Color(copy.r, copy.g, copy.b, alpha) {}

	Color::Color() noexcept : Color(0.0f, 0.0f, 0.0f, 1.0f) {}

	std::ostream & operator<<(std::ostream & output, const Color & v) {
		output << "#" << v.toHex();
		return output;
	}

	bool Color::operator==(const Color & other) const {
		return r == other.r&&g == other.g&&b == other.b&&a == other.a;
	}

	bool Color::operator!=(const Color & other) const {
		return !operator==(other);
	}

	bool Color::operator<(const Color& other) const {
		//the real g right here
		return other.r < r && other.g < g && other.b < b && other.a < a;
	}

	bool Color::operator<=(const Color& other) const {
		return operator<(other) || operator==(other);
	}

	bool Color::operator>(const Color& other) const {
		return !operator<=(other);
	}

	bool Color::operator>=(const Color& other) const {
		return !operator<(other);
	}

	Color fromHex(const std::string hexcode) {
		unsigned int value = 0;
		std::istringstream stream = std::istringstream(hexcode);
		stream >> std::hex >> value;
		return Color(value);
	}
}//mc
