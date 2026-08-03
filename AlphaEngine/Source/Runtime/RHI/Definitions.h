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
		ERHIResourceAccessType access{ ERHIResourceAccessType::Static };
		ERHIResourceBindFlags flags{ ERHIResourceBindFlags::ShaderResource };

	};

	struct RHITextureDesc
	{
		UInt32 width{ 2 };				// 幅
		UInt32 height{ 2 };				// 高さ
		UInt32 depth{ 1 };				// 配列数と同じ
		UInt32 mip_level{ 1 };			// ミップマップレベル
		UInt32 multi_sample_count{ 1 };	// マルチサンプルカウント
		UInt32 multi_sample_quality{ 0 };	// マルチサンプルクオリティ
		ERHIResourceDimension dimension{ ERHIResourceDimension::Tex2D };
		ERHIResourceBindFlags flags{ ERHIResourceBindFlags::None };			// フラグ
		ERHIResourceFormat format{ ERHIResourceFormat::RGBAUnorm };			// フォーマット
		
		Vector4 clear_value_color{ 0.0f,0.0f,0.0f,1.0f };
		Float clear_value_depth{ 0.0f };
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
		Int32 depth_bias{ 0 };
		Float depth_bias_clamp{ 0.0f };
		Float slope_scaled_depth_bias{ 0.0f };
		UInt32 forced_sample_count{ 0 };
		Bool front_counter_clockwise{ false };
		Bool depth_clip_enable{ true };
		Bool multisample_enable{ false };
		Bool antialiased_line_enable{ false };
		Bool conservative_raster_enable{ false };
	};

	struct RHIVertexElementDesc
	{
		String semantic_name{L""};				// セマンティクス名
		UInt32 semantic_index{ 0 };				// セマンティクス番号(名前が被った時に使用する基本的には0)
		ERHIResourceFormat format{ ERHIResourceFormat::RGBFloat };				// フォーマット
		UInt32 input_slot{ 0 };					// bind番号
		UInt32 aligned_offset{ 0xffffffff };				// 基本0xffffffff(自動計算)
	};

	struct RHIVertexLayoutDesc
	{
		RHIVertexElementDesc elements[16]{};		// 項目
		UInt32 element_count{ 0 };				// 項目数
		UInt32 stride{ 0 };						// 項目のストライド(自動計算されるので基本0)
	};

	struct RHIBlendStateDesc
	{
		ERHIBlend src_blend{ ERHIBlend::One };
		ERHIBlend dest_blend{ ERHIBlend::Zero };
		ERHIBlendOp blend_op{ ERHIBlendOp::Add };

		ERHIBlend src_blend_alpha{ ERHIBlend::One };
		ERHIBlend dest_blend_alpha{ ERHIBlend::Zero };
		ERHIBlendOp blend_op_alpha{ ERHIBlendOp::Add };

		Bool blend_enable{ false };
		Bool logic_op_enable{ false };
		UInt8 write_mask{ 0x0F };
	};

	struct RHIComputeSettings 
	{
		RHIShader* compute_shader{ nullptr };
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
		//RHIDescriptorView* View{ nullptr };		
	};

	struct RHIGraphicsStateDesc
	{
		RHIRasterizerStateDesc rasterizer{};
		RHIBlendStateDesc blend_states[8]{};
		ERHIResourceFormat render_targets[8]{};
		UInt32 render_target_count{ 0 };
		ERHIResourceFormat depth_format{ ERHIResourceFormat::Depth24Stencil8 };

		ERHIPrimitiveTopology primitive_topology{ ERHIPrimitiveTopology::TriangleList };


		ERHIFillMode fill_mode{ ERHIFillMode::Solid };
		ERHICullMode cull_mode{ ERHICullMode::Back };

		Bool alpha_to_coverage_enable{ false };
		Bool independent_blend_enable{ false };
		Bool depth_enable{ false };
	};


	struct RHIWorkGraphNodeDesc
	{
		String name{};                              // HLSL側の [NodeID] と対応させる名前
		ERHIWorkGraphNodeType node_type{ ERHIWorkGraphNodeType::Compute };
		Bool is_entrypoint{ false };                 // DispatchGraph()から直接呼べるか

		RHIShader* shader{ nullptr };                // Compute Nodeのシェーダー
		RHIShader* mesh_shader{ nullptr };           // Mesh Nodeのメッシュシェーダー
		RHIShader* pixel_shader{ nullptr };          // Mesh Nodeのピクセルシェーダー

		RHIGraphicsStateDesc graphics_state{};       // node_type == Meshの時だけ使う
		RHIPipelineLayout* local_layout{ nullptr };   // ノードごとのLocal Root Signature
	};

	struct RHIDynamicPipelineDesc
	{
		String graph_name{ L"WorkGraph" };
		RHIPipelineLayout* global_layout{ nullptr };
		TArray<RHIShader*> libraries{};
		TArray<RHIWorkGraphNodeDesc> nodes{};
	};

	struct RHIRayTraceHitGroupDesc
	{
		String name{};
		ERHIHitGroupType type{ ERHIHitGroupType::Triangles };
		RHIShader* any_hit_shader{ nullptr };
		RHIShader* closest_hit_shader{ nullptr };
		RHIShader* intersection_shader{ nullptr }; // Proceduralの時のみ
	};

	struct RHIRayTracePipelineDesc
	{
		RHIPipelineLayout* global_layout{ nullptr };
		TArray<RHIShader*> libraries{};

		RHIShader* ray_gen_shader{ nullptr };
		TArray<RHIShader*> miss_shaders{};
		TArray<RHIRayTraceHitGroupDesc> hit_groups{};

		UInt32 max_payload_size{ 0 };
		UInt32 max_attribute_size{ 0 };
		UInt32 max_recursion_depth{ 1 };
	};

	struct RHIBottomLevelGeometryDesc
	{
		UInt64 index_byte_offset{};
		UInt32 index_count{};
		bool is_opaque{ false };
	};

	struct RHIASMemoryInfo
	{
		UInt64 size{ 0 };
		UInt64 scratch_size{ 0 };
		UInt64 update_size{ 0 };
	};

	struct RHIBuildBLASDesc
	{
		RHIBuffer* vertex_buffer{ nullptr };
		RHIBuffer* index_buffer{ nullptr };
		RHIBuffer* result_as_buffer{ nullptr };
		RHIBuffer* scratch_as_buffer{ nullptr };
		RHIBuffer* source_as_buffer{ nullptr };

		UInt64 vertex_offset{};
		UInt64 index_offset{};
		UInt64 result_as_offset{};
		UInt64 scratch_as_offset{};
		UInt64 source_as_offset{};

		TArray<RHIBottomLevelGeometryDesc> geometries{};

		UInt32 vertex_count{ 0 };
		
	};

	struct RHIBuildTLASDesc
	{
		RHIBuffer* instance_desc_buffer{ nullptr }; // 
		RHIBuffer* result_as_buffer{ nullptr };
		RHIBuffer* scratch_as_buffer{ nullptr };

		UInt32 instance_count{ 0 };
	};

	struct RHIRayTraceDesc
	{
		RHIBuffer* shader_table_buffer{ nullptr }; // 

		UInt64 ray_gen_offset{};
		UInt64 miss_offset{};
		UInt64 hit_group_offset{};

		UInt64 miss_count{1};
		UInt64 hit_group_count{ 1 };


		UInt32 width{ 0 };
		UInt32 height{ 0 };
		UInt32 depth{ 0 };
	};

}