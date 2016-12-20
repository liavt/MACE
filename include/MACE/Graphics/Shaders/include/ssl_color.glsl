R"(

#ifndef _SSL_COLOR_LIBRARY_
#define _SSL_COLOR_LIBRARY_

layout(location = 1) out uint ssl_ID;

uniform float ssl_EntityID;

in float ssl_Opacity;
in vec4 ssl_Paint;

vec4 sslGetEntityColor(vec4 ssl_Tex){
	if(ssl_Tex.a > 0.0){
		vec4 ssl_EntityColor= mix(vec4(ssl_Paint.rgb,1.0),ssl_Tex,ssl_Paint.a);
		ssl_EntityColor.a*=ssl_Opacity;
		ssl_ID = uint(ssl_EntityID);
		return ssl_EntityColor;
	}else{
		ssl_ID = uint(0);
		return ssl_Tex;
	}
}


#endif

)"