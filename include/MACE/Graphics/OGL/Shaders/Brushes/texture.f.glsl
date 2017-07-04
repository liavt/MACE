//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R""(

//FRAGMENT SHADER

#include <mc_core>
#include <mc_frag>

uniform lowp sampler2D tex;

vec4 mc_frag_main(vec2 textureCoord){		
	vec4 color = texture2D(tex, textureCoord);
	return vec4(mix(mc_PrimaryColor.rgb, color.rgb, 1.0f - mc_PrimaryColor.a), color.a);
}


)""
