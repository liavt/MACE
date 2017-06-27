R"(
#ifndef _MC_POSITION_LIBRARY_
#define _MC_POSITION_LIBRARY_ 1

#include <mc_core>
#include <mc_entity>

layout(location = MACE__VAO_VERTICES_LOCATION) in vec3 mc_VertexPosition;

mat3 mcCreateRotationMatrix(vec3 mc_RotationInput){
	float mc_CosZ = cos(mc_RotationInput.z), mc_SinZ = sin(mc_RotationInput.z),
		   mc_CosY = cos(mc_RotationInput.y), mc_SinY = sin(mc_RotationInput.y),
		   mc_CosX = cos(mc_RotationInput.x), mc_SinX = sin(mc_RotationInput.x);

	mat3 mc_OutMatrix;
	
	mc_OutMatrix[0][0] = mc_CosZ*mc_CosY;
	mc_OutMatrix[1][1] = mc_CosZ*mc_CosX;
	mc_OutMatrix[2][2] = mc_CosX*mc_CosY;

	mc_OutMatrix[0][1] = mc_SinZ;
	mc_OutMatrix[0][2] = -mc_SinY;
	mc_OutMatrix[1][0] = -mc_SinZ;
	mc_OutMatrix[1][2] = mc_SinX;
	mc_OutMatrix[2][0] = mc_SinY;
	mc_OutMatrix[2][1] = -mc_SinX;
	
	return mc_OutMatrix;
}

vec4 mcGetEntityPosition(){
	//putting it all in one line allows for the compiler to optimize it into a single MAD operation		
	return vec4((mc_VertexPosition * mc_TransformScale + mc_TransformTranslation) * mc_Scale * mcCreateRotationMatrix(mc_BaseEntity.mc_Rotation)
							  + mc_BaseEntity.mc_Translation * mcCreateRotationMatrix(mc_ParentEntity.mc_Rotation)
							  + mc_ParentEntity.mc_Translation, 1.0);
}

#endif
)"
