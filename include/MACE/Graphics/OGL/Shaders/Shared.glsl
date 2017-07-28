R""(
#define MACE_UNIFORM_BUFFER layout(std140) uniform

precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

struct mc_EntityDataStruct{
	vec3 mc_Translation;
	vec3 mc_Rotation;
};

MACE_UNIFORM_BUFFER MACE_ENTITY_DATA_NAME{
	mc_EntityDataStruct mc_BaseEntity;
	mc_EntityDataStruct mc_ParentEntity;
	vec3 mc_Scale;
};

struct _mc_TextureAttachment{
	vec4 mc_Color;
	vec4 mc_TextureTransform;
};

MACE_UNIFORM_BUFFER MACE_PAINTER_DATA_NAME{
	vec3 _mc_TransformTranslation;
	vec3 _mc_TransformRotation;
	vec3 _mc_TransformScale;
	float _mc_Opacity;
	vec4 mc_Data;
	_mc_TextureAttachment mc_Foreground;
	_mc_TextureAttachment mc_Background;
	_mc_TextureAttachment mc_Mask;
	mat4 _mc_Filter;
};

layout(location = MACE_VAO_DEFAULT_VERTICES_LOCATION) in vec3 _mc_VertexPosition;

mat3 _mcCreateRotationMatrix(const in vec3 mc_RotationInput){
	float mc_CosZ = cos(mc_RotationInput.z), mc_SinZ = sin(mc_RotationInput.z),
		   mc_CosY = cos(mc_RotationInput.y), mc_SinY = sin(mc_RotationInput.y),
		   mc_CosX = cos(mc_RotationInput.x), mc_SinX = sin(mc_RotationInput.x);
		   
	return mat3(
					mc_CosZ*mc_CosY, mc_SinZ, -mc_SinY,
					-mc_SinZ, mc_CosZ*mc_CosX, mc_SinX,
					mc_SinY,-mc_SinX,mc_CosX*mc_CosY
					);
}

vec4 mcGetEntityPosition(){
	//putting it all in one line allows for the compiler to optimize it into a single MAD operation		
	return vec4((_mc_VertexPosition * _mc_TransformScale * _mcCreateRotationMatrix(_mc_TransformRotation) + _mc_TransformTranslation) * mc_Scale * _mcCreateRotationMatrix(mc_BaseEntity.mc_Rotation)
							  + mc_BaseEntity.mc_Translation * _mcCreateRotationMatrix(mc_ParentEntity.mc_Rotation) + mc_ParentEntity.mc_Translation, 1.0);
}

)""
