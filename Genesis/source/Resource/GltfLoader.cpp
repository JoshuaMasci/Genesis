#include "Genesis/Resource/GltfLoader.hpp"

#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"
#include <algorithm>

using namespace Genesis;

GltfModel::GltfModel(LegacyBackend* legacy_backend, tinygltf::Model& gltfModel)
{
	this->backend = legacy_backend;

	this->loadTextures(gltfModel);
	this->loadMaterials(gltfModel);
	this->loadMeshes(gltfModel);
	this->loadSkins(gltfModel);
	this->loadNodes(gltfModel);
	this->loadAnimations(gltfModel);
	this->calcHierarchy();
}

GltfModel::~GltfModel()
{
	this->unloadAnimations();
	this->unloadNodes();
	this->unloadSkins();
	this->unloadMeshes();
	this->unloadMaterials();
	this->unloadTextures();
}

TextureFilterMode getFilterMode(int mode)
{
	switch (mode) {
	case 9728:
		return TextureFilterMode::Nearest;
	case 9729:
		return TextureFilterMode::Linear;
	case 9984:
		return TextureFilterMode::Nearest;
	case 9985:
		return TextureFilterMode::Nearest;
	case 9986:
		return TextureFilterMode::Linear;
	case 9987:
		return TextureFilterMode::Linear;
	}

	return TextureFilterMode::Linear;
}

TextureWrapMode getWrapMode(int mode)
{
	switch (mode)
	{
	case 10497:
		return TextureWrapMode::Repeat;
	case 33071:
		return TextureWrapMode::Clamp_Edge;
	case 33648:
		return TextureWrapMode::Mirrored_Repeat;
	}

	return TextureWrapMode::Repeat;
}

float GltfModel::getAnimationLength(uint32_t animation_index)
{
	if (animation_index < this->animations.size())
	{
		return this->animations[animation_index].end_time;
	}

	return 0.0f;
}

void GltfModel::playAnimation(uint32_t animation_index, float current_time)
{
	/*if (animations.empty()) 
	{
		std::cout << ".glTF does not contain animation." << std::endl;
		return;
	}
	if (index > static_cast<uint32_t>(animations.size()) - 1) 
	{
		std::cout << "No animation with index " << index << std::endl;
		return;
	}*/
	
	Animation& animation = this->animations[animation_index];

	for (AnimationChannel& channel : animation.channels)
	{
		for (size_t i = 0; i < channel.timesteps.size() - 1; i++)
		{
			if ((current_time >= channel.timesteps[i]) && (current_time <= channel.timesteps[i + 1]))
			{
				float u = std::max(0.0f, current_time - channel.timesteps[i]) / (channel.timesteps[i + 1] - channel.timesteps[i]);
				if (u <= 1.0f)
				{
					switch (channel.path)
					{
					case Genesis::AnimationChannel::TRANSLATION:
						//this->node_storage[channel.node_index].local_transform.setPosition(glm::mix(channel.values[i], channel.values[i + 1], u));
						break;
					case Genesis::AnimationChannel::ROTATION:
						glm::quat q1;
						q1.x = channel.values[i].x;
						q1.y = channel.values[i].y;
						q1.z = channel.values[i].z;
						q1.w = channel.values[i].w;
						glm::quat q2;
						q2.x = channel.values[i + 1].x;
						q2.y = channel.values[i + 1].y;
						q2.z = channel.values[i + 1].z;
						q2.w = channel.values[i + 1].w;
						//this->node_storage[channel.node_index].local_transform.setOrientation(glm::normalize(glm::slerp(q1, q2, u)));
						break;
					case Genesis::AnimationChannel::SCALE:
						//this->node_storage[channel.node_index].local_transform.setScale(glm::mix(channel.values[i], channel.values[i + 1], u));
						break;
					}
				}
			}
		}
	}
}

matrix4F getRootTransform(GltfNode* node)
{
	matrix4F matrix = node->local_transform.getModelMatrix();
	GltfNode* parent = node->parent_node;
	while (parent != nullptr)
	{
		matrix = parent->local_transform.getModelMatrix() * matrix;
		parent = parent->parent_node;
	}

	return matrix;
}

void GltfModel::updateSkins()
{
	for (uint32_t skin_index = 0; skin_index < this->skins.size(); skin_index++)
	{
		GltfSkin& skin = this->skins[skin_index];

		GltfNode& root_node = this->node_storage[skin.skeleton_root_index];

		matrix4F root_matrix = getRootTransform(&root_node);
		matrix4F inverse_matrix = glm::inverse(root_matrix);

		for (uint32_t i = 0; i < skin.joints.size(); i++)
		{
			GltfNode& joint_node = this->node_storage[skin.joints[i]];
			matrix4F joint_matrix = getRootTransform(&joint_node) * skin.inverse_bind_matrices[i];
			skin.joint_matrices[i] = inverse_matrix * joint_matrix;
		}
	}
}

void GltfModel::loadTextures(tinygltf::Model& gltfModel)
{
	struct TextureSampler 
	{
		TextureFilterMode mag_filter;
		TextureFilterMode min_filter;
		TextureWrapMode address_mode_u;
		TextureWrapMode address_mode_v;
	};

	vector<TextureSampler> samplers(gltfModel.samplers.size());
	for (size_t i = 0; i < gltfModel.samplers.size(); i++) 
	{
		TextureSampler sampler = {};
		sampler.mag_filter = getFilterMode(gltfModel.samplers[i].minFilter);
		sampler.min_filter = getFilterMode(gltfModel.samplers[i].magFilter);
		sampler.address_mode_u = getWrapMode(gltfModel.samplers[i].wrapS);
		sampler.address_mode_v = getWrapMode(gltfModel.samplers[i].wrapT);
		samplers[i] = sampler;
	}

	this->textures.resize(gltfModel.textures.size());
	for (size_t i = 0; i < gltfModel.textures.size(); i++)
	{
		tinygltf::Texture texture = gltfModel.textures[i];
		tinygltf::Image image = gltfModel.images[texture.source];

		TextureCreateInfo info = {};
		info.format = (TextureFormat)image.component;
		info.size = { image.width, image.height };

		if (texture.sampler == -1)
		{
			// No sampler specified, use a default one
			info.filter_mode = TextureFilterMode::Linear;
			info.wrap_mode = TextureWrapMode::Repeat;
		}
		else 
		{
			info.filter_mode = samplers[texture.sampler].min_filter;
			info.wrap_mode = samplers[texture.sampler].address_mode_u;
		}

		this->textures[i] = backend->createTexture(info, (void*)&image.image[0]);
	}
}

void GltfModel::unloadTextures()
{
	for (size_t i = 0; i < this->textures.size(); i++)
	{
		this->backend->destoryTexture(this->textures[i]);
	}
}

void GltfModel::loadMaterials(tinygltf::Model& gltfModel)
{
	this->materials.resize(gltfModel.materials.size());
	for (size_t material_index = 0; material_index < gltfModel.materials.size(); material_index++)
	{
		tinygltf::Material material = gltfModel.materials[material_index];
		PbrMaterial& new_material = this->materials[material_index];

		if (material.additionalValues.find("alphaMode") != material.additionalValues.end())
		{
			tinygltf::Parameter param = material.additionalValues["alphaMode"];
			if (param.string_value == "BLEND")
			{
				new_material.alpha_mode = PbrMaterial::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK")
			{
				new_material.alpha_cutoff = 0.5f;
				new_material.alpha_mode = PbrMaterial::ALPHAMODE_MASK;
			}
		}
		if (material.additionalValues.find("alphaCutoff") != material.additionalValues.end())
		{
			new_material.alpha_cutoff = static_cast<float>(material.additionalValues["alphaCutoff"].Factor());
		}

		if (material.values.find("baseColorFactor") != material.values.end())
		{
			new_material.albedo_factor = glm::make_vec4(material.values["baseColorFactor"].ColorFactor().data());
		}
		if (material.values.find("metallicFactor") != material.values.end())
		{
			new_material.metallic_roughness_factor.x = static_cast<float>(material.values["metallicFactor"].Factor());
		}
		if (material.values.find("roughnessFactor") != material.values.end())
		{
			new_material.metallic_roughness_factor.y = static_cast<float>(material.values["roughnessFactor"].Factor());
		}
		if (material.additionalValues.find("emissiveFactor") != material.additionalValues.end())
		{
			new_material.emissive_factor = vector4F(glm::make_vec3(material.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
		}

		if (material.values.find("baseColorTexture") != material.values.end()) 
		{
			new_material.albedo_texture = this->textures[material.values["baseColorTexture"].TextureIndex()];
			new_material.albedo_uv = material.values["baseColorTexture"].TextureTexCoord();
		}
		if (material.values.find("metallicRoughnessTexture") != material.values.end()) 
		{
			new_material.metallic_roughness_texture = this->textures[material.values["metallicRoughnessTexture"].TextureIndex()];
			new_material.metallic_roughness_uv = material.values["metallicRoughnessTexture"].TextureTexCoord();
		}
		if (material.additionalValues.find("normalTexture") != material.additionalValues.end()) 
		{
			new_material.normal_texture = this->textures[material.additionalValues["normalTexture"].TextureIndex()];
			new_material.normal_uv = material.additionalValues["normalTexture"].TextureTexCoord();
		}
		if (material.additionalValues.find("emissiveTexture") != material.additionalValues.end()) 
		{
			new_material.emissive_texture = this->textures[material.additionalValues["emissiveTexture"].TextureIndex()];
			new_material.emissive_uv = material.additionalValues["emissiveTexture"].TextureTexCoord();
		}
		if (material.additionalValues.find("occlusionTexture") != material.additionalValues.end()) 
		{
			new_material.occlusion_texture = this->textures[material.additionalValues["occlusionTexture"].TextureIndex()];
			new_material.occlusion_uv = material.additionalValues["occlusionTexture"].TextureTexCoord();
		}
	}
}

void GltfModel::unloadMaterials()
{
	//No need to do anything
}

void GltfModel::loadMeshes(tinygltf::Model& gltfModel)
{
	this->meshes.resize(gltfModel.meshes.size());
	for (size_t mesh_index = 0; mesh_index < gltfModel.meshes.size(); mesh_index++)
	{
		tinygltf::Mesh mesh = gltfModel.meshes[mesh_index];
		PbrMesh& new_mesh = this->meshes[mesh_index];

		vector<PbrMeshVertex> vertex_buffer;
		vector<uint32_t> index_buffer;

		new_mesh.primitives.resize(mesh.primitives.size());
		for (size_t primitive_index = 0; primitive_index < mesh.primitives.size(); primitive_index++)
		{
			uint32_t vertex_start = static_cast<uint32_t>(vertex_buffer.size());
			uint32_t index_start = static_cast<uint32_t>(index_buffer.size());

			const tinygltf::Primitive &primitive = mesh.primitives[primitive_index];
			PbrMeshPrimitive& new_primitive = new_mesh.primitives[primitive_index];

			uint32_t index_count = 0;
			uint32_t vertex_count = 0;
			vector3F position_min;
			vector3F position_max;
			bool has_skin = false;
			bool has_indices = primitive.indices > -1;
			// Vertices
			{
				const float *buffer_postion = nullptr;
				const float *buffer_normals = nullptr;
				const float *buffer_uv0 = nullptr;
				const float *buffer_uv1 = nullptr;
				const uint16_t *buffer_joints = nullptr;
				const float *buffer_weights = nullptr;

				int position_byte_stride;
				int normal_byte_stride;
				int uv0_byte_stride;
				int uv1_byte_stride;
				int joint_byte_stride;
				int weight_byte_stride;

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor &position_accessor = gltfModel.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView &position_view = gltfModel.bufferViews[position_accessor.bufferView];
				buffer_postion = reinterpret_cast<const float *>(&(gltfModel.buffers[position_view.buffer].data[position_accessor.byteOffset + position_view.byteOffset]));
				position_min = vector3F(position_accessor.minValues[0], position_accessor.minValues[1], position_accessor.minValues[2]);
				position_max = vector3F(position_accessor.maxValues[0], position_accessor.maxValues[1], position_accessor.maxValues[2]);
				vertex_count = static_cast<uint32_t>(position_accessor.count);
				position_byte_stride = position_accessor.ByteStride(position_view) ? (position_accessor.ByteStride(position_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					const tinygltf::Accessor &normal_accessor = gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &normal_view = gltfModel.bufferViews[normal_accessor.bufferView];
					buffer_normals = reinterpret_cast<const float *>(&(gltfModel.buffers[normal_view.buffer].data[normal_accessor.byteOffset + normal_view.byteOffset]));
					normal_byte_stride = normal_accessor.ByteStride(normal_view) ? (normal_accessor.ByteStride(normal_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor &uv_accessor = gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &uv_view = gltfModel.bufferViews[uv_accessor.bufferView];
					buffer_uv0 = reinterpret_cast<const float *>(&(gltfModel.buffers[uv_view.buffer].data[uv_accessor.byteOffset + uv_view.byteOffset]));
					uv0_byte_stride = uv_accessor.ByteStride(uv_view) ? (uv_accessor.ByteStride(uv_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}
				if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end())
				{
					const tinygltf::Accessor &uv_accessor = gltfModel.accessors[primitive.attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView &uv_view = gltfModel.bufferViews[uv_accessor.bufferView];
					buffer_uv1 = reinterpret_cast<const float *>(&(gltfModel.buffers[uv_view.buffer].data[uv_accessor.byteOffset + uv_view.byteOffset]));
					uv1_byte_stride = uv_accessor.ByteStride(uv_view) ? (uv_accessor.ByteStride(uv_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor &joint_accessor = gltfModel.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView &joint_view = gltfModel.bufferViews[joint_accessor.bufferView];
					buffer_joints = reinterpret_cast<const uint16_t *>(&(gltfModel.buffers[joint_view.buffer].data[joint_accessor.byteOffset + joint_view.byteOffset]));
					joint_byte_stride = joint_accessor.ByteStride(joint_view) ? (joint_accessor.ByteStride(joint_view) / sizeof(buffer_joints[0])) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor &weightAccessor = gltfModel.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView &weightView = gltfModel.bufferViews[weightAccessor.bufferView];
					buffer_weights = reinterpret_cast<const float *>(&(gltfModel.buffers[weightView.buffer].data[weightAccessor.byteOffset + weightView.byteOffset]));
					weight_byte_stride = weightAccessor.ByteStride(weightView) ? (weightAccessor.ByteStride(weightView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				has_skin = (buffer_joints && buffer_weights);

				vertex_buffer.resize(vertex_start + position_accessor.count);
				for (size_t vertex_index = 0; vertex_index < position_accessor.count; vertex_index++)
				{
					PbrMeshVertex vertex{};
					vertex.position = vector4F(glm::make_vec3(&buffer_postion[vertex_index * position_byte_stride]), 1.0f);
					vertex.normal = glm::normalize(vector3F(buffer_normals ? glm::make_vec3(&buffer_normals[vertex_index * normal_byte_stride]) : vector3F(0.0f)));
					vertex.uv0 = buffer_uv0 ? glm::make_vec2(&buffer_uv0[vertex_index * uv0_byte_stride]) : vector3F(0.0f);
					vertex.uv1 = buffer_uv1 ? glm::make_vec2(&buffer_uv1[vertex_index * uv1_byte_stride]) : vector3F(0.0f);

					vertex.joint0 = has_skin ? vector4F(glm::make_vec4(&buffer_joints[vertex_index * joint_byte_stride])) : vector4F(0.0f);
					vertex.weight0 = has_skin ? glm::make_vec4(&buffer_weights[vertex_index * weight_byte_stride]) : vector4F(0.0f);
					// Fix for all zero weights
					if (glm::length(vertex.weight0) == 0.0f)
					{
						vertex.weight0 = vector4F(1.0f, 0.0f, 0.0f, 0.0f);
					}
					vertex_buffer[vertex_index + vertex_start] = vertex;
				}
			}
			// Indices
			if (has_indices)
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView &buffer_view = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[buffer_view.buffer];

				index_count = static_cast<uint32_t>(accessor.count);
				const void *dataPtr = &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]);

				index_buffer.resize(index_start + index_count);

				switch (accessor.componentType)
				{
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
				{
					const uint32_t *buf = static_cast<const uint32_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						index_buffer[index_start + index] = buf[index] + vertex_start;
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
				{
					const uint16_t *buf = static_cast<const uint16_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						index_buffer[index_start + index] = buf[index] + vertex_start;
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
				{
					const uint8_t *buf = static_cast<const uint8_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						index_buffer[index_start + index] = buf[index] + vertex_start;
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			new_primitive.first_index = index_start;
			new_primitive.index_count = index_count;
			new_primitive.vertex_count = vertex_count;
			new_primitive.material_index = primitive.material;
			new_primitive.bounding_box = { position_min, position_max };
		}

		for (auto primitive : new_mesh.primitives)
		{
			new_mesh.bounding_box.min = glm::min(new_mesh.bounding_box.min, primitive.bounding_box.min);
			new_mesh.bounding_box.max = glm::max(new_mesh.bounding_box.max, primitive.bounding_box.max);
		}

		VertexInputDescriptionCreateInfo create_info = {};
		vector<VertexElementType> elements =
		{
			VertexElementType::float_3,
			VertexElementType::float_3,
			VertexElementType::float_2,
			VertexElementType::float_2,
			VertexElementType::float_4,
			VertexElementType::float_4,
		};
		create_info.input_elements = elements.data();
		create_info.input_elements_count = (uint32_t)elements.size();

		new_mesh.vertex_buffer = this->backend->createVertexBuffer(vertex_buffer.data(), vertex_buffer.size() * sizeof(PbrMeshVertex), create_info);
		new_mesh.index_buffer = this->backend->createIndexBuffer(index_buffer.data(), index_buffer.size() * sizeof(uint32_t), IndexType::uint32);
	}
}

void GltfModel::unloadMeshes()
{
	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		this->backend->destoryVertexBuffer(this->meshes[i].vertex_buffer);
		this->backend->destoryIndexBuffer(this->meshes[i].index_buffer);
	}
}

void GltfModel::loadSkins(tinygltf::Model& gltfModel)
{
	this->skins.resize(gltfModel.skins.size());
	for (size_t skin_index = 0; skin_index < gltfModel.skins.size(); skin_index++)
	{
		const tinygltf::Skin& skin = gltfModel.skins[skin_index];
		GltfSkin& new_skin = this->skins[skin_index];

		new_skin.name = skin.name;

		GENESIS_ENGINE_ASSERT_ERROR(skin.skeleton > -1, "Error: no root node to skeleton");
		new_skin.skeleton_root_index = skin.skeleton;

		new_skin.joints.resize(skin.joints.size());
		for (size_t joint_index = 0; joint_index < skin.joints.size(); joint_index++)
		{
			new_skin.joints[joint_index] = skin.joints[joint_index];
		}

		GENESIS_ENGINE_ASSERT_ERROR(skin.inverseBindMatrices > -1, "Error: no inverse bind matrix");
		const tinygltf::Accessor &accessor = gltfModel.accessors[skin.inverseBindMatrices];
		const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
		const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
		new_skin.inverse_bind_matrices.resize(accessor.count);
		new_skin.joint_matrices.resize(accessor.count);
		memcpy(new_skin.inverse_bind_matrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(matrix4F));
	}
}

void GltfModel::unloadSkins()
{
	//No need to do anything
}

void GltfModel::loadNodes(tinygltf::Model& gltfModel)
{
	this->node_storage.resize(gltfModel.nodes.size());
	for (size_t node_index = 0; node_index < gltfModel.nodes.size(); node_index++)
	{
		const tinygltf::Node& node = gltfModel.nodes[node_index];
		GltfNode& new_node = this->node_storage[node_index];
		new_node.name = node.name;
		new_node.index = (uint32_t)node_index;

		if (node.translation.size() == 3)
		{
			new_node.local_transform.setPosition(glm::make_vec3(node.translation.data()));
		}

		if (node.rotation.size() == 4)
		{
			new_node.local_transform.setOrientation(glm::make_quat(node.rotation.data()));
		}

		if (node.scale.size() == 3)
		{
			new_node.local_transform.setScale(glm::make_vec3(node.scale.data()));
		}

		if (node.matrix.size() == 16)
		{
			new_node.local_transform.setTransform(glm::make_mat4x4(node.matrix.data()));
		}

		new_node.child_indices.resize(node.children.size());
		for (size_t child_index = 0; child_index < node.children.size(); child_index++)
		{
			new_node.child_indices[child_index] = (uint32_t)node.children[child_index];
		}

		if (node.mesh > -1)
		{
			new_node.mesh = &this->meshes[node.mesh];
		}
		else
		{
			new_node.mesh = nullptr;
		}

		if (node.skin > -1)
		{
			new_node.skin = &this->skins[node.skin];
		}
		else
		{
			new_node.skin = nullptr;
		}

	}
}

void GltfModel::unloadNodes()
{
	//No need to do anything
}

void GltfModel::loadAnimations(tinygltf::Model& gltfModel)
{
	this->animations.resize(gltfModel.animations.size());
	for (size_t animation_index = 0; animation_index < gltfModel.animations.size(); animation_index++)
	{
		const tinygltf::Animation& animation = gltfModel.animations[animation_index];
		Animation& new_animation = this->animations[animation_index];

		new_animation.name = animation.name;

		if (new_animation.name.empty())
		{
			new_animation.name = string("Animation_" + std::to_string(animation_index));
		}

		/*for (size_t sampler_index = 0; sampler_index < animation.samplers.size(); sampler_index++)
		{
			const tinygltf::AnimationSampler& sampler = animation.samplers[sampler_index];
			GltfAnimationSampler& new_sampler = new_animation.samplers[sampler_index];

			if (sampler.interpolation == "LINEAR")
			{
				new_sampler.interpolation = GltfAnimationSampler::InterpolationType::LINEAR;
			}
			else if (sampler.interpolation == "STEP")
			{
				new_sampler.interpolation = GltfAnimationSampler::InterpolationType::STEP;
			}
			else if (sampler.interpolation == "CUBICSPLINE")
			{
				new_sampler.interpolation = GltfAnimationSampler::InterpolationType::CUBICSPLINE;
			}

			// Read sampler input time values
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[sampler.input];
				const tinygltf::BufferView &buffer_view = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[buffer_view.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const void *dataPtr = &buffer.data[accessor.byteOffset + buffer_view.byteOffset];
				const float *buf = static_cast<const float*>(dataPtr);

				new_sampler.inputs.resize(accessor.count);
				for (size_t index = 0; index < accessor.count; index++) 
				{
					float value = buf[index];
					new_sampler.inputs[index] = value;

					new_animation.start_time = std::min(value, new_animation.start_time);
					new_animation.end_time = std::max(value, new_animation.end_time);
				}
			}

			// Read sampler output T/R/S values 
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[sampler.output];
				const tinygltf::BufferView &buffer_view = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[buffer_view.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const void *dataPtr = &buffer.data[accessor.byteOffset + buffer_view.byteOffset];

				new_sampler.outputs_vector4.resize(accessor.count);

				switch (accessor.type)
				{
				case TINYGLTF_TYPE_VEC3: 
				{
					const vector3F *buf = static_cast<const vector3F*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) 
					{
						new_sampler.outputs_vector4[index] = vector4F(buf[index], 0.0f);
					}
					break;
				}
				case TINYGLTF_TYPE_VEC4: 
				{
					const vector4F *buf = static_cast<const vector4F*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) 
					{
						new_sampler.outputs_vector4[index] = vector4F(buf[index]);
					}
					break;
				}
				default: 
				{
					GENESIS_ENGINE_WARNING("Unknown sampler type");
					break;
				}
				}
			}
		}*/

		new_animation.channels.resize(animation.channels.size());
		for (size_t channel_index = 0; channel_index < animation.channels.size(); channel_index++)
		{
			tinygltf::AnimationChannel channel = animation.channels[channel_index];
			AnimationChannel& new_channel = new_animation.channels[channel_index];

			if (channel.target_path == "rotation")
			{
				new_channel.path = AnimationChannel::PathType::ROTATION;
			}
			if (channel.target_path == "translation")
			{
				new_channel.path = AnimationChannel::PathType::TRANSLATION;
			}
			if (channel.target_path == "scale")
			{
				new_channel.path = AnimationChannel::PathType::SCALE;
			}

			if (channel.sampler != -1)
			{
				const tinygltf::AnimationSampler& sampler = animation.samplers[channel.sampler];

				if (sampler.interpolation == "LINEAR")
				{
					new_channel.interpolation = AnimationChannel::InterpolationType::LINEAR;
				}
				else if (sampler.interpolation == "STEP")
				{
					new_channel.interpolation = AnimationChannel::InterpolationType::STEP;
				}
				else if (sampler.interpolation == "CUBICSPLINE")
				{
					new_channel.interpolation = AnimationChannel::InterpolationType::CUBICSPLINE;
				}

				// Read sampler input time values
				{
					const tinygltf::Accessor& input_accessor = gltfModel.accessors[sampler.input];
					const tinygltf::BufferView& input_buffer_view = gltfModel.bufferViews[input_accessor.bufferView];
					const tinygltf::Buffer& input_buffer = gltfModel.buffers[input_buffer_view.buffer];
					assert(input_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
					const float *input_data = static_cast<const float*>((void*)&input_buffer.data[input_accessor.byteOffset + input_buffer_view.byteOffset]);

					new_channel.timesteps.resize(input_accessor.count);
					for (size_t index = 0; index < input_accessor.count; index++)
					{
						float value = input_data[index];
						new_channel.timesteps[index] = value;

						new_animation.start_time = std::min(value, new_animation.start_time);
						new_animation.end_time = std::max(value, new_animation.end_time);
					}
				}

				// Read sampler output T/R/S values 
				{
					const tinygltf::Accessor &accessor = gltfModel.accessors[sampler.output];
					const tinygltf::BufferView &buffer_view = gltfModel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer &buffer = gltfModel.buffers[buffer_view.buffer];

					assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

					const void *dataPtr = &buffer.data[accessor.byteOffset + buffer_view.byteOffset];

					new_channel.values.resize(accessor.count);

					switch (accessor.type)
					{
					case TINYGLTF_TYPE_VEC3:
					{
						const vector3F *buf = static_cast<const vector3F*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							new_channel.values[index] = vector4F(buf[index], 0.0f);
						}
						break;
					}
					case TINYGLTF_TYPE_VEC4:
					{
						const vector4F *buf = static_cast<const vector4F*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							new_channel.values[index] = vector4F(buf[index]);
						}
						break;
					}
					default:
					{
						GENESIS_ENGINE_WARNING("Unknown sampler type");
						break;
					}
					}
				}


			}
			else
			{

			}

			//new_channel.sampler_index = (uint32_t)channel.sampler;
			//new_channel.node_index = (uint32_t)channel.target_node;
		}
	}
}

void GltfModel::unloadAnimations()
{
}

void GltfModel::calcHierarchy()
{
	for (GltfNode& node : this->node_storage)
	{
		for (uint32_t child_index : node.child_indices)
		{
			this->node_storage[child_index].parent_node = &node;
			node.child_nodes.push_back(&this->node_storage[child_index]);
		}
	}

	for (GltfNode& node : this->node_storage)
	{
		if (node.parent_node == nullptr)
		{
			this->root_nodes.push_back(&node);
		}
	}
}
