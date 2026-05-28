#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHISwapChain;

	class CommandQueue;

	class SwapChain
	{
	private:
		TUnique<RHISwapChain> m_rhi{ nullptr };

	public:
		SwapChain();
		~SwapChain();

		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			CommandQueue* queue{ nullptr };
			void* window_handle{ nullptr };

			UInt32 buffer_count{ 3 };
			UInt32 width{ 1920 };
			UInt32 height{ 1080 };
		};

		void Initialize(const Args& args);

		RHISwapChain* GetRHI() const;

		UInt32 GetCurrentBackBufferIndex() const;

		void Present();

	};
}