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
		inline static void writeDirectionalLight(CommandBufferInterface* command_buffer, string base_name, DirectionalLight& directional, vector3F direction)
		{
			command_buffer->setUniformVec3(base_name + ".color", directional.color);
			command_buffer->setUniformFloat(base_name + ".intensity", directional.intensity);

			command_buffer->setUniformVec3(base_name + ".direction", direction);
		};
		
		inline static void writePointLight(CommandBufferInterface* command_buffer, string base_name, PointLight& point, vector3F position)
		{
			command_buffer->setUniformVec3(base_name + ".color", point.color);
			command_buffer->setUniformFloat(base_name + ".intensity", point.intensity);

			command_buffer->setUniformVec2(base_name + ".attenuation", point.attenuation);
			command_buffer->setUniformVec3(base_name + ".position", position);
			command_buffer->setUniformFloat(base_name + ".range", point.range);

		};
		
		inline static void writeSpotLight(CommandBufferInterface* command_buffer, string base_name, SpotLight& spot, vector3F position, vector3F direction)
		{
			command_buffer->setUniformVec3(base_name + ".color", spot.color);
			command_buffer->setUniformFloat(base_name + ".intensity", spot.intensity);

			command_buffer->setUniformVec2(base_name + ".attenuation", spot.attenuation);
			command_buffer->setUniformVec3(base_name + ".position", position);
			command_buffer->setUniformFloat(base_name + ".range", spot.range);

			command_buffer->setUniformVec3(base_name + ".direction", direction);
			command_buffer->setUniformFloat(base_name + ".cutoff", glm::cos(glm::radians(spot.cutoff / 2.0f)));
		};
	};
}