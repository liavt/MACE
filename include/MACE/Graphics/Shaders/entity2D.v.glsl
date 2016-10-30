//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//VERTEX SHADER

#version 330 core

#include <ssl_vertex>

precision mediump float; // Defines precision for float and float-derived (vector/matrix) types.

layout(location = 1) in vec2 texCoord;

out lowp vec2 textureCoord;

void main(void){
	
	gl_Position = sslGetEntityPosition();
	
	textureCoord=texCoord;
}

)"