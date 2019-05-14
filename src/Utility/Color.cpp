/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Utility/Color.h>

#include <sstream>

namespace mc {
	namespace {
		Byte convertFloatToRGBA(const float& color) MACE_EXPECTS(color >= 0.0f && color <= 1.0f){
			return static_cast<Byte>(color * 255.0f);
		}

		float convertRGBAToFloat(const Byte& color) MACE_ENSURES(ret, ret >= 0.0f && ret <= 0.0f){
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

	Color::Color(const float red, const float green, const float blue, const float alpha) noexcept : r(red), g(green), b(blue), a(alpha) {}

	Color::Color(const int red, const int green, const int blue, const int alpha) noexcept: Color(static_cast<Byte>(red), static_cast<Byte>(green), static_cast<Byte>(blue), static_cast<Byte>(alpha)) {}

	Color::Color(const Byte red, const Byte green, const Byte blue, const Byte alpha) noexcept : Color(convertRGBAToFloat(red), convertRGBAToFloat(green), convertRGBAToFloat(blue), convertRGBAToFloat(alpha)) {}

	Color::Color(const unsigned int value) noexcept : Color(static_cast<Byte>(value >> 24 & 255), static_cast<Byte>(value >> 16 & 255), static_cast<Byte>(value >> 8 & 255), static_cast<Byte>(value & 255)) {}

	Color::Color(const std::array<float, 4>& rgba) : Color(rgba[0], rgba[1], rgba[2], rgba[3]){}

	Color::Color(const float values[4]) : Color(values[0], values[1], values[2], values[3]) {}

	Color::Color(const Vector<float, 4>& values) : Color(values.x(), values.y(), values.z(), values.w()) {}

	Color::Color(const Color & copy) noexcept : Color(copy.r, copy.g, copy.b, copy.a) {}

	Color::Color(const Color & copy, const float alpha) noexcept : Color(copy.r, copy.g, copy.b, alpha) {}

	Color::Color() noexcept : Color(0.0f, 0.0f, 0.0f, 1.0f) {}

	Color Color::lighten() const {
		return Color(math::min(r + 0.05f, 1.0f), math::min(g + 0.05f, 1.0f), math::min(b + 0.05f, 1.0f), a);
	}

	Color Color::darken() const {
		return Color(math::max(r - 0.05f, 0.0f), math::max(g - 0.05f, 0.0f), math::max(b - 0.05f, 0.0f), a);
	}

	Byte Color::getRed() const {
		return convertFloatToRGBA(this->r);
	}

	Byte Color::getGreen() const {
		return convertFloatToRGBA(this->g);
	}

	Byte Color::getBlue() const {
		return convertFloatToRGBA(this->b);
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

	void Color::setValues(const std::array<float, 3>& rgb) {
		this->r = rgb[0];
		this->g = rgb[1];
		this->b = rgb[2];
	}

	unsigned int Color::toUnsignedIntNoAlpha() const {
		const Byte red = convertFloatToRGBA(r);
		const Byte green = convertFloatToRGBA(g);
		const Byte blue = convertFloatToRGBA(b);
		return red << 24 | green << 16 | blue << 8;
	}

	std::string Color::toHex() const {
		std::ostringstream out = std::ostringstream();
		out << std::hex << toUnsignedInt();
		return out.str();
	}

	void Color::setValues(const std::array<float, 4>& rgba) {
		this->r = rgba[0];
		this->g = rgba[1];
		this->b = rgba[2];
		this->a = rgba[3];
	}

	std::array<float, 4> Color::getValues() const {
		return { { r,g,b,a } };
	}

	Byte Color::getAlpha() const {
		return convertFloatToRGBA(this->a);
	}

	void Color::setAlpha(const Byte& alpha) {
		this->a = convertRGBAToFloat(alpha);
	}

	Vector<float, 4> Color::toVector() const {
		return Vector<float, 4>({ r, g, b, a });
	}

	unsigned int Color::toUnsignedInt() const {
		const Byte alpha = convertFloatToRGBA(a);
		return toUnsignedIntNoAlpha() | alpha;
	}

	const float* Color::flatten(float arr[4]) const {
		arr[0] = r;
		arr[1] = g;
		arr[2] = b;
		arr[3] = a;

		return arr;
	}

	Size Color::size() const {
		return 4;
	}

	float* Color::begin() {
		return &r;
	}

	const float* Color::begin() const {
		return &r;
	}

	float* Color::end() {
		return begin() + size();
	}

	const float* Color::end() const {
		return begin() + size();
	}

	float& Color::getComponent(const Index i) {
#ifdef MACE_DEBUG_CHECK_ARGS
		if (i >= size()) MACE_UNLIKELY{
			MACE__THROW(OutOfBounds, "Component " + std::to_string(i) + " is greater than the size of this Color, " + std::to_string(size()));
		}
#endif

		return begin()[i];
	}

	const float& Color::getComponent(const Index i) const {
#ifdef MACE_DEBUG_CHECK_ARGS
		if (i >= size()) MACE_UNLIKELY{
			MACE__THROW(OutOfBounds, "Component " + std::to_string(i) + " is greater than the size of this Color, " + std::to_string(size()));
		}
#endif

		return begin()[i];
	}

	float& Color::operator[](const Index i) {
		return begin()[i];
	}

	const float& Color::operator[](const Index i) const {
		return begin()[i];
	}

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
