R""(
layout(location = MACE_SCENE_ATTACHMENT_INDEX, index = 1) out lowp vec4 glyphColor;

uniform lowp sampler2D tex;
uniform mediump sampler2D glyph;

vec4 mc_frag_main(void){
	vec4 foreground = mcGetForeground(tex);
	glyphColor = mcGetBackground(glyph) * foreground.aaaa;
	return vec4(foreground.rgb, 1.0);
}
)""
