R""(

//FRAGMENT SHADER

#include <mc_frag>

uniform lowp sampler2D tex1;
uniform lowp sampler2D tex2;

vec4 mc_frag_main(void){
	vec4 foreground = mcGetForeground(tex1);
	vec4 background = mcGetBackground(tex2);
	return mix(foreground, background, mc_Data[0]);
}


)""
