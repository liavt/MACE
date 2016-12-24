R"(
#ifndef _SSL_POSITION_LIBRARY_
#define _SSL_POSITION_LIBRARY_

#include <ssl_window>

struct ssl_EntityDataStruct{
	vec3 ssl_Translation;
	float ssl_StretchX;
	vec3 ssl_Scale;
	float ssl_StretchY;
	vec3 ssl_Rotation;
};

layout(std140) uniform ssl_BaseEntityBuffer{
	ssl_EntityDataStruct ssl_BaseEntity;
	ssl_EntityDataStruct ssl_ParentEntity;
};

layout(location = 15) in vec3 ssl_VertexPosition;

mat4 sslCreateRotationMatrix(vec3 ssl_RotationInput){
	float ssl_CosZ = cos(ssl_RotationInput.z), ssl_SinZ = sin(ssl_RotationInput.z),
		   ssl_CosY = cos(ssl_RotationInput.y), ssl_SinY = sin(ssl_RotationInput.y),
		   ssl_CosX = cos(ssl_RotationInput.x), ssl_SinX = sin(ssl_RotationInput.x);

	mat4 ssl_OutMatrix;
	
	ssl_OutMatrix[0][0] = ssl_CosZ*ssl_CosY;
	ssl_OutMatrix[1][1] = ssl_CosZ*ssl_CosX;
	ssl_OutMatrix[2][2] = ssl_CosX*ssl_CosY;

	ssl_OutMatrix[0][1] = ssl_SinZ;
	ssl_OutMatrix[0][2] = -ssl_SinY;
	ssl_OutMatrix[1][0] = -ssl_SinZ;
	ssl_OutMatrix[1][2] = ssl_SinX;
	ssl_OutMatrix[2][0] = ssl_SinY;
	ssl_OutMatrix[2][1] = -ssl_SinX;
	
	return ssl_OutMatrix;
}

vec4 sslGetEntityPosition(){
	//we do ssl_Scale first, then rotate, then translate.
	vec4 ssl_Position = (vec4(ssl_BaseEntity.ssl_Translation,1.0));//MAD operations right here!
	
	ssl_Position += (sslCreateRotationMatrix(ssl_BaseEntity.ssl_Rotation)*(vec4(ssl_BaseEntity.ssl_Scale*ssl_VertexPosition,0.0)));
	
	ssl_Position.xy -= 0.5f;//because we moved the origin to be the bottom left, we need to move it back to the center for ssl_Rotation
	
	//applying the parent properties now
	ssl_Position = (vec4(ssl_ParentEntity.ssl_Translation,1.0)+((sslCreateRotationMatrix(ssl_ParentEntity.ssl_Rotation)*(ssl_Position * vec4(ssl_ParentEntity.ssl_Scale,0.0)))));//what a MAD man!
		
	ssl_Position.xy *= 2;
	ssl_Position.xy -= 0.5;
	
	ssl_Position += vec4(ssl_BaseEntity.ssl_Scale/2,0.0f);
	
	vec2 ssl_SizeModifier = ssl_OriginalSize/ssl_CurrentSize;
	
	if(ssl_BaseEntity.ssl_StretchX==0){
		ssl_Position.x*=ssl_SizeModifier.x;
	}
	if(ssl_BaseEntity.ssl_StretchY==0){
		ssl_Position.y*=ssl_SizeModifier.y;
	}
	
	/*
	For those who didn't get the MAD joke:
	MAD means multipy, add, then divide. Doing math in that sequence will be done in 1 gpu cycle instead of 3, which saves a lot of processing power.  Thats what we are doing up there.
	That is also the reason it all has to go on one line. It is hard to understand, but that is simply the way it works.
	*/
	
	return ssl_Position;
}

#endif
)"