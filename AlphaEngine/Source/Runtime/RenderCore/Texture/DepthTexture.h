#pragma once
#include "Texture.h"

namespace Alpha
{
	class DepthTexture : public Texture
	{
	private:
		DescriptorHandle m_dsv_handle{ nullptr };

	public:
		DepthTexture();
		~DepthTexture();

		struct Args
		{
			Texture::Args base{};
			DescriptorHeap* dsv_heap{ nullptr };
		};
		// 通常の初期化
		void Initialize(const Args& args);

		DescriptorHandle GetDSVHandle() const { return m_dsv_handle; }

	};

}