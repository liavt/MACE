#pragma once
#include <MC-System/Utility/Vector.h>

namespace mc {
	//why 4 values? well, for a 3D positionm you need ot have 4 values. (x,y,z,w) The w is for multiplying it by a projection matrix.
	class Position : Vector4f{
		using Vector4f::Vector4f;
	};
}