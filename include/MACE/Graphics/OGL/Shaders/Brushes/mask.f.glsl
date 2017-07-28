R""(
uniform lowp sampler2D tex;
uniform highp sampler2D mask;

vec4 mc_frag_main(void){
	vec4 fragment = mcGetForeground(tex);
	vec4 mask = mcGetMask(mask);
	return vec4(fragment.rgb, fragment.a * mask.r);
}
)""
