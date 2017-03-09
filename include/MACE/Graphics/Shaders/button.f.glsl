//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#include <ssl_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

uniform lowp sampler2D backgroundTexture;
uniform lowp sampler2D hoverTexture;
uniform lowp sampler2D clickedTexture;
uniform lowp sampler2D disabledTexture;

sslUniformBuffer buttonData{
	sslAttachmentData backgroundData;
	sslAttachmentData hoveredData;
	sslAttachmentData clickedData;
	sslAttachmentData disabledData;
	float hovered
	float clicked;
	float disabled;
};

vec4 ssl_frag_main()  
{  	
	vec4 color = sslAttachmentBlend(backgroundData, texture(backgroundTexture, textureCoord));
	
	if(disabled == 1){
		vec4 disabledColor = sslAttachmentBlend(disabledData, texture(disabledTexture, textureCoord));
				
		return vec4(mix(color.rgb, disabledColor.rgb, disabledColor.a), color.a);
	}
	
	if(hovered == 1){
		vec4 hoverColor = sslAttachmentBlend(hoveredData, texture(hoverTexture, textureCoord));
		
		color = vec4(mix(color.rgb, hoverColor.rgb, hoverColor.a), color.a);
	}
	
	if(clicked == 1){
		vec4 clickColor = sslAttachmentBlend(clickedData, texture(clickedTexture, textureCoord));
		
		color = vec4(mix(color.rgb, clickedColor.rgb, clickedColor.a), color.a);
	}

	return color;
}       

)"
