R""(
#ifdef MACE_TEXTURE
layout(location = MACE_VAO_DEFAULT_TEXTURE_COORD_LOCATION) in vec2 _mcInputTextureCoord;

out highp vec2 _mcTextureCoord;
#endif

vec4 mc_vert_main(vec4);

void main(void){
#ifdef MACE_TEXTURE
	_mcTextureCoord = _mcInputTextureCoord;
#endif

	gl_Position = mc_vert_main(mcGetEntityPosition());
}
)""
