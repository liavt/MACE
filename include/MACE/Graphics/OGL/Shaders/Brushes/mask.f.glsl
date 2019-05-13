R""(
uniform lowp sampler2D tex;
uniform highp sampler2D mask;

vec4 mc_frag_main(void){
	return mcGetForeground(tex) * vec4(1.0f, 1.0f, 1.0f, mcGetMask(mask).r);
}
)""
