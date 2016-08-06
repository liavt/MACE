#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

uniform mat4 transform;

out vec2 textureCoord;

void main(){
	gl_Position.xyz = vertexPosition;
	gl_Position.w = 1.0;
	gl_Position*=transform;
	
	textureCoord=texCoord;
}