#pragma once

//Used to include GLM in correct order
#include <stdlib.h>
#include <iostream>

//GML needs this to not complain about rotations being deprecated
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_XYZW_ONLY
#define PI 3.14159265358979323846264338327950288

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Genesis
{
	typedef glm::u32vec2 vector2U;
	typedef glm::i32vec2 vector2I;
	typedef glm::i32vec3 vector3I;

	typedef glm::f32vec2 vector2F;
	typedef glm::f32vec3 vector3F;
	typedef glm::f32vec4 vector4F;
	typedef glm::f32quat quaternionF;
	typedef glm::f32mat3 matrix3F;
	typedef glm::f32mat4 matrix4F;

	typedef glm::f64vec2 vector2D;
	typedef glm::f64vec3 vector3D;
	typedef glm::f64vec4 vector4D;
	typedef glm::f64quat quaternionD;
	typedef glm::f64mat3 matrix3D;
	typedef glm::f64mat4 matrix4D;
};
