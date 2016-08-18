//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

#version 330 core

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

out lowp vec4 color;

uniform lowp vec4 paint;
uniform lowp float opacity;

uniform lowp sampler2D tex;

void main (void)  
{     
	color= mix(vec4(paint.rgb,1.0),texture(tex,textureCoord),paint.a);
	color.a=opacity;
}       

)"