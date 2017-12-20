R""(
#define MACE_UNIFORM_BUFFER layout(std140) uniform

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

struct mc_EntityDataStruct{
	vec3 mc_Translation;
	vec3 mc_Rotation;
	vec3 mc_Scale;
};

MACE_UNIFORM_BUFFER MACE_ENTITY_DATA_NAME{
	mc_EntityDataStruct mc_BaseEntity;
	mc_EntityDataStruct mc_ParentEntity;
	uint mc_EntityID;
};

struct _mc_TextureAttachment{
	vec4 mc_Color;
	vec4 mc_TextureTransform;
};

MACE_UNIFORM_BUFFER MACE_PAINTER_DATA_NAME{
	vec3 _mc_TransformTranslation;
	vec3 _mc_TransformRotation;
	vec3 _mc_TransformScale;
	vec4 mc_Data;
	_mc_TextureAttachment mc_Foreground;
	_mc_TextureAttachment mc_Background;
	_mc_TextureAttachment mc_Mask;
	mat4 _mc_Filter;
};
)""
 