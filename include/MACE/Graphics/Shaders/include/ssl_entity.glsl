R"(
#ifndef _SSL_ENTITY_LIBRARY_
#define _SSL_ENTITY_LIBRARY_ 1

#include <ssl_core>

struct ssl_EntityDataStruct{
	vec3 ssl_Translation;
	float ssl_StretchX;
	vec3 ssl_Rotation;
	float ssl_StretchY;
};

sslUniformBuffer ssl_BaseEntityBuffer{
	ssl_EntityDataStruct ssl_BaseEntity;
	ssl_EntityDataStruct ssl_ParentEntity;
	vec3 ssl_Scale;
	float ssl_EntityOpacity;
};

#endif
)"