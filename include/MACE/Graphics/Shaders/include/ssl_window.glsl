R"(

#ifndef _SSL_WINDOW_LIBRARY_
#define _SSL_WINDOW_LIBRARY_ 1

#include <ssl_core>

layout(std140) uniform ssl_WindowData{
	vec2 ssl_WindowRatios;
};

#endif

)"