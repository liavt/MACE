R""(
uniform lowp sampler2D tex1;
uniform lowp sampler2D tex2;

vec4 mc_frag_main(void){
	return mix(mcGetForeground(tex1), mcGetBackground(tex2), mc_Data[0]);
}
)""
