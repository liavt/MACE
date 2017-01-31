//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#include <ssl_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

uniform lowp sampler2D backgroundTexture;
uniform lowp sampler2D foregroundTexture;
uniform lowp sampler2D selectionTexture;

sslUniformBuffer textureData{
	sslAttachmentData selectionData;
	sslAttachmentData foregroundData;
	sslAttachmentData backgroundData;
	float progress;
};

vec4 ssl_frag_main()  
{  	
	vec4 selection =  sslAttachmentBlend(selectionData, texture(selectionTexture, textureCoord));
	
	if( progress >= selection.r ){
		vec4 foreground = sslAttachmentBlend(foregroundData, texture(foregroundTexture, textureCoord));
	
		//return vec4(foreground.rgb, foreground.a * selection.a);
	}
	
	return sslAttachmentBlend(backgroundData, texture(backgroundTexture, textureCoord));
}       

)"
