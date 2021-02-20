#pragma once

//Used to include GLM in correct order
#include <stdlib.h>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define GLM_FORCE_XYZW_ONLY

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace genesis_core
{
	static const float PI_F = 3.14159265358979323846264338327950288f;
	static const double PI_D = 3.14159265358979323846264338327950288;

	typedef glm::u32vec2 vec2u;
	typedef glm::u32vec3 vec3u;
	typedef glm::u32vec4 vec4u;

	typedef glm::i32vec2 vec2i;
	typedef glm::i32vec3 vec3i;
	typedef glm::i32vec4 vec4i;

	typedef glm::f32vec2 vec2f;
	typedef glm::f32vec3 vec3f;
	typedef glm::f32vec4 vec4f;

	typedef glm::f32quat quatf;
	typedef glm::f32mat3 mat3f;
	typedef glm::f32mat4 mat4f;

	typedef glm::f64vec2 vec2d;
	typedef glm::f64vec3 vec3d;
	typedef glm::f64vec4 vec4d;

	typedef glm::f64quat quatd;
	typedef glm::f64mat3 mat3d;
	typedef glm::f64mat4 mat4d;
}