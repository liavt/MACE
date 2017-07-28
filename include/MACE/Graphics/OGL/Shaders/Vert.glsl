R""(
layout(location = MACE_VAO_DEFAULT_TEXTURE_COORD_LOCATION) in vec2 _mcInputTextureCoord;

out highp vec2 _mcTextureCoord;

vec4 mc_vert_main(vec4);

void main(void){
	_mcTextureCoord = _mcInputTextureCoord;

	gl_Position = mc_vert_main(mcGetEntityPosition());
}
)""
