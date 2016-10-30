/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MACE/System/Utility/Matrix.h>
#include <unordered_map>
#include <GL/glew.h>


namespace mc {
namespace gfx {

/**
@todo make a shader class
*/
class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void init();
	void destroy();

	void bind() const;
	void unbind() const;

	void createFragment(const char shader[]);
	void createFragment(const std::string& shader);
	void createVertex(const char shader[]);
	void createVertex(const std::string& shader);

	void createUniform(const std::string& name);
	void createUniform(const char* name);

	int getUniformLocation(const std::string& name);
	int getUniformLocation(const char* name);

	int getProgramID() const;
	int getVertexID() const;
	int getFragmentID() const;

	//overloading setUniform

	//overloading every single possible combination of values is a pain. so lets use an x-macro instead!

	//this macro generates setUniform with raw arguments
#define _UNIFORM_RAW(T,suffix)																								\
	void setUniform(char * name, const T a){glUniform1##suffix(uniforms[name],a);}											\
	void setUniform(char * name, const T a, const T b){glUniform2##suffix(uniforms[name],a,b);}							\
	void setUniform(char * name, const T a, const T b, const T c){glUniform3##suffix(uniforms[name],a,b,c);}				\
	void setUniform(char * name, const T a, const T b, const T c, const T d){glUniform4##suffix(uniforms[name],a,b,c,d);}

	//this macro generates setUniform for use with arrays
#define _UNIFORM_ARRAY(T,suffix)																							\
	void setUniform(char * name, const Size arraySize,const T* a){glUniform1##suffix(uniforms[name],arraySize,a);}		\
	void setUniform(char * name, const Size componentSize, const Size arraySize, const T* a){ if(componentSize==1) glUniform1##suffix(uniforms[name],arraySize,a);else if(componentSize==2) glUniform2##suffix(uniforms[name],arraySize,a);else if(componentSize==3) glUniform3##suffix(uniforms[name],arraySize,a);else if(componentSize==4) glUniform4##suffix(uniforms[name],arraySize,a);	}

	//this macro generates setUniform for use with the Vector class
#define _UNIFORM_VECTOR(T,suffix) \
	void setUniform(char * name, const mc::Vector<T, 1> v) {glUniform1##suffix(uniforms[name],v[0]);}						\
	void setUniform(char * name, const mc::Vector<T, 2> v) {glUniform2##suffix(uniforms[name],v[0],v[1]);}				\
	void setUniform(char * name, const mc::Vector<T, 3> v) {glUniform3##suffix(uniforms[name],v[0],v[1],v[2]);}			\
	void setUniform(char * name, const mc::Vector<T, 4> v) {glUniform4##suffix(uniforms[name],v[0],v[1],v[2],v[3]);}		\

#define _CREATE_UNIFORM_MATRIX_FUNCTION(T,suffix,W,H) \
	void setUniform(char * name, const bool transpose, const mc::Matrix<T, W, H>& m) { T flattenedData[W*H];glUniformMatrix##W##x##H##suffix(uniforms[name], 1, transpose, m.flatten(flattenedData)); } \
	void setUniform(char * name, const mc::Matrix<T,W,H>& m){setUniform(name,true,m);}

#define _CREATE_UNIFORM_SQUARE_MATRIX_FUNCTION(T,suffix,N) \
	void setUniform(char * name, const bool transpose, const mc::Matrix<T, N, N>& m) { T flattenedData[N*N];glUniformMatrix##N##suffix(uniforms[name], 1, transpose, m.flatten(flattenedData)); } \
	void setUniform(char * name, const mc::Matrix<T,N,N>& m){setUniform(name,true,m);}

#define _UNIFORM_MATRIX(T, suffix) \
	_CREATE_UNIFORM_SQUARE_MATRIX_FUNCTION(T,suffix,2);	\
	_CREATE_UNIFORM_SQUARE_MATRIX_FUNCTION(T,suffix,3);	\
	_CREATE_UNIFORM_SQUARE_MATRIX_FUNCTION(T,suffix,4);	\
	_CREATE_UNIFORM_MATRIX_FUNCTION(T,suffix,2,3);			\
	_CREATE_UNIFORM_MATRIX_FUNCTION(T,suffix,3,2);			\
	_CREATE_UNIFORM_MATRIX_FUNCTION(T,suffix,2,4);			\
	_CREATE_UNIFORM_MATRIX_FUNCTION(T,suffix,3,4);			\
	_CREATE_UNIFORM_MATRIX_FUNCTION(T,suffix,4,3);			\

	//this macro combines all of the above macros
#define _GENERATE_UNIFORM_SETTERS(T,suffix)																				\
	_UNIFORM_RAW(T, suffix);																								\
	_UNIFORM_ARRAY(T, suffix ## v);																						\
	_UNIFORM_VECTOR(T,suffix);

	/*
	So what is going on here? I am using something called an X-Macro, a technique to generate large amounts of code without copying andpasting.

	Basically, the preproccessor has defined the macro _UNIFORM_RAW, and it takes in a typename and a suffix for a glUniform* call.

	So _UNIFORM_RAW(float,f) would generate this:

	void setUniform(char name[], float a){glUniform1f(uniforms[name],a}
	...

	Instead of having to copy and paste this for every type, the preproccessor does all this work for us, and its easier to modify and fix bugs.

	Without this x-macro, there would need to be 74 different methods, all overloading eachother. here, we only have to modify 14, and we save lspace
	*/

	//you cant send matrices of int to a shader, so we only generate matrices of float and double
	/**
	Set a uniform in this `ShaderProgram` to a value.
	*/
	_UNIFORM_MATRIX(float, fv);
	_UNIFORM_MATRIX(double, dv);

	_GENERATE_UNIFORM_SETTERS(float, f);
	_GENERATE_UNIFORM_SETTERS(double, d);
	_GENERATE_UNIFORM_SETTERS(int, i);
	_GENERATE_UNIFORM_SETTERS(unsigned int, ui);

	/*
	Of course, we dont want the user to have their code messed up by the x-macro, so we undefine it after we use it.
	*/
#undef _GENERATE_UNIFORM_SETTERS
#undef _UNIFORM_RAW
#undef _UNIFORM_ARRAY
#undef _UNIFORM_VECTOR
#undef _UNIFORM_MATRIX
#undef _CREATE_UNIFORM_MATRIX_FUNCTION
#undef _CREATE_UNIFORM_SQUARE_MATRIX_FUNCTION

private:
	int id = -1;
	int fragId = -1, vertId = -1;

	std::unordered_map<std::string, int> uniforms = std::unordered_map<std::string, int>();

	int createShader(const char code[], const GLenum& type);

	void createProgram();

};//ShaderProgram

}
}
