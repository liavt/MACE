//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

#version 330 core

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

uniform mediump mat4 rotation;
uniform mediump vec3 translation;
uniform mediump vec3 scale;

uniform mediump mat4 inheritedRotation;
uniform mediump vec3 inheritedTranslation;
uniform mediump vec3 inheritedScale;

uniform mediump mat4 ortho;

out lowp vec2 textureCoord;

void main(){
	//we do scale first, then rotate, then translate.
	gl_Position = ortho*(vec4(translation,1.0)+(rotation * vec4(scale * vertexPosition,0.0)));//MAD operations right here!
	//applying the parent properties now
	gl_Position = vec4(inheritedTranslation,1.0)+((inheritedRotation * (gl_Position * vec4(inheritedScale,0.0))));//what a MAD man!
	
	gl_Position.xy -= 1.0f;//set origin to bottom left
	
	/*
	For those who didn't get the MAD joke:
	MAD means multipy, add, then divide. Doing math in that sequence will be done in 1 gpu cycle instead of 3, which saves a lot of processing power.  Thats what we are doing up there.
	*/
	
	textureCoord=texCoord;
}

)"