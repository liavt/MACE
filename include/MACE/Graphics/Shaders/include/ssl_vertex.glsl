R"(

#ifndef _SSL_VERTEX_LIBRARY_
#define _SSL_VERTEX_LIBRARY_ 1

#include <ssl_core>
#include <ssl_position>

vec4 ssl_vert_main(vec4);

void main(void){
	gl_Position = ssl_vert_main(sslGetEntityPosition());
}

#endif

)"
