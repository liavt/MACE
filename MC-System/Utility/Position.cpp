#include <MC-System/Utility/Position.h>
namespace mc {
	void Coordinate::setRelative(float value)
	{
		isAbsolute = false;
		this->value.relative = value;
	}

	const float Coordinate::get(Size right) const
	{
		if (isAbsolute) {
			return (value.absolute / (((float) right)/2.0f))-1.0f;//By multiplying the divisor by 2, and then subtracting 1, you create the -1 to 1 coordinte space. Directly dividing them will create a 0-1 coordinate space, which opengl doesnt like.
		}
		else {
			return value.relative;
		}
	}

	Coordinate::Coordinate()
	{
		setRelative(0);
	}

	Coordinate::Coordinate(Coordinate & copy)
	{
		isAbsolute = copy.isAbsolute;
		value = copy.value;
	}

	Coordinate::Coordinate(unsigned int absoluteValue)
	{
		setAbsolute(absoluteValue);
	}

	Coordinate::Coordinate(float relativeValue)
	{
		setRelative(relativeValue);
	}

	void Coordinate::set(float relativeValue)
	{
		setRelative(relativeValue);
	}

	void Coordinate::set(unsigned int absoluteValue)
	{
		setAbsolute(absoluteValue);
	}

	void Coordinate::setAbsolute(unsigned int value)
	{
		isAbsolute = true;
		this->value.absolute = value;
	}

	bool Coordinate::isAbsoluteCoordinate()
	{
		return isAbsolute;
	}
}