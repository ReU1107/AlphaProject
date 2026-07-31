#include "D3D12GraphicsPipeline.h"
#include "D3D12PipelineLayout.h"
#include "D3D12Shader.h"
#include "D3D12Texture.h"
#include "RHI/Definitions.h"
#include "D3D12Util.h"
#include "Core/Math/MathUtil.h"


#if RHI_D3D12

namespace
{
	static DXGI_FORMAT GetDXGIFormat(Alpha::ERHIResourceFormat format)
	{
		switch (format)
		{
		case Alpha::ERHIResourceFormat::RGBAUnorm		: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Alpha::ERHIResourceFormat::Depth24Stencil8	: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case Alpha::ERHIResourceFormat::Depth16			: return DXGI_FORMAT_D16_UNORM;
		case Alpha::ERHIResourceFormat::Depth32			: return DXGI_FORMAT_D32_FLOAT;
		case Alpha::ERHIResourceFormat::RGBFloat		: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Alpha::ERHIResourceFormat::RGBAFloat		: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Alpha::ERHIResourceFormat::RGFloat			: return DXGI_FORMAT_R32G32_FLOAT;
		case Alpha::ERHIResourceFormat::RGBA32UInt		: return DXGI_FORMAT_R32G32B32A32_UINT;

		default:
			return DXGI_FORMAT_UNKNOWN;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetD3D12PrimitiveTopologyType(Alpha::ERHIPrimitiveTopology topology)
	{
		switch (topology)
		{
		case Alpha::ERHIPrimitiveTopology::TriangleList:	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case Alpha::ERHIPrimitiveTopology::LineList:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case Alpha::ERHIPrimitiveTopology::PointList:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}

		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}

	static D3D12_FILL_MODE GetD3D12FillMode(Alpha::ERHIFillMode fill_mode)
	{
		switch (fill_mode)
		{
		case Alpha::ERHIFillMode::Solid:		return D3D12_FILL_MODE_SOLID;
		case Alpha::ERHIFillMode::Wireframe:	return D3D12_FILL_MODE_WIREFRAME;
		default:
			return D3D12_FILL_MODE_SOLID;
		}
		return D3D12_FILL_MODE_SOLID;
	}

	static D3D12_CULL_MODE GetD3D12CullMode(Alpha::ERHICullMode cull_mode)
	{
		switch (cull_mode)
		{
		case Alpha::ERHICullMode::None:		return D3D12_CULL_MODE_NONE;
		case Alpha::ERHICullMode::Front:	return D3D12_CULL_MODE_FRONT;
		case Alpha::ERHICullMode::Back:		return D3D12_CULL_MODE_BACK;
		default:
			return D3D12_CULL_MODE_BACK;
		}
		return D3D12_CULL_MODE_BACK;
	}

	static D3D12_BLEND GetD3D12Blend(Alpha::ERHIBlend blend_mode)
	{
		switch (blend_mode)
		{
		case Alpha::ERHIBlend::Zero				: return D3D12_BLEND_ZERO;
		case Alpha::ERHIBlend::One				: return D3D12_BLEND_ONE;
		case Alpha::ERHIBlend::SrcColor			: return D3D12_BLEND_SRC_COLOR;
		case Alpha::ERHIBlend::InvSrcColor		: return D3D12_BLEND_INV_SRC_COLOR;
		case Alpha::ERHIBlend::SrcAlpha			: return D3D12_BLEND_SRC_ALPHA;
		case Alpha::ERHIBlend::InvSrcAlpha		: return D3D12_BLEND_INV_SRC_ALPHA;
		case Alpha::ERHIBlend::DestAlpha		: return D3D12_BLEND_DEST_ALPHA;
		case Alpha::ERHIBlend::InvDestAlpha		: return D3D12_BLEND_INV_DEST_ALPHA;
		case Alpha::ERHIBlend::DestColor		: return D3D12_BLEND_DEST_COLOR;
		case Alpha::ERHIBlend::InvDestColor		: return D3D12_BLEND_INV_DEST_COLOR;
		case Alpha::ERHIBlend::SrcAlphaSat		: return D3D12_BLEND_SRC_ALPHA_SAT;
		case Alpha::ERHIBlend::BlendFactor		: return D3D12_BLEND_BLEND_FACTOR;
		case Alpha::ERHIBlend::InvBlendFactor	: return D3D12_BLEND_INV_BLEND_FACTOR;
		default:
			break;
		}
		return D3D12_BLEND_ZERO;;
	}

	static D3D12_BLEND_OP GetD3D12BlendOp(Alpha::ERHIBlendOp blend_op)
	{
		switch (blend_op)
		{
		case Alpha::ERHIBlendOp::Add		: return D3D12_BLEND_OP_ADD;
		case Alpha::ERHIBlendOp::Subtract	: return D3D12_BLEND_OP_SUBTRACT;
		case Alpha::ERHIBlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case Alpha::ERHIBlendOp::Min		: return D3D12_BLEND_OP_MIN;
		case Alpha::ERHIBlendOp::Max		: return D3D12_BLEND_OP_MAX;
		default:
			break;
		}
		return D3D12_BLEND_OP_ADD;
	}

	static D3D12_RENDER_TARGET_BLEND_DESC GetBlendDesc(Alpha::RHIBlendStateDesc d)
	{
		D3D12_RENDER_TARGET_BLEND_DESC desc =
		{
			.BlendEnable = d.blend_enable,
			.LogicOpEnable = d.logic_op_enable,
			.SrcBlend = GetD3D12Blend(d.src_blend),
			.DestBlend = GetD3D12Blend(d.dest_blend),
			.BlendOp = GetD3D12BlendOp(d.blend_op),
			.SrcBlendAlpha = GetD3D12Blend(d.src_blend_alpha),
			.DestBlendAlpha = GetD3D12Blend(d.dest_blend_alpha),
			.BlendOpAlpha = GetD3D12BlendOp(d.blend_op_alpha),
			.LogicOp = D3D12_LOGIC_OP_NOOP,
			.RenderTargetWriteMask = d.write_mask
		};
		return desc;
	}

	static D3D12_RASTERIZER_DESC GetDefaultRasterizerDesc()
	{
		D3D12_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_BACK;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = TRUE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		desc.ForcedSampleCount = 0;
		desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		return desc;
	}

}


namespace Alpha
{
	D3D12GraphicsPipeline::D3D12GraphicsPipeline(D3D12Device* device)
		: RHIGraphicsPipeline()
		, D3D12DeviceChild(device)
	{
	}

	D3D12GraphicsPipeline::~D3D12GraphicsPipeline()
	{
	}

	bool D3D12GraphicsPipeline::Initialize(const RHIGraphicsPipelineDesc& desc)
	{
		auto device = GetD3D12Device();

		std::vector<uint8_t> stream_data;
		stream_data.reserve(1024);

		auto pipeline_layout = static_cast<D3D12PipelineLayout*>(desc.layout);

		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, ID3D12RootSignature*> root_signature;
		root_signature.data = pipeline_layout->GetNativeHandle();

		AddSubobject(stream_data, root_signature);

		// Pixel Shader (共通)
		if (desc.pixel_shader)
		{
			auto ps = static_cast<D3D12Shader*>(desc.pixel_shader);

			PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS, D3D12_SHADER_BYTECODE> ps_sub;
			ps_sub.data = { ps->GetBytecodePointer(), ps->GetBytecodeLength() };

			AddSubobject(stream_data, ps_sub);
		}

		D3D12_INPUT_ELEMENT_DESC input_elements[D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT] = {};
		std::vector<std::string> semantic_names = {};

		if (desc.mesh_shader)
		{
			// Mesh Shader
			auto ms = static_cast<D3D12Shader*>(desc.mesh_shader);
			PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS, D3D12_SHADER_BYTECODE> ms_sub;
			ms_sub.data = { ms->GetBytecodePointer(), ms->GetBytecodeLength() };
			AddSubobject(stream_data, ms_sub);

			// Amplification Shader (Task Shader)
			if (desc.task_shader)
			{
				auto ts = static_cast<D3D12Shader*>(desc.task_shader);
				PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS, D3D12_SHADER_BYTECODE> as_sub;
				as_sub.data = { ts->GetBytecodePointer(), ts->GetBytecodeLength() };
				AddSubobject(stream_data, as_sub);
			}
		}
		else
		{
			// Vertex Shader
			if (desc.vertex_shader)
			{
				auto vs = static_cast<D3D12Shader*>(desc.vertex_shader);
				PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS, D3D12_SHADER_BYTECODE> vs_sub;
				vs_sub.data = { vs->GetBytecodePointer(), vs->GetBytecodeLength() };
				AddSubobject(stream_data, vs_sub);
			}

			// Hull Shader
			if (desc.hull_shader)
			{
				auto hs = static_cast<D3D12Shader*>(desc.hull_shader);
				PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS, D3D12_SHADER_BYTECODE> hs_sub;
				hs_sub.data = { hs->GetBytecodePointer(), hs->GetBytecodeLength() };
				AddSubobject(stream_data, hs_sub);
			}

			// Domain Shader
			if (desc.domain_shader)
			{
				auto ds = static_cast<D3D12Shader*>(desc.domain_shader);
				PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS, D3D12_SHADER_BYTECODE> ds_sub;
				ds_sub.data = { ds->GetBytecodePointer(), ds->GetBytecodeLength() };
				AddSubobject(stream_data, ds_sub);
			}

			// Geometry Shader
			if (desc.geometry_shader)
			{
				auto gs = static_cast<D3D12Shader*>(desc.geometry_shader);
				PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS, D3D12_SHADER_BYTECODE> gs_sub;
				gs_sub.data = { gs->GetBytecodePointer(), gs->GetBytecodeLength() };
				AddSubobject(stream_data, gs_sub);
			}

			// Input Layout
			semantic_names.resize(desc.vertex_layout.element_count);

			for (UInt32 i = 0; i < desc.vertex_layout.element_count; i++)
			{
				const auto& src = desc.vertex_layout.elements[i];
				auto& dst = input_elements[i];
				semantic_names[i] = src.semantic_name.ToUtf8();
				dst.SemanticName = semantic_names[i].c_str();
				dst.SemanticIndex = src.semantic_index;
				dst.Format = GetDXGIFormat(src.format);
				dst.InputSlot = src.input_slot;
				dst.AlignedByteOffset = src.aligned_offset;
				dst.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				dst.InstanceDataStepRate = 0;
			}

			PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT, D3D12_INPUT_LAYOUT_DESC> il_sub;
			il_sub.data = { input_elements, desc.vertex_layout.element_count };
			AddSubobject(stream_data, il_sub);

			// Primitive Topology
			PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY, D3D12_PRIMITIVE_TOPOLOGY_TYPE> topo_sub;
			topo_sub.data = GetD3D12PrimitiveTopologyType(desc.primitive_topology);
			AddSubobject(stream_data, topo_sub);
		}
		// ラスタライザー
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER, D3D12_RASTERIZER_DESC> rs_sub;
		rs_sub.data = GetDefaultRasterizerDesc();
		rs_sub.data.FillMode = GetD3D12FillMode(desc.fill_mode);
		rs_sub.data.CullMode = GetD3D12CullMode(desc.cull_mode);
		rs_sub.data.FrontCounterClockwise = desc.rasterizer_state.front_counter_clockwise ? true : false;
		rs_sub.data.DepthBias = desc.rasterizer_state.depth_bias;
		rs_sub.data.DepthBiasClamp = desc.rasterizer_state.depth_bias_clamp;
		rs_sub.data.SlopeScaledDepthBias = desc.rasterizer_state.slope_scaled_depth_bias;
		rs_sub.data.DepthClipEnable = desc.rasterizer_state.depth_clip_enable ? true : false;
		rs_sub.data.MultisampleEnable = desc.rasterizer_state.multisample_enable ? true : false;
		rs_sub.data.AntialiasedLineEnable = desc.rasterizer_state.antialiased_line_enable ? true : false;
		rs_sub.data.ForcedSampleCount = desc.rasterizer_state.forced_sample_count;
		rs_sub.data.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		AddSubobject(stream_data, rs_sub);

		// デプスステンシルフォーマット
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT, DXGI_FORMAT> dsv_fmt_sub;
		dsv_fmt_sub.data = GetDXGIFormat(desc.depth_format);
		AddSubobject(stream_data, dsv_fmt_sub);

		// デプスステンシルステート
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL, D3D12_DEPTH_STENCIL_DESC> ds_sub;
		ds_sub.data.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		ds_sub.data.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		ds_sub.data.DepthEnable = desc.depth_enable;
		ds_sub.data.StencilEnable = false;
		AddSubobject(stream_data, ds_sub);

		// ブレンドステート & RTVフォーマット
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND, D3D12_BLEND_DESC> blend_sub = {};
		blend_sub.data.AlphaToCoverageEnable = desc.alpha_to_coverage_enable;
		blend_sub.data.IndependentBlendEnable = desc.independent_blend_enable;

		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS, D3D12_RT_FORMAT_ARRAY> rtv_fmts_sub = {};
		rtv_fmts_sub.data.NumRenderTargets = desc.render_target_count;

		for (UInt32 i = 0; i < desc.render_target_count; i++)
		{
			rtv_fmts_sub.data.RTFormats[i] = GetDXGIFormat(desc.render_targets[i]);
			blend_sub.data.RenderTarget[i] = GetBlendDesc(desc.blend_states[i]);
		}
		AddSubobject(stream_data, rtv_fmts_sub);
		AddSubobject(stream_data, blend_sub);

		// マルチサンプル設定
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, DXGI_SAMPLE_DESC> sample_sub;
		sample_sub.data.Count = 1;
		sample_sub.data.Quality = 0;
		AddSubobject(stream_data, sample_sub);

		// サンプルマスク
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, UINT> mask_sub;
		mask_sub.data = UINT_MAX;
		AddSubobject(stream_data, mask_sub);

		// ノードマスク
		PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, UINT> node_sub;
		node_sub.data = 0;
		AddSubobject(stream_data, node_sub);

		D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {};
		stream_desc.SizeInBytes = stream_data.size();
		stream_desc.pPipelineStateSubobjectStream = stream_data.data();

		HRESULT hr = device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&m_pipeline_state));
		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"グラフィックスパイプラインの作成に失敗しました"))
		{
			return false;
		}
		return true;
	}
}

#endif

/*
* MeshShaderに移行するにあたってこれまでのコードは供養っていうことでコメントアウトで残してはおく
* 	auto device = GetD3D12Device();

		auto pipeline_layout = static_cast<D3D12PipelineLayout*>(desc.layout);
		auto vertex_shader = static_cast<D3D12Shader*>(desc.vertex_shader);
		auto pixel_shader = static_cast<D3D12Shader*>(desc.pixel_shader);
		auto hull_shader = static_cast<D3D12Shader*>(desc.hull_shader);
		auto domain_shader = static_cast<D3D12Shader*>(desc.domain_shader);
		auto geometry_shader = static_cast<D3D12Shader*>(desc.geometry_shader);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};
		pipeline_state_desc.pRootSignature = pipeline_layout->GetNativeHandle();

		if (vertex_shader)
		{
			pipeline_state_desc.VS = { vertex_shader->GetBytecodePointer(), vertex_shader->GetBytecodeLength() };
		}
		if(pixel_shader)
		{
			pipeline_state_desc.PS = { pixel_shader->GetBytecodePointer(), pixel_shader->GetBytecodeLength() };
		}

		if(hull_shader)
		{
			pipeline_state_desc.HS = { hull_shader->GetBytecodePointer(), hull_shader->GetBytecodeLength() };
		}

		if(geometry_shader)
		{
			pipeline_state_desc.GS = { geometry_shader->GetBytecodePointer(), geometry_shader->GetBytecodeLength() };
		}

		if(domain_shader)
		{
			pipeline_state_desc.DS = { domain_shader->GetBytecodePointer(), domain_shader->GetBytecodeLength() };
		}

		pipeline_state_desc.PrimitiveTopologyType = GetD3D12PrimitiveTopologyType(desc.primitive_topology);

		D3D12_INPUT_ELEMENT_DESC input_elements[D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT] = {};
		std::vector<std::string> semantic_names = {};
		semantic_names.reserve(desc.vertex_layout.element_count);
		for (UInt32 element_index = 0; element_index < desc.vertex_layout.element_count; element_index++)
		{
			const auto& src_element = desc.vertex_layout.elements[element_index];
			auto& dst_element = input_elements[element_index];

			semantic_names.push_back(src_element.semantic_name.ToUtf8());

			dst_element.SemanticName = semantic_names.back().c_str();

			dst_element.SemanticIndex = src_element.semantic_index;
			dst_element.Format = GetDXGIFormat(src_element.format);
			dst_element.InputSlot = src_element.input_slot;
			dst_element.AlignedByteOffset = src_element.aligned_offset;
			dst_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			dst_element.InstanceDataStepRate = 0;
		}

		pipeline_state_desc.InputLayout.NumElements = desc.vertex_layout.element_count;
		pipeline_state_desc.InputLayout.pInputElementDescs = input_elements;
		
		pipeline_state_desc.NumRenderTargets = desc.render_target_count;

		pipeline_state_desc.DSVFormat = GetDXGIFormat(desc.depth_format);
		pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_state_desc.DepthStencilState.DepthEnable = desc.depth_enable;
		pipeline_state_desc.DepthStencilState.StencilEnable = false;

		// ブレンドステート
		pipeline_state_desc.BlendState.AlphaToCoverageEnable = desc.alpha_to_coverage_enable;
		pipeline_state_desc.BlendState.IndependentBlendEnable = desc.independent_blend_enable;

		for (UInt32 render_target_index = 0; render_target_index < desc.render_target_count; render_target_index++)
		{
			pipeline_state_desc.RTVFormats[render_target_index] = GetDXGIFormat(desc.render_targets[render_target_index]);

			pipeline_state_desc.BlendState.RenderTarget[render_target_index] = GetBlendDesc(desc.blend_states[render_target_index]);
		}

		// ラスタライザーステート
		pipeline_state_desc.RasterizerState = GetDefaultRasterizerDesc();

		pipeline_state_desc.RasterizerState.FillMode = GetD3D12FillMode(desc.fill_mode);
		pipeline_state_desc.RasterizerState.CullMode = GetD3D12CullMode(desc.cull_mode);
		pipeline_state_desc.RasterizerState.FrontCounterClockwise = desc.rasterizer_state.front_counter_clockwise ? true : false;
		pipeline_state_desc.RasterizerState.DepthBias = desc.rasterizer_state.depth_bias;
		pipeline_state_desc.RasterizerState.DepthBiasClamp = desc.rasterizer_state.depth_bias_clamp;
		pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = desc.rasterizer_state.slope_scaled_depth_bias;
		pipeline_state_desc.RasterizerState.DepthClipEnable = desc.rasterizer_state.depth_clip_enable ? true : false;
		pipeline_state_desc.RasterizerState.MultisampleEnable = desc.rasterizer_state.multisample_enable ? true : false;
		pipeline_state_desc.RasterizerState.AntialiasedLineEnable = desc.rasterizer_state.antialiased_line_enable ? true : false;
		pipeline_state_desc.RasterizerState.ForcedSampleCount = desc.rasterizer_state.forced_sample_count;
		pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		
		pipeline_state_desc.SampleDesc.Count = 1;
		pipeline_state_desc.SampleMask = UINT_MAX;

		HRESULT hr = device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&m_pipeline_state));

*/