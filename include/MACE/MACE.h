/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_H
#define MACE_H

/**
Namespace for everything in MACE. This includes constants, typedefs, tests, classes, and variables.
<p>
Preproccessor definitions will start with `MACE_` and `MACE_INCLUDED` will be defined to equal `true` if MACE is included.
@todo Add a proper logger and error handler
*/
namespace mc {}

#include <MACE/Core/Core.h>
#include <MACE/Utility/Utility.h>
#include <MACE/Graphics/Graphics.h>

#endif