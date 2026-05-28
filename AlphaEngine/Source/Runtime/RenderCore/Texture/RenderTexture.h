#pragma once
#include "Texture.h"

namespace Alpha
{
	class SwapChain;

	class RenderTexture : public Texture
	{
	private:
		DescriptorHandle m_rtv_handle{};

	public:
		RenderTexture();
		~RenderTexture();

		struct Args
		{ 
			Texture::Args base{};
			DescriptorHeap* rtv_heap{ nullptr };

			SwapChain* swap_chain{ nullptr };
			UInt32 buffer_index{ 0 };
		};

		// SwapChain から初期化(RenderCoreの初期化時にしか呼ばれない)
		void InitializeFromSwapChain(const Args& args);
		// 通常の初期化
		void Initialize(const Args& args);

		DescriptorHandle GetRTVHandle() const { return m_rtv_handle; }
	};

}