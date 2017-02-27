/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_CORE_CONFIGURE_H
#define MACE_CORE_CONFIGURE_H

#define MACE_INCLUDED 1

/*Meta stuff*/
#define MACE_TESTS 1
#define MACE_DEMOS 1

#define MACE_INCLUDES D:/Workspace/MACE/include

#define MACE_DIRECTORY D:/Workspace/MACE

#define MACE_SYSTEM_NAME Windows
#define MACE_SYSTEM_VERSION 10.0.10586
#define MACE_PROCESSOR_NAME AMD64

/*Libraries*/
/* #undef MACE_OPENCV */

/* #undef MACE_X11 */

/* #undef MACE_POSIX */

/*Operating System*/
/* #undef MACE_OSX */
#define MACE_WINDOWS 1
/* #undef MACE_UNIX */

/* #undef MACE_BIG_ENDIAN */

/* #undef MACE_SHARED_LIBS */
#define MACE_STATIC_LIBS 1

#define MACE_POINTER_SIZE 4

#define MACE_UNSIGNED_CHAR_SIZE 1
#define MACE_CHAR_SIZE 1
#define MACE_UNSIGNED_INT_SIZE 4
#define MACE_INT_SIZE 4
#define MACE_SHORT_INT_SIZE 2
#define MACE_LONG_INT_SIZE 4
#define MACE_LONG_LONG_INT_SIZE 8
#define MACE_FLOAT_SIZE 4
#define MACE_DOUBLE_SIZE 8
#define MACE_BOOL_SIZE 1

/*Compilers*/
/* #undef MACE_BORLAND */

#define MACE_MSVC 1
/* #undef MACE_MSVC_CL */
/* #undef MACE_MSVC_2005 */
#define MACE_MSVC_IDE 1

/* #undef MACE_CYGWIN */
/* #undef MACE_CLANG */
/* #undef MACE_GNU */
/* #undef MACE_HP */
/* #undef MACE_INTEL */
/* #undef MACE_ORACLE */
/* #undef MACE_TEXAS_INSTRUMENTS */
/* #undef MACE_WATCOM */
/* #undef MACE_ADSP */
/* #undef MACE_CRAY */
/* #undef MACE_MIPSPRO */
/* #undef MACE_PORTLAND */
/* #undef MACE_PATHSCALE */
/* #undef MACE_IBM */

#endif//MACE_CORE_CONFIGURE_H
