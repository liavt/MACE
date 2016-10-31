//this file is for rendering the final buffer to the screen
R"(

//FRAGMENT SHADER

#version 330 core

//in lowp vec2 textureCoord;

out lowp vec4 color;

//uniform lowp sampler2D tex;

void main(void){
	
	//color = texture(tex,textureCoord);
	
	color = vec4(1,0,0,1);
}

)"