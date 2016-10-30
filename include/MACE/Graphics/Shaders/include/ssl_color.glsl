R"(

#ifndef SSL_COLOR_LIBRARY
#define SSL_COLOR_LIBRARY

layout(std140) uniform ssl_PaintData{
	vec4 ssl_Paint;
	float ssl_Opacity;
	float ssl_EntityID;
};

vec4 sslGetEntityColor(vec4 ssl_Tex){
	vec4 ssl_EntityColor= mix(vec4(ssl_Paint.rgb,1.0),ssl_Tex,ssl_Paint.a);
	ssl_EntityColor.a=ssl_Opacity;
	return ssl_EntityColor;
}


#endif

)"