R"(
#ifndef _SSL_ENTITY_LIBRARY_
#define _SSL_ENTITY_LIBRARY_

struct ssl_EntityDataStruct{
	vec3 ssl_Translation;
	float ssl_StretchX;
	vec3 ssl_Rotation;
	float ssl_StretchY;
};

layout(std140) uniform ssl_BaseEntityBuffer{
	ssl_EntityDataStruct ssl_BaseEntity;
	ssl_EntityDataStruct ssl_ParentEntity;
	vec4 ssl_EntityPaint;
	vec3 ssl_Scale;
	float ssl_EntityOpacity;
};

#endif
)"