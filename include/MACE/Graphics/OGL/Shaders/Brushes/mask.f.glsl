R""(

//FRAGMENT SHADER

#include <mc_core>
#include <mc_frag>

uniform lowp sampler2D tex;
uniform lowp sampler2D mask;

vec4 mc_frag_main(vec2 textureCoord){
	vec4 color = texture2D(tex, textureCoord);
	return vec4(mix(mc_PrimaryColor.rgb, color.rgb, 1.0f - mc_PrimaryColor.a), color.a * texture2D(mask, textureCoord).r);
}


)""
