R""(

#ifndef _MC_FRAGMENT_LIBRARY_
#define _MC_FRAGMENT_LIBRARY_ 1

#include <mc_core>
#include <mc_entity>

in lowp vec2 mcOutputTextureCoord;

layout(location = MACE__SCENE_ATTACHMENT_INDEX) out lowp vec4 mc_OutColor;
layout(location = MACE__ID_ATTACHMENT_INDEX) out uint mc_OutID;

uniform uint mc_EntityID;

vec4 mc_frag_main(vec2);

void main(void){
	vec4 mc_Color = mc_frag_main(mcOutputTextureCoord);
		
	if(mc_Color.a != 0){
		mc_OutColor.rgb = mc_Color.rgb;
		mc_OutColor.a = mc_Color.a * mc_EntityOpacity;
		
		mc_OutID = mc_EntityID;
	}else{
		discard;
	}
}

#endif
)""
