//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#version 330 core

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

layout(std140) uniform ssl_PaintData{
	vec4 ssl_Paint;
	float ssl_Opacity;
};

vec4 sslGetEntityColor(vec4 ssl_Tex){
	vec4 ssl_EntityColor= mix(vec4(ssl_Paint.rgb,1.0),ssl_Tex,ssl_Paint.a);
	ssl_EntityColor.a=ssl_Opacity;
	return ssl_EntityColor;
}

out lowp vec4 color;

uniform lowp sampler2D tex;

void main (void)  
{     
	color = sslGetEntityColor(texture(tex,textureCoord));
}       

)"