/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

%module MACE

%{
	#define MACE_SWIG
	#include <MACE/MACE.h>
%}

%rename(add) operator+
%rename(subtract) operator-
%rename(multiply) operator*
%rename(divide) operator/
%rename(modulo) operator%
%rename(increment) operator++
%rename(decrement) operator--

%rename(equals) operator==
%rename(notEquals) operator!=
%rename(greaterThan) operator>
%rename(lessThan) operator<
%rename(greaterThanOrEquals) operator>=
%rename(lessThanOrEquals) operator<=

%rename(not) operator!
%rename(and) operator&&
%rename(or) operator||

%rename(binaryAnd) operator&
%rename(binaryOr) operator|
%rename(binaryXor) operator^
%rename(binaryComplement) operator~
%rename(binaryLeftShift) operator<<
%rename(binaryRightShift) operator>>

%include <MACE/MACE.h>