#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV0;
layout (location = 3) in vec2 inUV1;
layout (location = 4) in vec4 inJoint0;
layout (location = 5) in vec4 inWeight0;

layout(location = 0) out vec2 frag_uv;

#define MAX_NUM_JOINTS 128
struct UBONode {
	mat4 matrix;
	mat4 jointMatrix[MAX_NUM_JOINTS];
	float jointCount;
};
uniform UBONode node;

struct Environment
{
	vec3 camera_position;
	vec3 ambient_light;
	mat4 view_projection_matrix;
};
uniform Environment environment;

struct Matrices
{
	mat4 model;
	mat3 normal;
};
uniform Matrices matrices;

void main()
{
	vec4 locPos;
	if (node.jointCount > 0.0) 
	{
		// Mesh is skinned
		mat4 skinMat = 
			inWeight0.x * node.jointMatrix[int(inJoint0.x)] +
			inWeight0.y * node.jointMatrix[int(inJoint0.y)] +
			inWeight0.z * node.jointMatrix[int(inJoint0.z)] +
			inWeight0.w * node.jointMatrix[int(inJoint0.w)];

		locPos = matrices.model * node.matrix * skinMat * vec4(inPos, 1.0);
		//outNormal = normalize(transpose(inverse(mat3(matrices.model * node.matrix * skinMat))) * inNormal);
	} 
	else 
	{
		locPos = matrices.model * node.matrix * vec4(inPos, 1.0);
		//outNormal = normalize(transpose(inverse(mat3(matrices.model * node.matrix))) * inNormal);
	}
	//locPos.y = -locPos.y;

	vec4 vert_position = vec4(locPos.xyz / locPos.w, 1.0);
    gl_Position = environment.view_projection_matrix * vert_position;
	
	frag_uv = inUV0;
}