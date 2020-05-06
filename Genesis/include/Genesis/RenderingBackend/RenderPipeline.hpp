#pragma once

namespace Genesis
{
	enum class PolygonMode
	{
		Points,
		Lines,
		Triangles
	};

	enum class CullMode
	{
		None,
		Front,
		Back,
		All
	};

	enum class FrontFace
	{
		Clockwise,
		CounterClockwise
	};

	enum class CompareOp
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

	enum class StencilOp
	{
		Keep,
		Zero,
		Replace,
		Increment_Clamp,
		Decrement_Clamp,
		Invert,
		Increment_Wrap,
		Decrement_Wrap
	};

	struct StencilOpState
	{
		StencilOp fail_op = StencilOp::Zero;
		StencilOp pass_op = StencilOp::Zero;
		StencilOp depth_fail_op = StencilOp::Zero;
		CompareOp compare_op = CompareOp::Always;
		uint32_t compare_mask = 0;
		uint32_t write_mask = 0;
		uint32_t reference = 0;
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

	enum class ColorComponentMask
	{
		R = 0x00000001,
		G = 0x00000002,
		RG = 0x00000003,
		B = 0x00000004,
		RB = 0x00000005,
		RG = 0x00000006,
		RGB = 0x00000007,
		A = 0x00000008,
		RA = 0x00000009,
		GA = 0x0000000A,
		RGA = 0x0000000B,
		BA = 0x0000000C,
		RBA = 0x0000000D,
		RGA = 0x0000000E,
		RGBA = 0x0000000F
	};

	enum class LogicOp
	{
		Clear,
		And,
		And_Reverse,
		Copy,
		And_Inverted,
		No_Op,
		Xor,
		Or,
		Nor,
		Equivalent,
		Invert,
		Or_Reverse,
		Copy_Inverted,
		Or_Inverted,
		Nand,
		Set,
	};

	struct ColorBlendAttachmentState
	{
		bool blend_enabled = false;

		BlendFactor src_color_factor = BlendFactor::One;
		BlendFactor dst_color_factor = BlendFactor::Zero;
		BlendOp color_blend_op = BlendOp::None;

		BlendFactor src_alpha_factor = BlendFactor::One;
		BlendFactor dst_alpha_factor = BlendFactor::Zero;
		BlendOp alpha_blend_op = BlendOp::None;
		
		ColorComponentMask color_write_mask = ColorComponentMask::RGBA;
	};

	struct RasterizationState
	{
		bool depth_clamp_enable = false;
		bool rasterizer_discard = false;

		PolygonMode polygon_mode = PolygonMode::Triangles;
		CullMode cull_mode = CullMode::None;
		FrontFace front_face = FrontFace::CounterClockwise;

		bool  depth_bias_enable = false;
		float depth_bias_constantFactor = 0.0f;
		float depth_bias_clamp = 0.0f;
		float depth_bias_slope_factor = 0.0f;
		
		float line_width = 0.0f;
	};

	struct DepthStencilState
	{
		bool depth_test_enable = true;
		bool depth_write_enable = true;
		CompareOp depth_op = CompareOp::Less;

		bool depth_bounds_enable = false;
		float min_depth_bound = 0.0f;
		float max_depth_bound = 0.0f;

		bool stencil_test_enable = false;
		StencilOpState front_op;
		StencilOpState back_op;
	};

	struct ColorBlendState
	{
		bool logic_op_enable = false;
		LogicOp logic_op = LogicOp::Clear;

		uint32_t attachments_count = 0;
		const ColorBlendAttachmentState* attachments = nullptr;
		
		vector4F blend_constants;
	};

	struct RenderPipelineCreateInfo
	{
		RasterizationState rasterization_state;
		DepthStencilState depth_stencil_state;
		ColorBlendState color_blend_state;
	};

}