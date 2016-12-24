//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//FRAGMENT SHADER

#version 330 core

out vec4 fragColor;

in lowp vec2 textureCoord;

uniform lowp sampler2D tex;

void main(){
	fragColor = texture(tex, textureCoord);
}

)"