R"(
#ifndef _MC_VERTEX_LIBRARY_
#define _MC_VERTEX_LIBRARY_ 1

#include <mc_core>
#include <mc_position>

layout(location = MACE__VAO_TEX_COORD_LOCATION) in vec2 mcInputTextureCoord;

out lowp vec2 mcOutputTextureCoord;

vec4 mc_vert_main(vec4);

void main(void){
	mcOutputTextureCoord = mcInputTextureCoord;

	gl_Position = mc_vert_main(mcGetEntityPosition());
}

#endif
)"
