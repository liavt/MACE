R""(
#ifndef _MACE_ENTITY_LIBRARY_
#define _MACE_ENTITY_LIBRARY_ 1

#include <mc_core>

struct mc_EntityDataStruct{
	vec3 mc_Translation;
	vec3 mc_Rotation;
};

MACE__UNIFORM_BUFFER MACE__ENTITY_DATA_NAME{
	mc_EntityDataStruct mc_BaseEntity;
	mc_EntityDataStruct mc_ParentEntity;
	vec3 mc_Scale;
};

struct _mc_TextureAttachment{
	vec4 mc_Color;
	vec4 mc_TextureTransform;
};

MACE__UNIFORM_BUFFER MACE__PAINTER_DATA_NAME{
	vec3 _mc_TransformTranslation;
	vec3 _mc_TransformRotation;
	vec3 _mc_TransformScale;
	vec4 mc_Data;
	vec3 _mc_Filter;
	float _mc_Opacity;
	_mc_TextureAttachment mc_Foreground;
	_mc_TextureAttachment mc_Background;
	_mc_TextureAttachment mc_Mask;
};

#endif
)""
