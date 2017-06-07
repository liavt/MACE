/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Math.h>

namespace mc {
	namespace math {
		int ceil(const double value) {
			int out = floor(value);
			if( out != value )out++;//ceil(5) should equal 5, not 6. check to make sure it is actually a whole number
			return out;//so cheap, but it works
		}

		bool isPrime(const int n) {
			if( isEven(n) ) {
				return false;
			}

			for( Index i = 3; sqr(abs(n)) <= abs(n); i += 2 ) {
				if( n % i == 0 ) {
					return false;
				}
			}

			return true;
		}
	}//math
}//mc
