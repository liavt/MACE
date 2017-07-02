R""(
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
	vec3 mc_TransformTranslation;
	vec3 mc_TransformRotation;
	vec3 mc_TransformScale;
	vec4 mc_PrimaryColor;
	vec4 mc_SecondaryColor;
	vec4 mc_Data;
};

#endif
)""
