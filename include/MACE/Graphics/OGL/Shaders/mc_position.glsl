R""(
#ifndef _MACE_POSITION_LIBRARY_
#define _MACE_POSITION_LIBRARY_ 1

#include <mc_core>
#include <mc_entity>

layout(location = MACE__VAO_DEFAULT_VERTICES_LOCATION) in vec3 _mc_VertexPosition;

mat3 _mcCreateRotationMatrix(const vec3 mc_RotationInput){
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
							  + mc_BaseEntity.mc_Translation * _mcCreateRotationMatrix(mc_ParentEntity.mc_Rotation)
							  + mc_ParentEntity.mc_Translation, 1.0);
}

#endif
)""
