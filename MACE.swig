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