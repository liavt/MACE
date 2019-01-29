R""(
uniform lowp sampler2D tex;
uniform highp sampler2D mask;

vec4 mc_frag_main(void){
	vec4 fragment = mcGetForeground(tex);
	return vec4(fragment.rgb, fragment.a * mcGetMask(mask).r);
}
)""
