//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R""(

//FRAGMENT SHADER

#include <mc_frag>

uniform lowp sampler2D tex;

vec4 mc_frag_main(void){		
	return mcGetForeground(tex);
}


)""
