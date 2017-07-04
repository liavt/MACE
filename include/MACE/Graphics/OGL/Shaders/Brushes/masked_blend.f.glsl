R""(

//FRAGMENT SHADER

#include <mc_core>
#include <mc_frag>

uniform lowp sampler2D tex1;
uniform lowp sampler2D tex2;
uniform lowp sampler2D mask;

vec4 mc_frag_main(vec2 textureCoord){
	vec4 maskFragment = texture2D(mask, textureCoord);
	vec4 foreground = texture2D(tex1, textureCoord);
	vec4 background = texture2D(tex2, textureCoord);
	if(maskFragment.r >= mc_Data[0] && maskFragment.r <= mc_Data[1]){
		return vec4(mix(mc_PrimaryColor.rgb, foreground.rgb, (1.0f - mc_PrimaryColor.a)), foreground.a * maskFragment.a);
	} else{
		return vec4(mix(mc_SecondaryColor.rgb, background.rgb, (1.0f - mc_SecondaryColor.a)), background.a * maskFragment.a);
	}
}


)""
