R"(
#ifndef _MC_ENTITY_LIBRARY_
#define _MC_ENTITY_LIBRARY_ 1

#include <mc_core>

struct mc_EntityDataStruct{
	vec3 mc_Translation;
	vec3 mc_Rotation;
};

mcUniformBuffer mc_BaseEntityBuffer{
	mc_EntityDataStruct mc_BaseEntity;
	mc_EntityDataStruct mc_ParentEntity;
	vec3 mc_Scale;
	float mc_EntityOpacity;
};

mcUniformBuffer mc_PainterSettingsBuffer{
	mat4 mc_Transform;
	vec4 mc_Primary;
	vec4 mc_Secondary;
};

#endif
)"
