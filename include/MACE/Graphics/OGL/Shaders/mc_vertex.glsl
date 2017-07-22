R""(
#ifndef _MACE_VERTEX_LIBRARY_
#define _MACE_VERTEX_LIBRARY_ 1

#include <mc_core>
#include <mc_position>

layout(location = MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION) in vec2 _mcInputTextureCoord;

out highp vec2 _mcTextureCoord;

vec4 mc_vert_main(vec4);

void main(void){
	_mcTextureCoord = _mcInputTextureCoord;

	gl_Position = mc_vert_main(mcGetEntityPosition());
}

#endif
)""
