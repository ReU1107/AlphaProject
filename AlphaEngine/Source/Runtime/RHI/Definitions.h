#pragma once
#include "Core/Common.h"
#include "Types.h"
#include "Declarations.h"

#ifdef _WIN64
#include <Windows.h>
#endif

namespace Alpha
{
	struct RHIInitContext
	{

	};

	struct RHIBufferDesc
	{
		UInt64 stride{ 4 };
		UInt64 size{ 1 };
		ERHIBufferAccess access{ ERHIBufferAccess::Static };
		ERHIBufferBindFlags flags{ ERHIBufferBindFlags::ShaderResource };

	};

	struct RHITextureDesc
	{
		UInt32 width{ 2 };				// 幅
		UInt32 height{ 2 };				// 高さ
		union {
			UInt32 array_count{ 1 };
			UInt32 depth;
		};
		UInt32 mip_level{ 1 };			// ミップマップレベル
		UInt32 multi_sample_count{ 1 };	// マルチサンプルカウント
		UInt32 multi_sample_quality{ 0 };	// マルチサンプルクオリティ
		ERHIResourceDimension dimension{ ERHIResourceDimension::Tex2D };
		ERHIResourceFlags flags{ ERHIResourceFlags::None };			// フラグ
		ERHIResourceFormat format{ ERHIResourceFormat::RGBAUnorm };			// フォーマット
		
		Vector4 clear_value_color{ 0.0f,0.0f,0.0f,1.0f };
		Float clear_value_depth{ 1.0f };
		UInt8 clear_value_stencil{ 0 };

		RHISwapChain* swap_chain{ nullptr };	// SwapChainから作成する場合に指定
		UInt32 back_buffer_index{ 0 };		// SwapChainから作成する場合に指定
	};

	struct RHISwapChainDesc
	{
#ifdef _WIN64
		HWND window_handle;
#endif
		RHICommandQueue* queue{ nullptr };
		UInt32 buffer_count{ 3 };
		UInt32 width{ 1920 };
		UInt32 height{ 1080 };
	};

	struct RHICommandBufferDesc
	{
		UInt32 count{ 3 };
		ERHICommandType type{ ERHICommandType::Direct };

	};

	struct RHICommandQueueDesc
	{
		ERHICommandType type{ ERHICommandType::Direct };
	};

	struct RHICommandSignatureDesc
	{
		struct Argument
		{
			ERHIIndirectArgType type{};

			// type が Constant の場合のみ使用: どの RootParameter を書き換えるか
			UInt32 root_parameter_index{ 0 };
			// type が Constant の場合のみ使用: 書き換える値の数
			UInt32 dest_runtime_constant_offset{ 0 };
			UInt32 num_32bit_values_to_set{ 0 };

			// type が VertexBuffer/IndexBuffer の場合に使用するスロット番号など
			UInt32 slot_index{ 0 };
		};

		std::vector<Argument> arguments{};
		RHIPipelineLayout* layout{ nullptr };
		UInt32 stride{ 0 }; // 1コマンドあたりのバイトサイズ (0なら自動計算)
	};

	struct RHITimelineDesc
	{
	};

	struct RHIShaderDesc
	{
		std::wstring file_directory{};
		ERHIShaderType type{ ERHIShaderType::Vertex };
		std::wstring entry_point{ L"main" };

		std::wstring shader_model{};
		bool enable_debug{ true };
		bool enable_optimize{ false };
		std::vector<std::wstring> includes{};
	};

	struct RHIResourceViewDesc
	{
		RHIResource* resource{ nullptr };
		RHIBuffer* counter{ nullptr };
		ERHIResourceViewType type{ ERHIResourceViewType::Unknown };
		ERHIResourceViewFlags flags{};
		UInt32 buffer_stride{ 0 };			// 
		UInt32 heap_index{ 0 };
	};

	struct RHISamplerStateDesc
	{
		RHIDescriptorHeap* descriptor_heap{ nullptr };
		ERHIFilter filter{ ERHIFilter::Linear };
		ERHIAddressMode address_u{ ERHIAddressMode::Wrap };
		ERHIAddressMode address_v{ ERHIAddressMode::Wrap };
		ERHIAddressMode address_w{ ERHIAddressMode::Wrap };
		Float mip_lod_bias{ 0.0f };
		UInt32 max_anisotropy{ 16 };
		ERHIComparisonFunc comparison_func{ ERHIComparisonFunc::Never };
		Float border_color[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
		Float min_lod{ 0.0f };
		Float max_lod{ 3.402823466e+38f };
		UInt32 heap_index{ 0 };
	};

	struct RHIDescriptorHeapDesc
	{
		ERHIDescriptorType type{ ERHIDescriptorType::SRV_UAV_CBV };
		//struct
		//{
		//	ERHIResourceViewType ViewType;	// 
		//	UInt32 ReservationCount;		// 予約数
		//}Parameters[10];
		UInt64 max_count{ 0 };
	};

	struct RHIDescriptorRange 
	{
		UInt32 resource_num{ 0 };			// 同じリソースが連続する場合の数↓だと 8
		UInt32 register_index{ 0 };		// 開始時の番号 Texture2D tex[8] : register(t5); なら5
		ERHIResourceViewType view_type;
	};

	struct RHIPipelineLayoutDesc
	{
		struct Param
		{
			RHIDescriptorRange ranges[16];		// 
			UInt32 range_num{ 0 };				// この数だけ  DescriptorTableが使用された場合はこの数がRangeの数を表す
			UInt32 register_index{ 0 };			// この番号は
			ERHIResourceViewType view_type;		// この種類のリソースをバインドする
			ERHIShaderFlags shader_flags;		// 使用するシェーダーはこれ
			ERHIParameterType parameter_type;	// 
		};
		TArray<Param> parameters;

		ERHIPipelineLayoutFlags flags{};
	};

	struct RHIRasterizerStateDesc
	{
		ERHIFillMode fill_mode{ ERHIFillMode::Solid };
		ERHICullMode cull_mode{ ERHICullMode::Back };
		Bool front_counter_clockwise{ false };
		Int32 depth_bias{ 0 };
		Float depth_bias_clamp{ 0.0f };
		Float slope_scaled_depth_bias{ 0.0f };
		Bool depth_clip_enable{ true };
		Bool multisample_enable{ false };
		Bool antialiased_line_enable{ false };
		UInt32 forced_sample_count{ 0 };
		Bool conservative_raster_enable{ false };
	};

	struct RHIVertexElementDesc
	{
		String semantic_name{L""};				// セマンティクス名
		UInt32 semantic_index{ 0 };				// セマンティクス番号(名前が被った時に使用する基本的には０)
		ERHIResourceFormat format{ ERHIResourceFormat::RGBFloat };				// フォーマット
		UInt32 input_slot{ 0 };					// bind番号
		UInt32 aligned_offset{ 0xffffffff };				// 基本0xffffffff(自動計算)
		//InputClassification classification;		// 分類

		//UInt32 instanceCount;					// インスタンスの時に使用(それ以外の時は0固定)
	};

	struct RHIVertexLayoutDesc
	{
		RHIVertexElementDesc elements[16]{};		// 項目
		UInt32 element_count{ 0 };				// 項目数
		UInt32 stride{ 0 };						// 項目のストライド(自動計算されるので基本0)
	};

	struct RHIBlendStateDesc
	{
		Bool blend_enable{ false };
		Bool logic_op_enable{ false };

		ERHIBlend src_blend{ ERHIBlend::One };
		ERHIBlend dest_blend{ ERHIBlend::Zero };
		ERHIBlendOp blend_op{ ERHIBlendOp::Add };

		ERHIBlend src_blend_alpha{ ERHIBlend::One };
		ERHIBlend dest_blend_alpha{ ERHIBlend::Zero };
		ERHIBlendOp blend_op_alpha{ ERHIBlendOp::Add };

		UInt8 write_mask{ 0x0F };
	};

	struct RHIGraphicsPipelineDesc
	{
		RHIPipelineLayout* layout{ nullptr };
		RHIShader* vertex_shader{ nullptr };
		RHIShader* hull_shader{ nullptr };
		RHIShader* domain_shader{ nullptr };
		RHIShader* geometry_shader{ nullptr };
		RHIShader* pixel_shader{ nullptr };
		RHIShader* task_shader{ nullptr };
		RHIShader* mesh_shader{ nullptr };
		RHIDescriptorView* view{ nullptr };
		RHIVertexLayoutDesc vertex_layout{};
		RHIRasterizerStateDesc rasterizer_state{};
		ERHIResourceFormat render_targets[8]{};
		UInt32 render_target_count{ 0 };
		RHIBlendStateDesc blend_states[8]{};

		ERHIResourceFormat depth_format{ ERHIResourceFormat::Depth24Stencil8 };

		ERHIFillMode fill_mode{ ERHIFillMode::Solid };
		ERHICullMode cull_mode{ ERHICullMode::Back };
		ERHIPrimitiveTopology primitive_topology{ ERHIPrimitiveTopology::TriangleList };

		Bool alpha_to_coverage_enable{ false };
		Bool independent_blend_enable{ false };
		Bool depth_enable{ false };
	};

	struct RHIComputePipelineDesc
	{
		RHIPipelineLayout* layout{ nullptr };
		RHIShader* compute_shader{ nullptr };
		RHIDescriptorView* View{ nullptr };
	};

	struct RHIWorkGraphPipelineDesc
	{
		RHIPipelineLayout* layout{ nullptr };
		RHIShader* compute_shader{ nullptr };
		RHIDescriptorView* View{ nullptr };
	};

	struct RHIRayTracingPipelineDesc
	{
		RHIShader* RayTracingShader{ nullptr };
		RHIDescriptorView* View{ nullptr };
	};
}