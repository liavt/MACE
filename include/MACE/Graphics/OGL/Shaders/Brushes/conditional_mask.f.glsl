R""(
uniform lowp sampler2D tex1;
uniform lowp sampler2D tex2;
uniform highp sampler2D mask;

vec4 mc_frag_main(void){
	vec4 maskFragment = mcGetMask(mask);
	return mix(mcGetBackground(tex2), mcGetForeground(tex1), maskFragment.r >= mc_Data[0] && maskFragment.r <= mc_Data[1]) * vec4(1.0f, 1.0f, 1.0f, maskFragment.a);
}
)""
