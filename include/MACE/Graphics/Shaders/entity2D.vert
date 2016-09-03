//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

#version 330 core

precision mediump float; // Defines precision for float and float-derived (vector/matrix) types.

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(std140) uniform entity_data{
	vec3 translation;
	vec3 scale;
	mat4 rotation;
	vec3 inheritedTranslation;
	vec3 inheritedScale;
	mat4 inheritedRotation;
};

out lowp vec2 textureCoord;

void main(){
	//we do scale first, then rotate, then translate.
	gl_Position = (vec4(translation,1.0)+(rotation * (vec4(scale*vertexPosition,0.0))));//MAD operations right here!
	
	gl_Position.xy -= 0.5f;//because we moved the origin to be the bottom left, we need to move it back to the center for rotation
	
	//applying the parent properties now
	gl_Position = (vec4(inheritedTranslation,1.0)+((inheritedRotation * (gl_Position * vec4(inheritedScale,0.0)))));//what a MAD man!
	
//	gl_Position.xy += 0.5f;
	
	gl_Position = ((gl_Position)*vec4(2.0f,2.0f,1.0f,1.0f))+vec4(-0.5f,-0.5f,0,0);//we need to convert it to be 0.0 to 1.0 coordinates and the origin to be the bottom left.
	
	gl_Position += vec4(scale/2,0.0f);

	
	//gl_Position += vec4(scale/2.0f,0.0f);
	
	//gl_Position = ortho*gl_Position;
	
	/*
	For those who didn't get the MAD joke:
	MAD means multipy, add, then divide. Doing math in that sequence will be done in 1 gpu cycle instead of 3, which saves a lot of processing power.  Thats what we are doing up there.
	*/
	
	textureCoord=texCoord;
}

)"