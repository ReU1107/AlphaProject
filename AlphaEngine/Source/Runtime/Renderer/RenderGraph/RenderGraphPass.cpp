
#include "RenderGraphPass.h"

//#include "D3D12/D3D12Subsystem.h"
//#include "D3D12/D3D12PipelineState.h"
//#include "D3D12/D3D12CommandList.h"
//#include "D3D12/D3D12Shader.h"

namespace Alpha
{
	RenderGraphPass::RenderGraphPass()
	{
		//m_pipeline = D3D12Subsystem::Get().CreateGraphicsPipelineState();

		//std::wstring file = L"C:/Users/owner/source/repos/ModelViewer/GraphicsLibrary/TestShader.hlsl";

		//m_shader = D3D12Subsystem::Get().CreateEffectShader(file, D3D12ShaderType::Vertex | D3D12ShaderType::Pixel);

		//m_pipeline->Create(m_shader);
	}

	RenderGraphPass::~RenderGraphPass()
	{
		delete m_shader;
		delete m_pipeline;
	}

	//void RenderGraphPass::SetPipeline(D3D12CommandList* list)
	//{
	//	list->SetGraphicsPipeline(m_pipeline);
	//	list->SetRootSignature(m_shader->GetRootSignature());
	//}


	// シェーダーやパイプラインが同じ場合
	class RenderPacket
	{

	};

}