R"(

#ifndef _SSL_FRAGMENT_LIBRARY_
#define _SSL_FRAGMENT_LIBRARY_ 1

#include <ssl_core>
#include <ssl_entity>

layout(location = 0) out lowp vec4 ssl_OutColor;
layout(location = 1) out uint ssl_OutID;

uniform uint ssl_EntityID;

vec4 ssl_frag_main(void);

void main(void){
	vec4 ssl_Color = ssl_frag_main();

	ssl_Color.a *= ssl_EntityOpacity;
	
	if(ssl_Color.a == 0.0){
		discard;
	}
	
//	ssl_Color.rgb = mix(ssl_EntityPaint.rgb, ssl_Color.rgb, 1.0 - ssl_EntityPaint.a);
	
	ssl_OutColor = ssl_Color;
	
	ssl_OutID = ssl_EntityID;
}

#endif
)"
