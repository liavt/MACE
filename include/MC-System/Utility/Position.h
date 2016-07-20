#pragma once
#include <MC-System/Constants.h>

namespace mc {
	
	/**
	Stores a coordinate on the screen. Can store a relative position (a `float` from -1 to 1, where 0 is the center of the screen.) Can also store an absolute position (an 'unsigned int' that is divided by the screen's size.)

	@see Coordinate#getAsAbsoluteCoordinate()
	*/
	struct Coordinate {
		Coordinate();
		Coordinate(Coordinate& copy);
		Coordinate(unsigned int absoluteValue);
		Coordinate(float relativeValue);

		void set(float relativeValue);
		void set(unsigned int absoluteValue);
		void setAbsolute(unsigned int absoluteValue);
		void setRelative(float relativeValue);

		const float get(Size right) const;

		bool isAbsoluteCoordinate();

	private:
		union {
			float relative;
			unsigned int absolute;
		} value;

		bool isAbsolute;
	};

}