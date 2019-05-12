R""(
layout(location = MACE_SCENE_ATTACHMENT_INDEX, index = 1) out lowp vec4 blendColor;

uniform lowp sampler2D tex1;
uniform mediump sampler2D tex2;

vec4 mc_frag_main(void){
	vec4 foreground = mcGetForeground(tex1);
	blendColor = mcGetBackground(tex2) * foreground.aaaa;
	return vec4(foreground.rgb, 1.0);
}
)""
