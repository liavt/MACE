R"(

#include <ssl_window>

#ifndef _SSL_POSITION_LIBRARY_
#define _SSL_POSITION_LIBRARY_

struct ssl_EntityDataStruct{
	vec3 ssl_Translation;
	float ssl_StretchX;
	vec3 ssl_Scale;
	float ssl_StretchY;
	mat4 ssl_Rotation;
};

layout(location = 15) in vec3 ssl_VertexPosition;

layout(std140) uniform ssl_BaseEntityBuffer{
	ssl_EntityDataStruct ssl_BaseEntity;
	ssl_EntityDataStruct ssl_ParentEntity;
};


vec4 sslGetEntityPosition(){
	//we do ssl_Scale first, then rotate, then translate.
	vec4 ssl_Position = (vec4(ssl_BaseEntity.ssl_Translation,1.0)+(ssl_BaseEntity.ssl_Rotation * (vec4(ssl_BaseEntity.ssl_Scale*ssl_VertexPosition,0.0))));//MAD operations right here!
	
	ssl_Position.xy -= 0.5f;//because we moved the origin to be the bottom left, we need to move it back to the center for ssl_Rotation
	
	//applying the parent properties now
	ssl_Position = (vec4(ssl_ParentEntity.ssl_Translation,1.0)+((ssl_ParentEntity.ssl_Rotation * (ssl_Position * vec4(ssl_ParentEntity.ssl_Scale,0.0)))));//what a MAD man!
		
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
	*/
	
	return ssl_Position;
}

#endif

)"