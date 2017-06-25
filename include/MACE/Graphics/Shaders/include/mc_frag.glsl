R"(

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
		
	mc_Color.a *= mc_EntityOpacity;
	
	if(mc_Color.a == 0.0){
		discard;
	}
		
	mc_OutColor = mc_Color;
	
	mc_OutID = mc_EntityID;
}

#endif
)"
