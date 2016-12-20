//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#version 330 core

#include <ssl_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

layout(location = 0) out lowp vec4 color;

uniform lowp sampler2D tex;

void main (void)  
{     
	color = sslGetEntityColor(texture(tex,textureCoord));//
}       

)"