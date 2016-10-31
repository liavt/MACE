//this file is for rendering the final buffer to the screen
R"(

//VERTEX SHADER

#version 330 core

layout(location = 0) in vec3 vertCoord;
//layout(location = 1) in vec2 texCoord;

//out lowp vec2 textureCoord;

void main(void){
	
	gl_Position = vec4(vertCoord,0.0);
	
	//textureCoord=texCoord;
}

)"