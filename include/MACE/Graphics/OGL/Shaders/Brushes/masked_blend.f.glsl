R""(

//FRAGMENT SHADER

#include <mc_frag>

uniform lowp sampler2D tex1;
uniform lowp sampler2D tex2;
uniform highp sampler2D mask;

vec4 mc_frag_main(void){
	vec4 maskFragment = mcGetMask(mask);
	vec4 foreground = mcGetForeground(tex1);
	vec4 background = mcGetBackground(tex2);
	if(maskFragment.r >= mc_Data[0] && maskFragment.r <= mc_Data[1]){
		return vec4(foreground.rgb, foreground.a * maskFragment.a);
	} else{
		return vec4(background.rgb, background.a * maskFragment.a);
	}
}


)""
