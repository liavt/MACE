R""(

//FRAGMENT SHADER

#include <mc_core>
#include <mc_frag>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

uniform lowp sampler2D tex;
uniform lowp sampler2D mask;

vec4 mc_frag_main(vec2 textureCoord){
	vec4 maskFragment = texture2D(mask, textureCoord);
	if(maskFragment.r >= mc_Secondary[0] && maskFragment.r <= mc_Secondary[1]){
		vec4 color = texture2D(tex, textureCoord);
		return vec4(mix(mc_Primary.rgb, color.rgb, (1.0f - mc_Primary.a)), color.a * maskFragment.a);
	} else{
		return vec4(0,0,0,0);
	}
}


)""
