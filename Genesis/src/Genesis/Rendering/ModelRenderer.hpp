#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Rendering/Lighting.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"

namespace Genesis
{
	class TexturedModelRenderer
	{
	public:
		TexturedModelRenderer(RenderingBackend* backend);
		~TexturedModelRenderer();

		void drawAmbient(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, vector3F& ambient_light);

		void drawDirectional(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir);
		void drawPoint(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, PointLight& light, vector3F& light_pos);
		void drawSpot(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir);

		void drawShadow(CommandBuffer* command_buffer, Mesh& mesh, matrix4F& view_proj_matrix, matrix4F& model_matrix);

		void drawDirectionalShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix);
		void drawSpotShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix);

	private:
		RenderingBackend* backend;

		Shader ambient_pass;
		Shader directional_pass;
		Shader point_pass;
		Shader spot_pass;
		Shader shadow_pass;
		Shader directional_shadow_pass;
		Shader spot_shadow_pass;
	};

	class TexturedNormalModelRenderer
	{
	public:
		TexturedNormalModelRenderer(RenderingBackend* backend);
		~TexturedNormalModelRenderer();

		void drawAmbient(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& height_texture, float height_scale, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, vector3F& ambient_light);

		void drawDirectional(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, Texture& height_texture, float height_scale, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir);
		void drawPoint(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, Texture& height_texture, float height_scale, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, PointLight& light, vector3F& light_pos);
		void drawSpot(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, Texture& height_texture, float height_scale, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir);

		void drawShadow(CommandBuffer* command_buffer, Mesh& mesh, matrix4F& view_proj_matrix, matrix4F& model_matrix);

		void drawDirectionalShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, Texture& height_texture, float height_scale, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix);
		void drawSpotShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, Texture& height_texture, float height_scale, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix);

	private:
		RenderingBackend* backend;

		Shader ambient_pass;
		Shader directional_pass;
		Shader point_pass;
		Shader spot_pass;
		Shader shadow_pass;
		Shader directional_shadow_pass;
		Shader spot_shadow_pass;
	};
}