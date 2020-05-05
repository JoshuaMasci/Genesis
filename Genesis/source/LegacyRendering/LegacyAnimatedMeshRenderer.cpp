#include "Genesis/LegacyRendering/LegacyAnimatedMeshRenderer.hpp"

#include "Genesis/Debug/Log.hpp"

using namespace Genesis;

#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

#define MAX_NUM_JOINTS 128u

struct Node;

struct Vertex 
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv0;
	glm::vec2 uv1;
	glm::vec4 joint0;
	glm::vec4 weight0;
};

struct BoundingBox 
{
	glm::vec3 min;
	glm::vec3 max;
	bool valid = false;
	BoundingBox() {};
	BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
	BoundingBox getAABB(glm::mat4 m) 
	{
		glm::vec3 min = glm::vec3(m[3]);
		glm::vec3 max = min;
		glm::vec3 v0, v1;

		glm::vec3 right = glm::vec3(m[0]);
		v0 = right * this->min.x;
		v1 = right * this->max.x;
		min += glm::min(v0, v1);
		max += glm::max(v0, v1);

		glm::vec3 up = glm::vec3(m[1]);
		v0 = up * this->min.y;
		v1 = up * this->max.y;
		min += glm::min(v0, v1);
		max += glm::max(v0, v1);

		glm::vec3 back = glm::vec3(m[2]);
		v0 = back * this->min.z;
		v1 = back * this->max.z;
		min += glm::min(v0, v1);
		max += glm::max(v0, v1);

		return BoundingBox(min, max);
	}
};

struct Material
{
	enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
	AlphaMode alphaMode = ALPHAMODE_OPAQUE;
	float alphaCutoff = 1.0f;
	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;
	glm::vec4 baseColorFactor = glm::vec4(1.0f);
	glm::vec4 emissiveFactor = glm::vec4(1.0f);
	Texture2D *baseColorTexture;
	Texture2D *metallicRoughnessTexture;
	Texture2D *normalTexture;
	Texture2D *occlusionTexture;
	Texture2D *emissiveTexture;
	struct TexCoordSets 
	{
		uint8_t baseColor = 0;
		uint8_t metallicRoughness = 0;
		uint8_t specularGlossiness = 0;
		uint8_t normal = 0;
		uint8_t occlusion = 0;
		uint8_t emissive = 0;
	} texCoordSets;
	struct Extension 
	{
		Texture2D *specularGlossinessTexture;
		Texture2D *diffuseTexture;
		glm::vec4 diffuseFactor = glm::vec4(1.0f);
		glm::vec3 specularFactor = glm::vec3(0.0f);
	} extension;
	struct PbrWorkflows 
	{
		bool metallicRoughness = true;
		bool specularGlossiness = false;
	} pbrWorkflows;
};

/*
	glTF primitive
*/
struct Primitive 
{
	uint32_t firstIndex;
	uint32_t indexCount;
	uint32_t vertexCount;
	Material &material;
	bool hasIndices;

	BoundingBox bb;

	Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material &material) : firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), material(material) {
		hasIndices = indexCount > 0;
	};

	void setBoundingBox(glm::vec3 min, glm::vec3 max) {
		bb.min = min;
		bb.max = max;
		bb.valid = true;
	}
};

/*
	glTF mesh
*/
struct Mesh 
{
	vector<Primitive*> primitives;
	BoundingBox bb;
	BoundingBox aabb;

	struct UniformBlock 
	{
		glm::mat4 matrix;
		glm::mat4 jointMatrix[MAX_NUM_JOINTS]{};
		float jointcount{ 0 };
	} uniformBlock;

	Mesh(){};

	~Mesh() 
	{
		for (Primitive* p : primitives)
			delete p;
	}

	void setBoundingBox(glm::vec3 min, glm::vec3 max) 
	{
		bb.min = min;
		bb.max = max;
		bb.valid = true;
	}
};

/*
	glTF skin
*/
struct Skin 
{
	std::string name;
	Node *skeletonRoot = nullptr;
	vector<glm::mat4> inverseBindMatrices;
	vector<Node*> joints;
};

/*
	glTF animation channel
*/
struct AnimationChannel 
{
	enum PathType { TRANSLATION, ROTATION, SCALE };
	PathType path;
	Node *node;
	uint32_t samplerIndex;
};

/*
	glTF animation sampler
*/
struct AnimationSampler 
{
	enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
	InterpolationType interpolation;
	std::vector<float> inputs;
	std::vector<glm::vec4> outputsVec4;
};

/*
	glTF animation
*/
struct Animation 
{
	std::string name;
	std::vector<AnimationSampler> samplers;
	std::vector<AnimationChannel> channels;
	float start = std::numeric_limits<float>::max();
	float end = std::numeric_limits<float>::min();
};

/*
	glTF node
*/
struct Node 
{
	Node *parent;
	uint32_t index;
	vector<Node*> children;
	glm::mat4 matrix;
	std::string name;
	Mesh *mesh;
	Skin *skin;
	int32_t skinIndex = -1;
	glm::vec3 translation{};
	glm::vec3 scale{ 1.0f };
	glm::quat rotation{};
	BoundingBox bvh;
	BoundingBox aabb;

	glm::mat4 localMatrix() 
	{
		return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
	}

	glm::mat4 getMatrix() 
	{
		glm::mat4 m = localMatrix();
		Node *p = parent;
		while (p) 
		{
			m = p->localMatrix() * m;
			p = p->parent;
		}
		return m;
	}

	void update() 
	{
		if (mesh) 
		{
			glm::mat4 m = getMatrix();
			mesh->uniformBlock.matrix = m;
			if (skin) 
			{
				// Update join matrices
				glm::mat4 inverseTransform = glm::inverse(m);
				size_t numJoints = std::min((uint32_t)skin->joints.size(), MAX_NUM_JOINTS);
				for (size_t i = 0; i < numJoints; i++) 
				{
					Node *jointNode = skin->joints[i];
					glm::mat4 jointMat = jointNode->getMatrix() * skin->inverseBindMatrices[i];
					jointMat = inverseTransform * jointMat;
					mesh->uniformBlock.jointMatrix[i] = jointMat;
				}
				mesh->uniformBlock.jointcount = (float)numJoints;
				//memcpy(mesh->uniformBuffer.mapped, &mesh->uniformBlock, sizeof(mesh->uniformBlock));
			}
			else 
			{
				//memcpy(mesh->uniformBuffer.mapped, &m, sizeof(glm::mat4));
			}
		}

		for (auto& child : children) 
		{
			child->update();
		}
	}

	~Node() {
		if (mesh) {
			delete mesh;
		}
		for (auto& child : children) 
		{
			delete child;
		}
	}
};

vector<Node*> nodes;
vector<Node*> linearNodes;

vector<Skin*> skins;

vector<Texture2D> textures;
//vector<TextureSampler> textureSamplers;
vector<Material> materials;
vector<Animation> animations;
vector<string> extensions;

Node* findNode(Node *parent, uint32_t index)
{
	Node* nodeFound = nullptr;
	if (parent->index == index) {
		return parent;
	}
	for (auto& child : parent->children) {
		nodeFound = findNode(child, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}

Node* nodeFromIndex(uint32_t index)
{
	Node* nodeFound = nullptr;
	for (auto &node : nodes) {
		nodeFound = findNode(node, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}


void loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, vector<uint32_t>& indexBuffer, vector<Vertex>& vertexBuffer, float globalscale)
{
	Node *newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	newNode->skinIndex = node.skin;
	newNode->matrix = glm::mat4(1.0f);

	// Generate local node matrix
	glm::vec3 translation = glm::vec3(0.0f);
	if (node.translation.size() == 3) 
	{
		translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	glm::mat4 rotation = glm::mat4(1.0f);
	if (node.rotation.size() == 4) 
	{
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = glm::mat4(q);
	}
	glm::vec3 scale = glm::vec3(1.0f);
	if (node.scale.size() == 3) {
		scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
	};

	// Node with children
	if (node.children.size() > 0) {
		for (size_t i = 0; i < node.children.size(); i++) 
		{
			loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer, globalscale);
		}
	}

	// Node contains mesh data
	if (node.mesh > -1)
	{
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		Mesh* newMesh = new Mesh();
		for (size_t j = 0; j < mesh.primitives.size(); j++) 
		{
			const tinygltf::Primitive &primitive = mesh.primitives[j];
			uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
			uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			glm::vec3 posMin{};
			glm::vec3 posMax{};
			bool hasSkin = false;
			bool hasIndices = primitive.indices > -1;
			// Vertices
			{
				const float *bufferPos = nullptr;
				const float *bufferNormals = nullptr;
				const float *bufferTexCoordSet0 = nullptr;
				const float *bufferTexCoordSet1 = nullptr;
				const uint16_t *bufferJoints = nullptr;
				const float *bufferWeights = nullptr;

				int posByteStride;
				int normByteStride;
				int uv0ByteStride;
				int uv1ByteStride;
				int jointByteStride;
				int weightByteStride;

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
				vertexCount = static_cast<uint32_t>(posAccessor.count);
				posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet0 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv0ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}
				if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet1 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv1ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor &jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView &jointView = model.bufferViews[jointAccessor.bufferView];
					bufferJoints = reinterpret_cast<const uint16_t *>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
					jointByteStride = jointAccessor.ByteStride(jointView) ? (jointAccessor.ByteStride(jointView) / sizeof(bufferJoints[0])) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor &weightAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView &weightView = model.bufferViews[weightAccessor.bufferView];
					bufferWeights = reinterpret_cast<const float *>(&(model.buffers[weightView.buffer].data[weightAccessor.byteOffset + weightView.byteOffset]));
					weightByteStride = weightAccessor.ByteStride(weightView) ? (weightAccessor.ByteStride(weightView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				hasSkin = (bufferJoints && bufferWeights);

				for (size_t v = 0; v < posAccessor.count; v++) 
				{
					Vertex vert{};
					vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * posByteStride]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * normByteStride]) : glm::vec3(0.0f)));
					vert.uv0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * uv0ByteStride]) : glm::vec3(0.0f);
					vert.uv1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * uv1ByteStride]) : glm::vec3(0.0f);

					vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * jointByteStride])) : glm::vec4(0.0f);
					vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * weightByteStride]) : glm::vec4(0.0f);
					// Fix for all zero weights
					if (glm::length(vert.weight0) == 0.0f) 
					{
						vert.weight0 = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
					}
					vertexBuffer.push_back(vert);
				}
			}
			// Indices
			if (hasIndices)
			{
				const tinygltf::Accessor &accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

				indexCount = static_cast<uint32_t>(accessor.count);
				const void *dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

				switch (accessor.componentType) 
				{
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: 
				{
					const uint32_t *buf = static_cast<const uint32_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) 
					{
						indexBuffer.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: 
				{
					const uint16_t *buf = static_cast<const uint16_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) 
					{
						indexBuffer.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: 
				{
					const uint8_t *buf = static_cast<const uint8_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) 
					{
						indexBuffer.push_back(buf[index] + vertexStart);
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			Primitive *newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
			newPrimitive->setBoundingBox(posMin, posMax);
			newMesh->primitives.push_back(newPrimitive);
		}
		// Mesh BB from BBs of primitives
		for (auto p : newMesh->primitives) 
		{
			if (p->bb.valid && !newMesh->bb.valid) 
			{
				newMesh->bb = p->bb;
				newMesh->bb.valid = true;
			}
			newMesh->bb.min = glm::min(newMesh->bb.min, p->bb.min);
			newMesh->bb.max = glm::max(newMesh->bb.max, p->bb.max);
		}
		newNode->mesh = newMesh;
	}
	if (parent) 
	{
		parent->children.push_back(newNode);
	}
	else 
	{
		nodes.push_back(newNode);
	}
	linearNodes.push_back(newNode);
}

void loadMaterials(tinygltf::Model &gltfModel)
{
	for (tinygltf::Material &mat : gltfModel.materials) {
		Material material{};
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			material.baseColorTexture = &textures[mat.values["baseColorTexture"].TextureIndex()];
			material.texCoordSets.baseColor = mat.values["baseColorTexture"].TextureTexCoord();
		}
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.metallicRoughnessTexture = &textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
			material.texCoordSets.metallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.normalTexture = &textures[mat.additionalValues["normalTexture"].TextureIndex()];
			material.texCoordSets.normal = mat.additionalValues["normalTexture"].TextureTexCoord();
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.emissiveTexture = &textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
			material.texCoordSets.emissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.occlusionTexture = &textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
			material.texCoordSets.occlusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
		}
		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND") {
				material.alphaMode = Material::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK") {
				material.alphaCutoff = 0.5f;
				material.alphaMode = Material::ALPHAMODE_MASK;
			}
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}
		if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
			material.emissiveFactor = glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
			material.emissiveFactor = glm::vec4(0.0f);
		}

		// Extensions
		// @TODO: Find out if there is a nicer way of reading these properties with recent tinygltf headers
		if (mat.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
			auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
			if (ext->second.Has("specularGlossinessTexture")) {
				auto index = ext->second.Get("specularGlossinessTexture").Get("index");
				material.extension.specularGlossinessTexture = &textures[index.Get<int>()];
				auto texCoordSet = ext->second.Get("specularGlossinessTexture").Get("texCoord");
				material.texCoordSets.specularGlossiness = texCoordSet.Get<int>();
				material.pbrWorkflows.specularGlossiness = true;
			}
			if (ext->second.Has("diffuseTexture")) {
				auto index = ext->second.Get("diffuseTexture").Get("index");
				material.extension.diffuseTexture = &textures[index.Get<int>()];
			}
			if (ext->second.Has("diffuseFactor")) {
				auto factor = ext->second.Get("diffuseFactor");
				for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
					auto val = factor.Get(i);
					material.extension.diffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
				}
			}
			if (ext->second.Has("specularFactor")) {
				auto factor = ext->second.Get("specularFactor");
				for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
					auto val = factor.Get(i);
					material.extension.specularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
				}
			}
		}

		materials.push_back(material);
	}
	// Push a default material at the end of the list for meshes with no material assigned
	materials.push_back(Material());
}

void loadTextures(tinygltf::Model &gltfModel, LegacyBackend* backend)
{
	for (tinygltf::Texture &tex : gltfModel.textures)
	{
		tinygltf::Image image = gltfModel.images[tex.source];

		TextureCreateInfo info = {};
		info.format = (TextureFormat)image.component;
		info.size = { image.width, image.height };
		textures.push_back(backend->createTexture(info, (void*)&image.image[0]));
	}
}

void loadAnimations(tinygltf::Model &gltfModel)
{
	for (tinygltf::Animation &anim : gltfModel.animations) 
	{
		Animation animation{};
		animation.name = anim.name;
		if (anim.name.empty()) 
		{
			animation.name = std::to_string(animations.size());
		}

		// Samplers
		for (auto &samp : anim.samplers) 
		{
			AnimationSampler sampler{};

			if (samp.interpolation == "LINEAR") 
			{
				sampler.interpolation = AnimationSampler::InterpolationType::LINEAR;
			}
			if (samp.interpolation == "STEP") 
			{
				sampler.interpolation = AnimationSampler::InterpolationType::STEP;
			}
			if (samp.interpolation == "CUBICSPLINE") 
			{
				sampler.interpolation = AnimationSampler::InterpolationType::CUBICSPLINE;
			}

			// Read sampler input time values
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[samp.input];
				const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const void *dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
				const float *buf = static_cast<const float*>(dataPtr);
				for (size_t index = 0; index < accessor.count; index++) {
					sampler.inputs.push_back(buf[index]);
				}

				for (auto input : sampler.inputs) {
					if (input < animation.start) {
						animation.start = input;
					};
					if (input > animation.end) {
						animation.end = input;
					}
				}
			}

			// Read sampler output T/R/S values 
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[samp.output];
				const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const void *dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

				switch (accessor.type) {
				case TINYGLTF_TYPE_VEC3: {
					const glm::vec3 *buf = static_cast<const glm::vec3*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) {
						sampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));
					}
					break;
				}
				case TINYGLTF_TYPE_VEC4: {
					const glm::vec4 *buf = static_cast<const glm::vec4*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) {
						sampler.outputsVec4.push_back(buf[index]);
					}
					break;
				}
				default: {
					std::cout << "unknown type" << std::endl;
					break;
				}
				}
			}

			animation.samplers.push_back(sampler);
		}

		// Channels
		for (auto &source : anim.channels) 
		{
			AnimationChannel channel{};

			if (source.target_path == "rotation") 
			{
				channel.path = AnimationChannel::PathType::ROTATION;
			}
			if (source.target_path == "translation") 
			{
				channel.path = AnimationChannel::PathType::TRANSLATION;
			}
			if (source.target_path == "scale") 
			{
				channel.path = AnimationChannel::PathType::SCALE;
			}
			if (source.target_path == "weights") 
			{
				std::cout << "weights not yet supported, skipping channel" << std::endl;
				continue;
			}
			channel.samplerIndex = source.sampler;
			channel.node = nodeFromIndex(source.target_node);
			if (!channel.node) {
				continue;
			}

			animation.channels.push_back(channel);
		}

		animations.push_back(animation);
	}
}

void updateAnimation(uint32_t index, float time)
{
	if (animations.empty()) 
	{
		std::cout << ".glTF does not contain animation." << std::endl;
		return;
	}
	if (index > static_cast<uint32_t>(animations.size()) - 1) 
	{
		std::cout << "No animation with index " << index << std::endl;
		return;
	}
	Animation &animation = animations[index];

	bool updated = false;
	for (auto& channel : animation.channels) 
	{
		AnimationSampler &sampler = animation.samplers[channel.samplerIndex];
		if (sampler.inputs.size() > sampler.outputsVec4.size()) 
		{
			continue;
		}

		for (size_t i = 0; i < sampler.inputs.size() - 1; i++) 
		{
			if ((time >= sampler.inputs[i]) && (time <= sampler.inputs[i + 1])) 
			{
				float u = std::max(0.0f, time - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);
				if (u <= 1.0f) {
					switch (channel.path) 
					{
					case AnimationChannel::PathType::TRANSLATION:
					{
						glm::vec4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
						channel.node->translation = glm::vec3(trans);
						break;
					}
					case AnimationChannel::PathType::SCALE: 
					{
						glm::vec4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
						channel.node->scale = glm::vec3(trans);
						break;
					}
					case AnimationChannel::PathType::ROTATION: 
					{
						glm::quat q1;
						q1.x = sampler.outputsVec4[i].x;
						q1.y = sampler.outputsVec4[i].y;
						q1.z = sampler.outputsVec4[i].z;
						q1.w = sampler.outputsVec4[i].w;
						glm::quat q2;
						q2.x = sampler.outputsVec4[i + 1].x;
						q2.y = sampler.outputsVec4[i + 1].y;
						q2.z = sampler.outputsVec4[i + 1].z;
						q2.w = sampler.outputsVec4[i + 1].w;
						channel.node->rotation = glm::normalize(glm::slerp(q1, q2, u));
						break;
					}
					}
					updated = true;
				}
			}
		}
	}
	if (updated) 
	{
		for (auto &node : nodes) 
		{
			node->update();
		}
	}
}

void loadSkins(tinygltf::Model &gltfModel)
{
	for (tinygltf::Skin &source : gltfModel.skins) 
	{
		Skin *newSkin = new Skin{};
		newSkin->name = source.name;

		// Find skeleton root node
		if (source.skeleton > -1) 
		{
			newSkin->skeletonRoot = nodeFromIndex(source.skeleton);
		}

		// Find joint nodes
		for (int jointIndex : source.joints) 
		{
			Node* node = nodeFromIndex(jointIndex);
			if (node) 
			{
				newSkin->joints.push_back(nodeFromIndex(jointIndex));
			}
		}

		// Get inverse bind matrices from buffer
		if (source.inverseBindMatrices > -1) 
		{
			const tinygltf::Accessor &accessor = gltfModel.accessors[source.inverseBindMatrices];
			const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
			newSkin->inverseBindMatrices.resize(accessor.count);
			memcpy(newSkin->inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
		}

		skins.push_back(newSkin);
	}
}

#include "Genesis/Platform/FileSystem.hpp"

LegacyAnimatedMeshRenderer::LegacyAnimatedMeshRenderer(LegacyBackend* backend)
{
	this->legacy_backend = backend;

	string file_name = "res/BoxAnimated.gltf";

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF loader;
	string error;
	string warning;

	bool ret = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, file_name);

	if (!error.empty()) 
	{
		GENESIS_ENGINE_ERROR("Error: {}", error);
	}

	if (!warning.empty()) 
	{
		GENESIS_ENGINE_WARNING("Warning: {}", warning);
	}

	if (!ret) 
	{
		GENESIS_ENGINE_CRITICAL("Failed to parse glTF");
	}


	vector<Vertex> vertexBuffer;
	vector<uint32_t> indexBuffer;

	loadTextures(gltfModel, this->legacy_backend);
	loadMaterials(gltfModel);
	// TODO: scene handling with no default scene
	const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
	for (size_t i = 0; i < scene.nodes.size(); i++) 
	{
		const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
		loadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer, 1.0f);
	}
	if (gltfModel.animations.size() > 0) 
	{
		loadAnimations(gltfModel);
	}
	loadSkins(gltfModel);

	for (auto node : linearNodes) 
	{
		// Assign skins
		if (node->skinIndex > -1) 
		{
			node->skin = skins[node->skinIndex];
		}
		// Initial pose
		if (node->mesh) 
		{
			node->update();
		}
	}
	
	size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
	size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
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

	this->vertex_buffer = this->legacy_backend->createVertexBuffer(vertexBuffer.data(), vertexBufferSize, create_info);
	this->index_buffer = this->legacy_backend->createIndexBuffer(indexBuffer.data(), indexBufferSize, IndexType::uint32);
	this->index_count = (uint32_t)indexBuffer.size();

	string vert_data = "";
	string frag_data = "";

	FileSystem::loadFileString("res/shaders_opengl/Animated.vert", vert_data);
	FileSystem::loadFileString("res/shaders_opengl/Animated.frag", frag_data);
	this->program = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
}

LegacyAnimatedMeshRenderer::~LegacyAnimatedMeshRenderer()
{
	this->legacy_backend->destoryShaderProgram(this->program);
	this->legacy_backend->destoryVertexBuffer(this->vertex_buffer);
	this->legacy_backend->destoryIndexBuffer(this->index_buffer);
}

void writeEnvironmentUniform1(LegacyBackend* backend, SceneData* environment)
{
	backend->setUniform3f("environment.camera_position", environment->camera_position);
	backend->setUniform3f("environment.ambient_light", environment->ambient_light);
	backend->setUniformMat4f("environment.view_projection_matrix", environment->view_projection_matrix);
}

void writeTransformUniform1(LegacyBackend* backend, const TransformF& transform)
{
	backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
	backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
}

#include <glm/gtx/string_cast.hpp>

void drawNode(Node *node, LegacyBackend* backend)
{
	if (node->mesh) 
	{
		backend->setUniformMat4f("node.matrix", node->mesh->uniformBlock.matrix);
		for (size_t i = 0; i < node->mesh->uniformBlock.jointcount; i++)
		{
			backend->setUniformMat4f("node.jointMatrix[" + std::to_string(i) + "]", node->mesh->uniformBlock.jointMatrix[i]);
		}
		backend->setUniform1f("node.jointCount", node->mesh->uniformBlock.jointcount);

		for (Primitive* primitive : node->mesh->primitives) 
		{
			backend->drawIndex(primitive->indexCount, primitive->firstIndex);
		}
	}
	for (auto& child : node->children) 
	{
		drawNode(child, backend);
	}
}

void LegacyAnimatedMeshRenderer::drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum)
{
	static float anim_time = 0.0;

	updateAnimation(0, anim_time);
	
	anim_time += (1.0f / 60.0f);
	if (anim_time > animations[0].end)
	{
		anim_time -= animations[0].end;
	}

	this->legacy_backend->bindShaderProgram(this->program);

	TransformF render_transform = TransformF();

	//Environment
	writeEnvironmentUniform1(this->legacy_backend, environment);

	//Matrices
	writeTransformUniform1(this->legacy_backend, render_transform);

	//this->legacy_backend->setUniformTexture("in_texture", 0, textures[0]);

	this->legacy_backend->bindVertexBuffer(this->vertex_buffer);
	this->legacy_backend->bindIndexBuffer(this->index_buffer);
	for (auto& node : nodes)
	{
		drawNode(node, this->legacy_backend);
	}

	this->legacy_backend->bindShaderProgram(nullptr);
}

void LegacyAnimatedMeshRenderer::drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformF& light_transform)
{
}

void LegacyAnimatedMeshRenderer::drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformF& light_transform)
{
}

void LegacyAnimatedMeshRenderer::drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformF& light_transform)
{
}
