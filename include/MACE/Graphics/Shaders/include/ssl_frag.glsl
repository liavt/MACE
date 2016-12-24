R"(

#ifndef _SSL_FRAGMENT_LIBRARY_
#define _SSL_FRAGMENT_LIBRARY_
layout(location = 0) out lowp vec4 color;
layout(location = 1) out uint ssl_ID;

uniform float ssl_EntityID;

vec4 ssl_frag_main(void);

void main(void){
	color = ssl_frag_main();
	
	if(color.a == 0.0){
		discard;
	}
	
	ssl_ID = uint(ssl_EntityID);
}

#endif
)"