#include "ModelRenderer.hpp"

#include "Genesis/Rendering/RenderUtils.hpp"

using namespace Genesis;

TexturedModelRenderer::TexturedModelRenderer(RenderingBackend* backend)
{
	this->backend = backend;

	this->ambient_pass = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/texture");

	string texture_light_vert = "resources/shaders/vulkan/texture_light.vert.spv";
	this->directional_pass = ShaderLoader::loadShader(this->backend, texture_light_vert, "resources/shaders/vulkan/texture_directional.frag.spv");
	this->point_pass = ShaderLoader::loadShader(this->backend, texture_light_vert, "resources/shaders/vulkan/texture_point.frag.spv");
	this->spot_pass = ShaderLoader::loadShader(this->backend, texture_light_vert, "resources/shaders/vulkan/texture_spot.frag.spv");

	this->shadow_pass = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/shadow");

	//Point light's don't get shadows
	//Don't want to deal with cubemaps yet
	this->directional_shadow_pass = ShaderLoader::loadShader(this->backend, texture_light_vert, "resources/shaders/vulkan/texture_directional_shadow.frag.spv");
	this->spot_shadow_pass = ShaderLoader::loadShader(this->backend, texture_light_vert, "resources/shaders/vulkan/texture_spot_shadow.frag.spv");
}

TexturedModelRenderer::~TexturedModelRenderer()
{
	this->backend->destroyShader(this->ambient_pass);
	this->backend->destroyShader(this->directional_pass);
	this->backend->destroyShader(this->point_pass);
	this->backend->destroyShader(this->spot_pass);
	this->backend->destroyShader(this->shadow_pass);
	this->backend->destroyShader(this->directional_shadow_pass);
	this->backend->destroyShader(this->spot_shadow_pass);
}

void TexturedModelRenderer::drawAmbient(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, vector3F& ambient_light)
{
	command_buffer->setShader(this->ambient_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformVec3("environment.ambient_light", ambient_light);
	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedModelRenderer::drawDirectional(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir)
{
	command_buffer->setShader(this->directional_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeDirectionalLight(command_buffer, "lights.directional", light, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedModelRenderer::drawPoint(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, PointLight& light, vector3F& light_pos)
{
	command_buffer->setShader(this->point_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writePointLight(command_buffer, "lights.point", light, light_pos);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedModelRenderer::drawSpot(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir)
{
	command_buffer->setShader(this->spot_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeSpotLight(command_buffer, "lights.spot", light, light_pos, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedModelRenderer::drawShadow(CommandBuffer* command_buffer, Mesh& mesh, matrix4F& view_proj_matrix, matrix4F& model_matrix)
{
	command_buffer->setShader(this->shadow_pass);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedModelRenderer::drawDirectionalShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix)
{
	command_buffer->setShader(this->directional_shadow_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeDirectionalLight(command_buffer, "lights.directional", light, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->setUniformMat4("lights.shadow_matrix", shadow_matrix);
	command_buffer->setUniformView("shadow_map", shadow_view, 0);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedModelRenderer::drawSpotShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix)
{
	command_buffer->setShader(this->spot_shadow_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeSpotLight(command_buffer, "lights.spot", light, light_pos, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->setUniformMat4("lights.shadow_matrix", shadow_matrix);
	command_buffer->setUniformView("shadow_map", shadow_view, 0);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

TexturedNormalModelRenderer::TexturedNormalModelRenderer(RenderingBackend* backend)
{
	this->backend = backend;

	this->ambient_pass = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/texture_normal");
	string texture_normal_vert = "resources/shaders/vulkan/texture_normal.vert.spv";
	this->directional_pass = ShaderLoader::loadShader(this->backend, texture_normal_vert, "resources/shaders/vulkan/texture_normal_directional.frag.spv");
	this->point_pass = nullptr;
	this->spot_pass = ShaderLoader::loadShader(this->backend, texture_normal_vert, "resources/shaders/vulkan/texture_normal_spot.frag.spv");

	this->shadow_pass = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/shadow_normal");
	this->directional_shadow_pass = ShaderLoader::loadShader(this->backend, texture_normal_vert, "resources/shaders/vulkan/texture_normal_directional_shadow.frag.spv");
	this->spot_shadow_pass = ShaderLoader::loadShader(this->backend, texture_normal_vert, "resources/shaders/vulkan/texture_normal_spot_shadow.frag.spv");
}

TexturedNormalModelRenderer::~TexturedNormalModelRenderer()
{
	this->backend->destroyShader(this->ambient_pass);
	this->backend->destroyShader(this->directional_pass);
	this->backend->destroyShader(this->spot_pass);
	this->backend->destroyShader(this->shadow_pass);
	this->backend->destroyShader(this->directional_shadow_pass);
	this->backend->destroyShader(this->spot_shadow_pass);

}

void TexturedNormalModelRenderer::drawAmbient(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, vector3F& ambient_light)
{
	command_buffer->setShader(this->ambient_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformVec3("environment.ambient_light", ambient_light);
	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedNormalModelRenderer::drawDirectional(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir)
{
	command_buffer->setShader(this->directional_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformTexture("normal_texture", normal_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeDirectionalLight(command_buffer, "lights.directional", light, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedNormalModelRenderer::drawPoint(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, PointLight& light, vector3F& light_pos)
{
}

void TexturedNormalModelRenderer::drawSpot(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir)
{
	command_buffer->setShader(this->spot_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformTexture("normal_texture", normal_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeSpotLight(command_buffer, "lights.spot", light, light_pos, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedNormalModelRenderer::drawShadow(CommandBuffer* command_buffer, Mesh& mesh, matrix4F& view_proj_matrix, matrix4F& model_matrix)
{
	command_buffer->setShader(this->shadow_pass);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedNormalModelRenderer::drawDirectionalShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, DirectionalLight& light, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix)
{
	command_buffer->setShader(this->directional_shadow_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformTexture("normal_texture", normal_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeDirectionalLight(command_buffer, "lights.directional", light, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->setUniformMat4("lights.shadow_matrix", shadow_matrix);
	command_buffer->setUniformView("shadow_map", shadow_view, 0);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}

void TexturedNormalModelRenderer::drawSpotShadow(CommandBuffer* command_buffer, Mesh& mesh, Texture& albedo_texture, Texture& normal_texture, matrix4F& view_proj_matrix, matrix4F& model_matrix, matrix3F& normal_matrix, vector3F& eye_pos, SpotLight& light, vector3F& light_pos, vector3F& light_dir, View& shadow_view, matrix4F& shadow_matrix)
{
	command_buffer->setShader(this->spot_shadow_pass);
	command_buffer->setUniformTexture("albedo_texture", albedo_texture);
	command_buffer->setUniformTexture("normal_texture", normal_texture);
	command_buffer->setUniformMat4("matrices.mvp", view_proj_matrix * model_matrix);
	command_buffer->setUniformMat4("matrices.model", model_matrix);
	command_buffer->setUniformMat3("matrices.normal", normal_matrix);

	//Light
	UniformWrite::writeSpotLight(command_buffer, "lights.spot", light, light_pos, light_dir);
	command_buffer->setUniformVec3("lights.eye_pos", eye_pos);

	command_buffer->setUniformMat4("lights.shadow_matrix", shadow_matrix);
	command_buffer->setUniformView("shadow_map", shadow_view, 0);

	command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
}
