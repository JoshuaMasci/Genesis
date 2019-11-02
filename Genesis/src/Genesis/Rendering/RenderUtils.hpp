#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Lighting.hpp"
#include "Genesis/Rendering/CommandBuffer.hpp"

namespace Genesis
{
	//Utils for setting uniform data
	class UniformWrite
	{
	public:
		inline static void writeDirectionalLight(CommandBuffer* command_buffer, string base_name, DirectionalLight& directional, vector3F direction)
		{
			command_buffer->setUniformVec3(base_name + ".color", directional.color);
			command_buffer->setUniformFloat(base_name + ".intensity", directional.intensity);

			command_buffer->setUniformVec3(base_name + ".direction", direction);
		};
		
		inline static void writePointLight(CommandBuffer* command_buffer, string base_name, PointLight& directional, vector3F position)
		{
			command_buffer->setUniformVec3(base_name + ".color", directional.color);
			command_buffer->setUniformFloat(base_name + ".intensity", directional.intensity);

			command_buffer->setUniformVec3(base_name + ".attenuation", directional.attenuation);
			command_buffer->setUniformVec3(base_name + ".position", position);
			command_buffer->setUniformFloat(base_name + ".range", directional.range);

		};
		
		inline static void writeSpotLight(CommandBuffer* command_buffer, string base_name, SpotLight& directional, vector3F position, vector3F direction)
		{
			command_buffer->setUniformVec3(base_name + ".color", directional.color);
			command_buffer->setUniformFloat(base_name + ".intensity", directional.intensity);

			command_buffer->setUniformVec3(base_name + ".attenuation", directional.attenuation);
			command_buffer->setUniformVec3(base_name + ".position", position);
			command_buffer->setUniformFloat(base_name + ".range", directional.range);

			command_buffer->setUniformVec3(base_name + ".direction", direction);
			command_buffer->setUniformFloat(base_name + ".cutoff", glm::cos(glm::radians(directional.cutoff / 2.0f)));
		};
	};
}