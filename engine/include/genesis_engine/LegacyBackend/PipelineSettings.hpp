#pragma once

#include "genesis_core/hash/murmur_hash.hpp"

namespace genesis_engine
{
	enum class CullMode
	{
		None,
		Front,
		Back,
		All 
	};

	enum class DepthTest
	{
		None,
		Test_Only,
		Test_And_Write
	};

	enum class DepthOp
	{
		Never,
		Less,
		Equal,
		Less_Equal,
		Greater,
		Not_Equal,
		Greater_Equal,
		Always
	};

	enum class BlendFactor
	{
		Zero,
		One,
		Color_Src,
		One_Minus_Color_Src,
		Color_Dst,
		One_Minus_Color_Dst,
		Alpha_Src,
		One_Minus_Alpha_Src,
		Alpha_Dst,
		One_Minus_Alpha_Dst
	};

	enum class BlendOp
	{
		None,
		Add,
		Subtract,
		Reverse_Subtract,
		Min,
		Max
	};

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
			genesis_core::MurmurHash2 hash;
			hash.begin();
			hash.add(*this);
			return hash.end();
		};
	};

}