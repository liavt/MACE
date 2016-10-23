//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R"(

//VERTEX SHADER

#version 330 core


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(std140) uniform window_data{
	vec2 originalSize;
	vec2 currentSize;
	vec2 mouseCoord;
};

layout(std140) uniform entity_data{
	vec3 translation;
	float stretch_x;
	vec3 scale;
	float stretch_y;
	vec3 inheritedTranslation;
	vec3 inheritedScale;
	mat4 rotation;
	mat4 inheritedRotation;
};

vec4 getEntityPosition(){
	//we do scale first, then rotate, then translate.
	vec4 position = (vec4(translation,1.0)+(rotation * (vec4(scale*vertexPosition,0.0))));//MAD operations right here!
	
	position.xy -= 0.5f;//because we moved the origin to be the bottom left, we need to move it back to the center for rotation
	
	//applying the parent properties now
	position = (vec4(inheritedTranslation,1.0)+((inheritedRotation * (position * vec4(inheritedScale,0.0)))));//what a MAD man!
		
	position = ((position)*vec4(2.0f,2.0f,1.0f,1.0f))+vec4(-0.5f,-0.5f,0,0);//we need to convert it to be 0.0 to 1.0 coordinates and the origin to be the bottom left.
	
	position += vec4(scale/2,0.0f);
	
	vec2 sizeModifier = originalSize/currentSize;
	
	if(stretch_x==0){
		position.x*=sizeModifier.x;
	}
	if(stretch_y==0){
		position.y*=sizeModifier.y;
	}
	
	/*
	For those who didn't get the MAD joke:
	MAD means multipy, add, then divide. Doing math in that sequence will be done in 1 gpu cycle instead of 3, which saves a lot of processing power.  Thats what we are doing up there.
	*/
	
	return position;
}

precision mediump float; // Defines precision for float and float-derived (vector/matrix) types.

out lowp vec2 textureCoord;

void main(){
	
	gl_Position = getEntityPosition();
	
	gl_Position.x += mouseCoord.x/currentSize.x;
	gl_Position.y += (currentSize.y-mouseCoord.y)/currentSize.y;
	
	textureCoord=texCoord;
}

)"