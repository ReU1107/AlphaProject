#pragma once
#include "Declarations.h"

namespace Alpha
{
	/*
	* シェーダーパイプラインで使用するレイアウト
	* 定数バッファやテクスチャ、サンプラー等のリソースのバインド情報を持つ
	*/
	class RHIPipelineLayout
	{
	public:
		virtual ~RHIPipelineLayout() {}

		virtual bool Initialize(const RHIPipelineLayoutDesc& desc) = 0;

	};

}