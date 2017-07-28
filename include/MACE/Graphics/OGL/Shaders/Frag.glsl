R""(
in highp vec2 _mcTextureCoord;

layout(location = MACE_SCENE_ATTACHMENT_INDEX) out lowp vec4 _mc_OutColor;
layout(location = MACE_ID_ATTACHMENT_INDEX) out uint _mc_OutID;

uniform uint mc_EntityID;

vec4 _mcGetTexture(const in sampler2D mc_Sampler, const in _mc_TextureAttachment mc_Tex){
	vec4 mc_Fragment = texture(mc_Sampler, _mcTextureCoord * mc_Tex.mc_TextureTransform.wz + mc_Tex.mc_TextureTransform.xy);
	
	return vec4(mix(mc_Tex.mc_Color.rgb, mc_Fragment.rgb, 1.0f - mc_Tex.mc_Color.a), mc_Fragment.a);;
}

vec4 mcGetForeground(const in sampler2D mc_Samp){
	return _mcGetTexture(mc_Samp, mc_Foreground);
}

vec4 mcGetBackground(const in sampler2D mc_Samp){
	return _mcGetTexture(mc_Samp, mc_Background);
}

vec4 mcGetMask(const in sampler2D mc_Samp){
	return _mcGetTexture(mc_Samp, mc_Mask);
}

vec4 mc_frag_main(void);

void main(void){
	vec4 mc_Fragment = mc_frag_main();
		
	if(mc_Fragment.a != 0){
		_mc_OutColor = mc_Fragment * _mc_Filter;
		
		_mc_OutID = mc_EntityID;
	}else{
		discard;
	}
}
)""
