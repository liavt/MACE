/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/System/Exceptions.h>
#include <iostream>
#include <string>
#include <MACE/System/System.h>

namespace mc {
	void Exception::handleException(const std::exception & e)
	{
		System::requestStop();
		std::cerr << "Error occured: "<<e.what()<<std::endl;
		throw e;
	}
}