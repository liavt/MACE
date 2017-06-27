//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#include <mc_core>
#include <mc_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

uniform lowp sampler2D tex;

vec4 mc_frag_main(vec2 textureCoord){		
	vec4 color = texture2D(tex, textureCoord);
	return vec4(mix(mc_Primary.rgb, color.rgb, (1.0f - mc_Primary.a)), color.a);
}


)"