#pragma once

namespace Genesis
{
	typedef uint32_t PipelineSettingsHash;

	static const PipelineSettingsHash CullModeMask = 0x03;
	enum class CullMode
	{
		None = 0x00,
		Front = 0x01,
		Back = 0x02,
		All  = 0x03
	};

	static const PipelineSettingsHash DepthTestMask = 0x0C;
	enum class DepthTest
	{
		None = 0x00,
		Test_Only = 0x04,
		Test_And_Write = 0x08
	};

	static const PipelineSettingsHash DepthOpMask = 0x70;
	enum class DepthOp
	{
		Never = 0x00,
		Less = 0x10,
		Equal = 0x20,
		Less_Equal = 0x30,
		Greater = 0x40,
		Not_Equal = 0x50,
		Greater_Equal = 0x60,
		Always = 0x70
	};
	static const uint16_t DepthTestOpBitSize = 3;

	struct PipelineSettings
	{
		CullMode cull_mode = CullMode::Back;
		DepthTest depth_test = DepthTest::Test_And_Write;
		DepthOp depth_op = DepthOp::Less;

		PipelineSettingsHash getHash()
		{
			switch (cull_mode)
			{
			case Genesis::CullMode::None:
				break;
			case Genesis::CullMode::Front:
				break;
			case Genesis::CullMode::Back:
				break;
			default:
				break;
			}

			PipelineSettingsHash hash = 0x0000;
			hash |= ((PipelineSettingsHash)this->cull_mode & CullModeMask);
			hash |= ((PipelineSettingsHash)this->depth_test & DepthTestMask);
			hash |= ((PipelineSettingsHash)this->depth_op & DepthOpMask);
			return hash;
		};
	};

}