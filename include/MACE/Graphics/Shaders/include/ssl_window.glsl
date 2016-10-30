R"(

#ifndef SSL_WINDOW_LIBRARY
#define SSL_WINDOW_LIBRARY

layout(std140) uniform ssl_WindowData{
	vec2 ssl_OriginalSize;
	vec2 ssl_CurrentSize;
	vec2 ssl_MouseCoord;
};

#endif

)"