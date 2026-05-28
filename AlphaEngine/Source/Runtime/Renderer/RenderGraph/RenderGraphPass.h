#pragma once


namespace Alpha
{
	// 実行するタイミング
	//enum class Execute
	//{
	//
	//};

	class D3D12GraphicsPipelineState;
	class D3D12EffectShader;

	class RenderCommand
	{
		// std::function<void()>;
		// priority;
		// operator<() priority比較
	};

	class RenderGraphPass
	{
	private:
		D3D12GraphicsPipelineState* m_pipeline;
		D3D12EffectShader* m_shader;
		//std::vector<RenderCommand*>
	public:
		RenderGraphPass();
		~RenderGraphPass();

		//void SetPipeline(D3D12CommandList* list);

	};

	/*
	本来はこのパスを継承したクラスを作成し
	パス特有の処理(コマンド積み)を行う必要がある

	*/
}