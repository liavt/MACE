R""(
uniform lowp sampler2D tex;
uniform mediump sampler2D glyph;

vec4 mc_frag_main(void){
	vec3 glyph = mcGetBackground(glyph).rgb;
	float alpha = dot(glyph, vec3(1.0)) / 3.0;
	return vec4(mix(glyph, mcGetForeground(tex).rgb, alpha), alpha);
}
)""
