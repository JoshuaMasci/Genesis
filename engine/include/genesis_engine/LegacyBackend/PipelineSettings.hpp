#pragma once

#include "genesis_core/hash/murmur_hash.hpp"
#include "genesis_core/rendering/pipeline.hpp"

namespace genesis
{
	struct PipelineSettings
	{
		CullMode cull_mode = CullMode::Back;
		
		DepthTest depth_test = DepthTest::Test_And_Write;
		DepthOp depth_op = DepthOp::Less;

		BlendOp blend_op = BlendOp::None;
		BlendFactor src_factor = BlendFactor::One;
		BlendFactor dst_factor = BlendFactor::Zero;

		uint32_t getHash()
		{
			genesis::MurmurHash2 hash;
			hash.begin();
			hash.add(*this);
			return hash.end();
		};
	};

}