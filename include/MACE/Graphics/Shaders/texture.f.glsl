//when the preprocessor copy and pastes this file, the newlines will be syntax errors. we need to specify that this is a multiline string. if you want syntax highlighting, make sure to configure your editor to ignore this line
R""(

//FRAGMENT SHADER

#include <ssl_core>

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

in lowp vec2 textureCoord;

struct ssl_EntityDataStruct{
	vec3 ssl_Translation;
	vec3 ssl_Rotation;
};

sslUniformBuffer ssl_BaseEntityBuffer{
	ssl_EntityDataStruct ssl_BaseEntity;
	ssl_EntityDataStruct ssl_ParentEntity;
	vec3 ssl_Scale;
	float ssl_EntityOpacity;
};

layout(location = 0) out lowp vec4 ssl_OutColor;
layout(location = 1) out uint ssl_OutID;

uniform lowp sampler2D tex;

uniform uint ssl_EntityID;

void main(void){		
	ssl_OutColor = vec4(1.0, 0, 0, 1.0);
	
	ssl_OutID = ssl_EntityID;
}


)""
