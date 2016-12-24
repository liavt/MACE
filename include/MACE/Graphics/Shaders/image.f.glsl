//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#version 330 core

#include <ssl_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

layout(std140) uniform textureData{
	vec4 paint;
	float opacity;
};

in lowp vec2 textureCoord;

uniform lowp sampler2D tex;

vec4 ssl_frag_main()  
{    
	vec4 textureColor = texture(tex,textureCoord);
	textureColor.a *= opacity;
	
	if(textureColor.a > 0.0){ 
		vec4 entityColor= mix(vec4(paint.rgb, 1.0), textureColor, 1.0 - paint.a);
		return entityColor;
	}
	
	//if its transparent, return the color
	return textureColor;
}       

)"