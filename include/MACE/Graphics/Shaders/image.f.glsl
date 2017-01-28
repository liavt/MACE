//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#include <ssl_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

uniform lowp sampler2D tex;

sslUniformBuffer textureData{
	sslAttachmentData image;
};

vec4 ssl_frag_main()  
{    
	return sslAttachmentBlend(image, texture(tex, textureCoord));
}       

)"
