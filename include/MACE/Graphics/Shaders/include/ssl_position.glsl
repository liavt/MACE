R"(
#ifndef _SSL_POSITION_LIBRARY_
#define _SSL_POSITION_LIBRARY_

#include <ssl_window>
#include <ssl_entity>

layout(location = 15) in vec3 ssl_VertexPosition;

mat3 sslCreateRotationMatrix(vec3 ssl_RotationInput){
	float ssl_CosZ = cos(ssl_RotationInput.z), ssl_SinZ = sin(ssl_RotationInput.z),
		   ssl_CosY = cos(ssl_RotationInput.y), ssl_SinY = sin(ssl_RotationInput.y),
		   ssl_CosX = cos(ssl_RotationInput.x), ssl_SinX = sin(ssl_RotationInput.x);

	mat3 ssl_OutMatrix;
	
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

	vec4 ssl_Position = vec4(ssl_VertexPosition, 1.0);
	
	ssl_Position.xyz *= ssl_ParentEntity.ssl_Scale;
	ssl_Position.xyz *= ssl_BaseEntity.ssl_Scale;

	ssl_Position.xyz *= sslCreateRotationMatrix(ssl_BaseEntity.ssl_Rotation);
		
	ssl_Position.xyz += ssl_BaseEntity.ssl_Translation*ssl_ParentEntity.ssl_Scale;
		
	ssl_Position.xyz *= sslCreateRotationMatrix(ssl_ParentEntity.ssl_Rotation);
	
	ssl_Position.xyz += ssl_ParentEntity.ssl_Translation;
	
	vec2 ssl_SizeModifier = ssl_OriginalSize/ssl_CurrentSize;
	
	if(ssl_BaseEntity.ssl_StretchX == 0){
		ssl_Position.x*=ssl_SizeModifier.x;
	}
	if(ssl_BaseEntity.ssl_StretchY == 0){
		ssl_Position.y*=ssl_SizeModifier.y;
	}
		
	return ssl_Position;
}

#endif
)"