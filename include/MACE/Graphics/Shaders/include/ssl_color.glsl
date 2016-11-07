R"(

#ifndef _SSL_COLOR_LIBRARY_
#define _SSL_COLOR_LIBRARY_

layout(location = 1) out uint ssl_ID;

layout(std140) uniform ssl_PaintData{
	vec4 ssl_Paint;
	float ssl_Opacity;
	float ssl_EntityID;
};

vec4 sslGetEntityColor(vec4 ssl_Tex){
	vec4 ssl_EntityColor= mix(vec4(ssl_Paint.rgb,1.0),ssl_Tex,ssl_Paint.a);
	ssl_EntityColor.a=ssl_Opacity;
	ssl_ID = uint(ssl_EntityID);
	return ssl_EntityColor;
}


#endif

)"